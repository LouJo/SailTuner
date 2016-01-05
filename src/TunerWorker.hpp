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
	static const int nbSecPreventRunning = 40;
	static const int nbSampleBuffer = 512;
	static const char *filename_record;

	QMutex mutex;
	QWaitCondition condition;

	bool running, quit;

	// to update vars
	double la_to_update;
	int temperament_to_update;

	public:
	/// constructor
	TunerWorker();
	~TunerWorker();

	public slots:
	void Start();
	void Stop();
	void SetTemperamentIndex(int idx);
	void SetLa(double la);
	void Entry();
	void Quit();

	/// write a file with raw audio
	static void set_record(const char *filename_record);

	signals:
	void resultUpdated(const PitchDetection::PitchResult &result);
	void temperamentListUpdated(const QStringList &list);
};

#endif
