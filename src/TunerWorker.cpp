/*
 * Copyright 2016 (C) Louis-Joseph Fournier
 * Copyright 2026 (C) Mark Washeim
 *
 * louisjoseph.fournier@gmail.com
 * blueprint@poetaster.de
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

#include <algorithm>
#include <iostream>
#include <stdint.h>

#include "audio/FreqPlayer.hpp"
#include "TunerWorker.hpp"

using namespace std;

/// linear resampling of a mono 16-bit buffer; used when the device
/// negotiates a rate different from PitchDetection::rate
static QVector<qint16> resampleLinear(const qint16 *src, int n, int fromHz, int toHz)
{
	QVector<qint16> out;
	if (n <= 0) return out;
	if (fromHz == toHz) {
		out.resize(n);
		std::copy(src, src + n, out.data());
		return out;
	}
	const double step = double(fromHz) / double(toHz);
	const int out_n = int(double(n) * step);
	out.reserve(out_n);
	double pos = 0;
	for (int i = 0; i < out_n; ++i) {
		const int idx = int(pos);
		if (idx >= n - 1)
			out.append(src[n - 1]);
		else
			out.append(qint16(int(src[idx] + (src[idx + 1] - src[idx]) * (pos - idx))));
		pos += step;
	}
	return out;
}

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
	octave_to_update(-1),
	pitch(nullptr),
	player(nullptr),
	record_file(nullptr),
	audio_in(nullptr),
	in_rate(0),
	bytes_seen(0),
	tried_preferred_format(false),
	audio_out(nullptr),
	out_rate(0),
	blank_asserted(false),
	pump(nullptr),
	watchdog(nullptr),
	loop(nullptr)
{
	scratch.resize(kChunk);
}

TunerWorker::~TunerWorker()
{
}

void TunerWorker::Start()
{
	cerr << __func__ << endl;
	mutex.lock();
	running = true;
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
	}
	else {
		// stop after a frame
		play_stop_counter = 1;
	}
	mutex.unlock();
}

void TunerWorker::Quit()
{
	QEventLoop *l = nullptr;
	mutex.lock();
	quit = true;
	l = loop;
	mutex.unlock();

	// deliver the quit to the event loop running in the worker thread
	if (l) QMetaObject::invokeMethod(l, "quit", Qt::QueuedConnection);
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

void TunerWorker::SetNoteOctave(int note, int octave)
{
	mutex.lock();
	note_to_update = note;
	octave_to_update = octave;
	mutex.unlock();
}

void TunerWorker::Entry()
{
	cerr << __func__ << endl;

	// exit if already asked to quit
	{
		QMutexLocker lock(&mutex);
		if (quit) return;
	}

	pitch = new PitchDetection();
	emit temperamentListUpdated(pitch->GetTemperamentList());

	player = new FreqPlayer<int16_t>(PitchDetection::rate);

	if (filename_record) record_file = new ofstream(filename_record);

	// prevent lpm if running on startup
	bool started_active = false;
	{
		QMutexLocker lock(&mutex);
		started_active = running || playing;
	}
	if (started_active) {
		blank_asserted = true;
		blank_timer.start();
		blank_prevent(true);
	}

    // idle time is spent in the event loop,
    // the pump timer and the readyRead() notifications wake it
	QEventLoop l;
	{
		QMutexLocker lock(&mutex);
		if (quit) {
			delete player;
			delete pitch;
			if (record_file) {
				record_file->close();
				delete record_file;
			}
			return;
		}
		loop = &l;
	}

	// create the timers here: a QTimer must be started on its own thread,
    // and Entry() runs on the worker thread

	pump = new QTimer(this);
	pump->setInterval(kPumpMs);
	connect(pump, &QTimer::timeout, this, &TunerWorker::pumpAudio);
	watchdog = new QTimer(this);
	watchdog->setSingleShot(true);
	watchdog->setInterval(kWatchMs);
	connect(watchdog, &QTimer::timeout, this, &TunerWorker::checkForSilence);

	pump->start();
	l.exec();

	{
		QMutexLocker lock(&mutex);
		loop = nullptr;
	}
	pump->stop();

	closeRecord();
	closePlayback();

	delete player;
	delete pitch;

	if (record_file) {
		record_file->close();
		delete record_file;
	}
	blank_prevent(false);
}

/// pump the audio devices: apply config, read the microphone, analyse,
/// play the reference tone, and keep the screen from blanking
void TunerWorker::pumpAudio()
{
	bool do_run = false, do_play = false;
	{
		QMutexLocker lock(&mutex);
		// stop playing after a frame
		if (play_stop_counter >= 2) playing = false;

		// update config
		if (la_to_update) {
			pitch->SetLa(la_to_update);
			la_to_update = 0;
		}
		if (temperament_to_update != -1) {
			pitch->SetTemperament(temperament_to_update);
			temperament_to_update = -1;
		}
		if (note_to_update != -1) {
			result.note = note_to_update;
			note_to_update = -1;
			player->SetFreq(pitch->GetNoteFreq(result.note, result.octave));
		}
		if (octave_to_update != -1) {
			result.octave = octave_to_update;
			octave_to_update = -1;
			player->SetFreq(pitch->GetNoteFreq(result.note, result.octave));
		}

		do_run = running;
		do_play = playing;
	}

	// ---------------- capture ----------------
	if (do_run) {
		// reopen if the stream is gone or its QIODevice was destroyed
		if (!audio_in || !in_source) {
			if (audio_in)
				closeRecord();
			// open failed before: retry at most once per kRetryMs
			if (!record_retry.isValid() || record_retry.elapsed() >= kRetryMs) {
				record_retry.start();
				openRecord(tried_preferred_format);
			}
		}

		if (in_source) {
            // drain whatever is available; no bytesAvailable() check,
			int total = 0;
			while (total < kChunk) {
				const qint64 r = in_source->read(reinterpret_cast <char *> (scratch.data() + total),
						qint64(kChunk - total) * qint64(sizeof(qint16)));
				if (r <= 0) break;
				total += int(r / sizeof(qint16));
			}

			if (total > 0) {
				bytes_seen += total;
				watchdog->start();

				const qint16 *data = scratch.constData();
				int n = total;
				if (in_rate != PitchDetection::rate) {
					// the device negotiated another rate: resample
					buf16 = resampleLinear(scratch.constData(), total, in_rate, PitchDetection::rate);
					data = buf16.constData();
					n = buf16.size();
				}

				// record in file if needed
				if (record_file) record_file->write((const char *) data, n * int(sizeof(qint16)));

				pitch->AudioAnalyse(data, n);

				PitchDetection::PitchResult res;
				if (pitch->GetResultUpdated(res)) {
					if (res.found) cout << Scale::NoteName(res.note) << " " << res.frequency << endl;
					result = res;
					emit resultUpdated(res);
				}
			}
		}
	}
	else if (audio_in) {
		// not running anymore: release the microphone
		tried_preferred_format = false;
		closeRecord();
	}

	// ---------------- playback ----------------
	bool close_play = false;
	if (do_play) {
		// reopen if the stream is gone or its QIODevice was destroyed
		if (!audio_out || !out_source) {
			if (audio_out)
				closePlayback();
			if (!play_retry.isValid() || play_retry.elapsed() >= kRetryMs) {
				play_retry.start();
				openPlayback();
			}
		}

		if (out_source) {
			int16_t buf[nbSampleBuffer];
			int stop_counter;
			{
				QMutexLocker lock(&mutex);
				stop_counter = play_stop_counter;
				if (stop_counter > 0) play_stop_counter = stop_counter + 1;
			}

			player->WriteAudio(buf, nbSampleBuffer, stop_counter > 0);

			const qint16 *data = buf;
			int n = nbSampleBuffer;
			QVector<qint16> tmp;
			if (out_rate != PitchDetection::rate) {
				// the device negotiated another rate: resample
				tmp = resampleLinear(buf, nbSampleBuffer, PitchDetection::rate, out_rate);
				data = tmp.constData();
				n = tmp.size();
			}
			if (out_source->write(const_cast <char *> (reinterpret_cast <const char *> (data)),
						qint64(n) * qint64(sizeof(qint16))) < 0) {
				cerr << "audio write failed" << endl;
			}
		}
	}
	else {
		// keep counting until the stream is released
		QMutexLocker lock(&mutex);
		play_stop_counter++;
		close_play = (play_stop_counter >= 10) && (audio_out != nullptr);
	}
	if (close_play) closePlayback();

	// ---------------- screen blanking ----------------
	if (do_run || do_play) {
		if (!blank_asserted) {
			blank_asserted = true;
			blank_timer.start();
			blank_prevent(true);
		}
		else if (blank_timer.elapsed() >= qint64(nbSecPreventBlanking) * 1000) {
			blank_timer.restart();
			blank_prevent(true);
		}
	}
	else if (blank_asserted) {
		blank_asserted = false;
		blank_prevent(false);
	}
}

/// open the capture stream, negotiating the format with the device
bool TunerWorker::openRecord(bool preferred)
{
	const QAudioDeviceInfo device = QAudioDeviceInfo::defaultInputDevice();
	if (device.isNull()) {
		cerr << "no input device" << endl;
		return false;
	}

	QAudioFormat format;
	if (preferred) {
		// second attempt: stop arguing with the device and take what it likes
		format = device.preferredFormat();
	}
	else {
		format.setSampleRate(PitchDetection::rate);
		format.setChannelCount(1);
		format.setSampleSize(16);
		format.setCodec(QStringLiteral("audio/pcm"));
		format.setByteOrder(QAudioFormat::LittleEndian);
		format.setSampleType(QAudioFormat::SignedInt);
		if (!device.isFormatSupported(format))
			format = device.nearestFormat(format);
	}

	// we only know how to handle mono 16-bit signed
	if (format.sampleSize() != 16
			|| format.sampleType() != QAudioFormat::SignedInt
			|| format.channelCount() != 1) {
		cerr << "unsupported input format" << endl;
		return false;
	}

	// whatever rate we end up with is what we divide by
	in_rate = format.sampleRate();

	audio_in = new QAudioInput(device, format, this);
	connect(audio_in, &QAudioInput::stateChanged, this, &TunerWorker::recordStateChanged);

	in_source = audio_in->start();
	if (!in_source) {
		cerr << "could not open microphone" << endl;
		audio_in->deleteLater();
		audio_in = nullptr;
		return false;
	}

	// readyRead() is a hint only, the timer is the real mechanism
	connect(in_source.data(), &QIODevice::readyRead, this, &TunerWorker::pumpAudio);

	// reset detection, as when the old code started the pulseaudio stream
	{
		QMutexLocker lock(&mutex);
		pitch->Reset();
		result.found = false;
		emit resultUpdated(result);
	}

	bytes_seen = 0;
	watchdog->start();

	cerr << "capturing " << in_rate << " Hz" << endl;
	return true;
}

void TunerWorker::closeRecord()
{
	watchdog->stop();
	in_source.clear();
	if (audio_in) {
		audio_in->stop();
		delete audio_in;
		audio_in = nullptr;
	}
	bytes_seen = 0;
}

void TunerWorker::recordStateChanged(QAudio::State state)
{
	if (!audio_in)
		return;

	if (state == QAudio::StoppedState && audio_in->error() != QAudio::NoError)
		cerr << "record stopped with error " << audio_in->error() << endl;

	// do NOT drop the stream here: some backends pass through
	// Stopped/Idle states asynchronously right after start(). A dead
	// stream is handled by the pump (the QIODevice goes away, the
	// stream is reopened) and by the watchdog (no bytes arrive).
}

void TunerWorker::checkForSilence()
{
	if (bytes_seen > 0)
		return;

	if (!tried_preferred_format) {
		tried_preferred_format = true;
		cerr << "no audio after " << kWatchMs << " ms, retrying with the device format" << endl;
		closeRecord();
		if (openRecord(true))
			watchdog->start();
		return;
	}

	cerr << "still no audio from the microphone, check the audio sources" << endl;
}

/// open the playback stream
bool TunerWorker::openPlayback()
{
	const QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice();
	if (device.isNull()) {
		cerr << "no output device" << endl;
		return false;
	}

	QAudioFormat format;
	format.setSampleRate(PitchDetection::rate);
	format.setChannelCount(1);
	format.setSampleSize(16);
	format.setCodec(QStringLiteral("audio/pcm"));
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::SignedInt);
	if (!device.isFormatSupported(format))
		format = device.nearestFormat(format);

	if (format.sampleSize() != 16
			|| format.sampleType() != QAudioFormat::SignedInt
			|| format.channelCount() != 1) {
		cerr << "unsupported output format" << endl;
		return false;
	}

	out_rate = format.sampleRate();

	audio_out = new QAudioOutput(format, this);
	connect(audio_out, &QAudioOutput::stateChanged, this, &TunerWorker::playStateChanged);

	out_source = audio_out->start();
	if (!out_source) {
		cerr << "could not open playback" << endl;
		audio_out->deleteLater();
		audio_out = nullptr;
		return false;
	}

	// update frequency to update from previous tuner results
	player->Reset();
	player->SetFreq(pitch->GetNoteFreq(result.note, result.octave));

	cerr << "playing " << out_rate << " Hz" << endl;
	return true;
}

void TunerWorker::closePlayback()
{
	if (audio_out) {
		audio_out->stop();
		delete audio_out;
		audio_out = nullptr;
	}
	out_source.clear();
}

void TunerWorker::playStateChanged(QAudio::State state)
{
	if (!audio_out)
		return;

	if (state == QAudio::StoppedState && audio_out->error() != QAudio::NoError)
		cerr << "playback stopped with error " << audio_out->error() << endl;

	// like recordStateChanged: a dead stream is handled by the pump
}

/// Set a filename before instanciation to record raw audio stream

void TunerWorker::set_record(const char *f)
{
	filename_record = f;
}

