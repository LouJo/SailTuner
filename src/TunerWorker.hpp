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

#ifndef _TUNER_WORKER_HPP
#define _TUNER_WORKER_HPP

#include <QObject>
#include <QMutex>
#include <QStringList>
#include <QVector>
#include <QPointer>
#include <QElapsedTimer>
#include <QTimer>
#include <QEventLoop>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QIODevice>
#include <fstream>
#include <stdint.h>

#include "PitchDetection.hpp"
#include "audio/FreqPlayer.hpp"

/**
 * Worker class to receive audio and
 * do pitch detection in a thread
 *
 * Audio capture and playback go through Qt Multimedia (QAudioInput /
 * QAudioOutput): a timer pumps the stream
 */
class TunerWorker : public QObject {
	Q_OBJECT

	private:
	/// time between dbus calls that prevent screen blanking
	static const int nbSecPreventBlanking = 40;
	/// nb of audio samples to write to the playback device at once
	static const int nbSampleBuffer = 512;
	/// maximum number of samples read from the microphone per pump
	static const int kChunk = 4096;
	/// pump period in milliseconds: one nbSampleBuffer frame at
	/// PitchDetection::rate, so playback is fed at its real time rate
	static const int kPumpMs = nbSampleBuffer * 1000 / PitchDetection::rate;
	/// time a stream may stay silent before retrying another format (ms)
	static const int kWatchMs = 2500;
	/// minimum delay between two open attempts (ms)
	static const int kRetryMs = 1000;

	static const char *filename_record;

	QMutex mutex;

	int play_stop_counter;
	bool running, playing, quit;

	// to update vars
	double la_to_update;
	int temperament_to_update, note_to_update, octave_to_update;

	// worker thread state
	PitchDetection *pitch;
	FreqPlayer<int16_t> *player;
	std::ofstream *record_file;
	PitchDetection::PitchResult result;

	// capture (QAudioInput, worker thread)
	QAudioInput *audio_in;
	QPointer<QIODevice> in_source;
	int in_rate;
	qint64 bytes_seen;
	bool tried_preferred_format;
	QElapsedTimer record_retry;
	QVector<qint16> scratch;
	QVector<qint16> buf16;

	// playback (QAudioOutput, worker thread)
	QAudioOutput *audio_out;
	QPointer<QIODevice> out_source;
	int out_rate;
	QElapsedTimer play_retry;

	// screen blanking
	bool blank_asserted;
	QElapsedTimer blank_timer;

	// timers (worker thread, created in Entry)
	QTimer *pump;
	QTimer *watchdog;

	QEventLoop *loop;

	public:
	/// constructor
	TunerWorker();
	~TunerWorker();

	public slots:
	void Start();
	void Stop();
	void SetPlaying(bool p);
	void SetTemperamentIndex(int idx);
	void SetLa(double la);
	void SetNote(int note);
	void SetOctave(int octave);
	void Entry();
	void Quit();
	void SetNoteOctave(int note, int octave);

	/// write a file with raw audio
	static void set_record(const char *filename_record);

	signals:
	void resultUpdated(const PitchDetection::PitchResult &result);
	void temperamentListUpdated(const QStringList &list);

	private slots:
	/// pump the audio devices (timer + readyRead)
	void pumpAudio();
	/// capture stream state change
	void recordStateChanged(QAudio::State state);
	/// playback stream state change
	void playStateChanged(QAudio::State state);
	/// the capture stream has been silent too long
	void checkForSilence();

	private:
	/// open the capture stream
	bool openRecord(bool preferred);
	/// close the capture stream
	void closeRecord();
	/// open the playback stream
	bool openPlayback();
	/// close the playback stream
	void closePlayback();
};

#endif
