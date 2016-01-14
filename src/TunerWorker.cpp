/* Copyright 2016 (C) Louis-Joseph Fournier 
 * louisjoseph.fournier@gmail.com
 *
 * This file is part of SailTuner.
 *
 * SailTuner is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SailTuner is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <QDBusConnection>
#include <QDBusInterface>

#include <iostream>
#include <fstream>
#include <stdint.h>

extern "C" {
#include <pulse/simple.h>
}

#include "audio/FreqPlayer.hpp"
#include "TunerWorker.hpp"

#define name_(x) #x
#define name(x) name_(x)
#define NAME name(TARGET)

using namespace std;

/// file name to record audio

const char * TunerWorker::filename_record = NULL;

/// function to prevent screen blank on Sailfish OS

static void blank_prevent(bool prevent)
{
	cerr << __func__ << " " << prevent << endl;
	QDBusConnection system = QDBusConnection::connectToBus(QDBusConnection::SystemBus, "system");
	QDBusInterface interface("com.nokia.mce", "/com/nokia/mce/request", "com.nokia.mce.request", system);

	if (prevent) {
		interface.call(QLatin1String("req_display_blanking_pause"));
	} else {
		interface.call(QLatin1String("req_display_cancel_blanking_pause"));
	}
}

TunerWorker::TunerWorker() :
	play_stop_counter(0),
	running(false),
	playing(false),
	quit(false),
	la_to_update(0),
	temperament_to_update(0), // update the first time in every cases
	note_to_update(-1),
	octave_to_update(-1)
{
	//qRegisterMetaType<PitchDetection::PitchResult>("PitchDetection::PitchResult");
}

TunerWorker::~TunerWorker()
{
}

void TunerWorker::Start()
{
	cerr << __func__ << endl;
	mutex.lock();
	running = true;
	condition.wakeOne();
	mutex.unlock();
}

void TunerWorker::Stop()
{
	cerr << __func__ << endl;
	mutex.lock();
	running = false;
	mutex.unlock();
}

void TunerWorker::SetPlaying(bool p)
{
	cerr << __func__ << " " << p << endl;
	if (p == playing) return;
	mutex.lock();
	if (p) {
		playing = p;
		play_stop_counter = 0;
		condition.wakeOne();
	}
	else {
		// stop after a frame
		play_stop_counter = 1;
	}
	mutex.unlock();
}

void TunerWorker::Quit()
{
	mutex.lock();
	quit = true;
	condition.wakeOne();
	mutex.unlock();
}

void TunerWorker::SetLa(double la)
{
	mutex.lock();
	la_to_update = la;
	mutex.unlock();
}

void TunerWorker::SetTemperamentIndex(int idx)
{
	mutex.lock();
	temperament_to_update = idx;
	mutex.unlock();
}

void TunerWorker::SetNote(int note)
{
	mutex.lock();
	note_to_update = note;
	mutex.unlock();
}


void TunerWorker::SetOctave(int octave)
{
	mutex.lock();
	octave_to_update = octave;
	mutex.unlock();
}

void TunerWorker::Entry()
{
	cerr << __func__ << endl;

	int nbSamplePreventBlanking = nbSecPreventBlanking * PitchDetection::rate;
	int nb_sample_running = 0;

	int16_t *buffer = new int16_t[nbSampleBuffer];

	PitchDetection::PitchResult result;

	ofstream *file = NULL;
	if (filename_record) file = new ofstream(filename_record);

	PitchDetection *pitchDetection = new PitchDetection();
	emit temperamentListUpdated(pitchDetection->GetTemperamentList());

	FreqPlayer<int16_t> *player = new FreqPlayer<int16_t>(PitchDetection::rate);

	// pulseaudio
	pa_simple *p_record = NULL, *p_play = NULL;
	pa_sample_spec p_spec;

	p_spec.format = PA_SAMPLE_S16NE;
	p_spec.channels = 1;
	p_spec.rate = PitchDetection::rate;

	// prevent lpm if running on startup
	if (running || playing) blank_prevent(true);

	while (1) {

		mutex.lock();
		// stop playing after a frame
		if (play_stop_counter >= 2) playing = false;

		// free pulseaudio if not running
		if (!running && p_record) {
			pa_simple_free(p_record);
			p_record = nullptr;
		}
		// free playing after a delay of inactivity to avoid clac

		// wait for running
		if (!running && !playing) {
			blank_prevent(false);
			bool waked;
			while (!running && !playing && !quit) {
				waked = condition.wait(&mutex, p_play ? 1000000 : ULONG_MAX);
				// free playing now
				if (!waked && p_play) {
					pa_simple_free(p_play);
					p_play = nullptr;
				}
			}
			cerr << "wake-up" << endl;
			nb_sample_running = 0;
			if (!quit) blank_prevent(true);
		}
		if (quit) {
			mutex.unlock();
			break;
		}

		// update config
		if (la_to_update) {
			pitchDetection->SetLa(la_to_update);
			la_to_update = 0;
		}
		if (temperament_to_update != -1) {
			pitchDetection->SetTemperament(temperament_to_update);
			temperament_to_update = -1;
		}
		if (note_to_update != -1) {
			result.note = note_to_update;
			note_to_update = -1;
			player->SetFreq(pitchDetection->GetNoteFreq(result.note, result.octave));
		}
		if (octave_to_update != -1) {
			result.octave = octave_to_update;
			octave_to_update = -1;
			player->SetFreq(pitchDetection->GetNoteFreq(result.note, result.octave));
		}
		bool m_running = running;
		bool m_playing = playing;
		mutex.unlock();

		if (m_running ) {
			// tuner detection running

			if (!p_record) {
				// start pulseaudio if stopped
				p_record = pa_simple_new(
						NULL,
						NAME,
						PA_STREAM_RECORD,
						NULL,
						"Tuner record",
						&p_spec,
						NULL,
						NULL,
						NULL
						);
				// reset detection
				pitchDetection->Reset();
				result.found = false;
				emit resultUpdated(result);
			}

			// get audio data
			int size = pa_simple_read(p_record, (void*) buffer, nbSampleBuffer << 1, NULL);
			if (size < 0) {
				cerr << "audio read failed " << size << endl;
				continue;
			}
			//cerr << "read " << nb_sample_running << endl;

			// record in file is needed
			if (file) file->write((char*) buffer, nbSampleBuffer << 1);

			pitchDetection->AudioAnalyse(buffer, nbSampleBuffer);

			if (pitchDetection->GetResultUpdated(result)) {
				if (result.found) cout << Scale::NoteName(result.note) << " " << result.frequency << endl;
				emit resultUpdated(result);
			}
		} // running

		if (m_playing) {
			// play
			if (!p_play) {
				// start pulseaudio if stopped
				p_play = pa_simple_new(
						NULL,
						NAME,
						PA_STREAM_PLAYBACK,
						NULL,
						"Tuner playback",
						&p_spec,
						NULL,
						NULL,
						NULL
						);

				// update frequency to update from previous tuner results
				player->Reset();
				player->SetFreq(pitchDetection->GetNoteFreq(result.note, result.octave));
			}

			const int stop_counter = play_stop_counter;

			player->WriteAudio(buffer, nbSampleBuffer, stop_counter > 0);
			if (pa_simple_write(p_play, buffer, nbSampleBuffer << 1, nullptr) < 0) {
				cerr << "audio write failed" << endl;
			}

			if (stop_counter) play_stop_counter = stop_counter + 1;

		} // playing

		// prevent screen blanking
		nb_sample_running += nbSampleBuffer;
		if (nb_sample_running >= nbSamplePreventBlanking && (running || playing)) {
			nb_sample_running = 0;
			blank_prevent(true);
		}
	}

	if (p_record) pa_simple_free(p_record);
	if (p_play) pa_simple_free(p_play);

	delete player;
	delete pitchDetection;
	delete buffer;

	if (file) {
		file->close();
		delete file;
	}
	blank_prevent(false);
}

/// Set a filename before instanciation to record raw audio stream

void TunerWorker::set_record(const char *f)
{
	filename_record = f;
}

