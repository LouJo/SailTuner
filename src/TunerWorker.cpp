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

#include "TunerWorker.hpp"

using namespace std;

// high 10hz / 16k
static double a10[] = { 1        , -2.99214602,  2.98432286, -0.99217678 };
static double b10[] = { 0.99608071, -2.98824212,  2.98824212, -0.99608071 };

const char * TunerWorker::filename_record = NULL;

/// function to prevent screen blank on Sailfish OS

static void blank_prevent(bool prevent)
{
	cerr << __func__ << endl;
	QDBusConnection system = QDBusConnection::connectToBus(QDBusConnection::SystemBus, "system");
	QDBusInterface interface("com.nokia.mce", "/com/nokia/mce/request", "com.nokia.mce.request", system);

	if (prevent) {
		interface.call(QLatin1String("req_display_blanking_pause"));
	} else {
		interface.call(QLatin1String("req_display_cancel_blanking_pause"));
	}
}

TunerWorker::TunerWorker() :
	high_filter(NULL),
	cross(NULL),
	scale(NULL),
	running(false),
	quit(false),
	la_to_update(0),
	freq_to_update(NULL)
{
	// part of reset
	found = false;
	count_found = count_not_found = 0;
	nb_sample_running = 0;
	note_found = octave_found = -1;
	ResetDeviation();
}

TunerWorker::~TunerWorker()
{
	if (filename_record && file_record.is_open()) file_record.close();
	if (high_filter) delete high_filter;
	if (cross) delete cross;
	if (scale) delete scale;
}

/// reset analyse values
void TunerWorker::Reset()
{
	found = false;
	count_found = count_not_found = 0;
	nb_sample_running = 0;
	note_found = octave_found = -1;
	ResetDeviation();
	blank_prevent(true);
	high_filter->Clear();
	cross->Clear();
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
	mutex.lock();
	running = false;
	mutex.unlock();
}

void TunerWorker::Quit()
{
	mutex.lock();
	running = false;
	quit = true;
	condition.wakeOne();
	mutex.unlock();
}

void TunerWorker::SetNotesFrequencies(const double *notes_freq)
{
	mutex.lock();
	freq_to_update = notes_freq;
	mutex.unlock();
}

void TunerWorker::SetLa(double la)
{
	mutex.lock();
	la_to_update = la;
	mutex.unlock();
}

void TunerWorker::ComputeFrame(int16_t v)
{
	v = (*high_filter)(v);
	(*cross)(v);
}

void TunerWorker::ResetDeviation()
{
	// reset deviation values
	nb_deviation = 0;
	deviation_start = 0;
	deviation_sum = 0;
}

void TunerWorker::UpdateDeviation(double d)
{
	if (nb_deviation == nbDeviationValues) {
		deviation_sum -= deviation_values[deviation_start];
		deviation_start = (deviation_start + 1) % nbDeviationValues;
		nb_deviation--;
	}
	deviation_values[(deviation_start + nb_deviation) % nbDeviationValues] = d;
	nb_deviation++;
	deviation_sum += d;
}

void TunerWorker::SetFound(int n, int o, double d)
{
	if (n != note_found || o != octave_found) {
		note_found = n;
		octave_found = o;
		count_found = 0;
		SetNotFound();

		ResetDeviation();
		UpdateDeviation(d);
	}
	else if (count_found++ >= nbConfirm) {
		found = true;
		count_not_found = 0;
		UpdateDeviation(d);

		resultFound(n, o, deviation_sum / nb_deviation, cross->Freq());
	}
	else {
		UpdateDeviation(d);
	}
}

void TunerWorker::SetNotFound()
{
	if (count_not_found++ >= nbDefect) {
		count_found = 0;
		if (found) {
			found = false;
			ResetDeviation();
			resultNotFound(cross->Freq());
		}
	}
}

void TunerWorker::AudioAnalyse(const int16_t *ptr, int nb_frame)
{
	nb_sample_running += nb_frame;

	// record in file is needed
	if (filename_record && file_record.is_open()) file_record.write((char*) ptr, nb_frame * sizeof(int16_t));

	// compute every audio frame
	while (nb_frame--) ComputeFrame(*ptr++);

	// find note, octave, deviation
	if (cross->Freq()) {
		int n, o = 0;
		double d = 0;
		n = scale->FindNote(cross->Freq(), o, d);
		SetFound(n, o, d);
	}
	else { // no freq
		SetNotFound();
	}

	// prevent screen blanking
	if (nb_sample_running >= nbSamplePreventRunning && running) {
		nb_sample_running = 0;
		blank_prevent(true);
	}
}


void TunerWorker::Entry()
{
	// initialisations
	if (filename_record) file_record.open(filename_record);

	high_filter = new LinearFilter<int16_t>(3, a10, b10);

	ZeroCross<int16_t>::Config cross_config({rate, defaultNbFrame, defaultFreqMin, defaultFreqMax});
	cross = new ZeroCross<int16_t>(cross_config);

	scale = new Scale();

	while (1) {
		// wait for running
		mutex.lock();
		if (!running) {
			blank_prevent(false);
			while (!running && !quit) condition.wait(&mutex);
			// reset operations on start
			if (!quit) Reset();
		}
		if (quit) {
			mutex.unlock();
			break;
		}
		// update config
		if (la_to_update) {
			scale->SetLa(la_to_update);
			la_to_update = 0;
		}
		if (freq_to_update) {
			scale->SetNotesFrequencies(freq_to_update);
			freq_to_update = NULL;
		}
		mutex.unlock();

		std::cout << __func__ << " do job" << std::endl;
	}
}

/// Set a filename to record raw audio stream

void TunerWorker::set_record(const char *f)
{
	filename_record = f;
}

/// for analyse_file console logs
static void display_results(int note, int octave, double deviation, double frequency)
{
	cout << frequency << " " << Scale::NoteName(note) << " " << octave << " " << deviation << endl;
}

static void display_no_results(double freq)
{
	cout << freq << endl;
}

/// analyse a file (static function)
void TunerWorker::analyse_file(const char *filename)
{
	cout << "analyse file " << filename << endl;
	ifstream fin;
	fin.open(filename);

	const int nb_frame = 1024;
	TunerWorker *tuner = new TunerWorker();
	int16_t buffer[nb_frame];

	connect(tuner, &TunerWorker::resultFound, NULL, display_results);
	connect(tuner, &TunerWorker::resultNotFound, NULL, display_no_results);

	while (1) {
		fin.read((char*) buffer, sizeof(buffer));
		tuner->AudioAnalyse(buffer, sizeof(buffer) >> 1);
//		cout << "." << endl;

		if (fin.eof()) break;
	}
	fin.close();
	delete tuner;
}
