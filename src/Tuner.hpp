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

#ifndef _TUNER_HPP
#define _TUNER_HPP

#include <QStringList>

#include "PitchDetection.hpp"
#include "TunerWorker.hpp"

class Tuner : public QObject {
	Q_OBJECT
	Q_PROPERTY(bool running READ GetRunning WRITE SetRunning NOTIFY runningChanged)
	Q_PROPERTY(double freq READ GetFreq NOTIFY resultChanged)
	Q_PROPERTY(double deviation READ GetDeviation NOTIFY resultChanged)
	Q_PROPERTY(int note READ GetNote NOTIFY resultChanged)
	Q_PROPERTY(int octave READ GetOctave NOTIFY resultChanged)
	Q_PROPERTY(bool found READ GetFound NOTIFY foundChanged)
	Q_PROPERTY(int temperament_idx READ GetTemperamentIndex WRITE SetTemperamentIndex NOTIFY temperamentChanged)
	Q_PROPERTY(QStringList temperament_list READ GetTemperamentList NOTIFY temperamentListChanged)
	Q_PROPERTY(double la READ GetLa WRITE SetLa NOTIFY laChanged)

	private:
	TunerWorker *worker;
	QThread workerThread;

	PitchDetection::PitchResult result;
	QStringList temperament_list;
	bool running;
	double la;
	int temperament_idx;

	public:
	Tuner();
	~Tuner();

	bool GetRunning();
	void SetRunning(bool r);
	double GetFreq();
	int GetNote();
	int GetOctave();
	double GetDeviation();
	bool GetFound();
	unsigned int GetTemperamentIndex();
	void SetTemperamentIndex(int idx);
	QStringList GetTemperamentList() const;
	double GetLa();
	void SetLa(double la);

	public slots:
	// slots from worker
	void ResultUpdated(const PitchDetection::PitchResult &result);
	void TemperamentListUpdated(const QStringList &list);

	signals:
	// signals to UI
	void runningChanged();
	void foundChanged();
	void laChanged();
	void resultChanged();
	void temperamentChanged();
	void temperamentListChanged();
};

#endif
