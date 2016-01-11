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

#ifndef _TUNER_WORKER_HPP
#define _TUNER_WORKER_HPP

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <QStringList>

#include "PitchDetection.hpp"

/**
 * Worker class to receive audio and 
 * do pitch detection in a thread
 *
 */
class TunerWorker : public QObject {
	Q_OBJECT

	private:
	/// time beetween dbus signals to prevent screen blanking
	static const int nbSecPreventBlanking = 40;
	/// nb of audio sample to read from pulseaudio at once
	static const int nbSampleBuffer = 512;

	static const char *filename_record;

	QMutex mutex;
	QWaitCondition condition;

	bool running, playing, quit;

	// to update vars
	double la_to_update;
	int temperament_to_update, note_to_update, octave_to_update;

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

	/// write a file with raw audio
	static void set_record(const char *filename_record);

	signals:
	void resultUpdated(const PitchDetection::PitchResult &result);
	void temperamentListUpdated(const QStringList &list);
};

#endif
