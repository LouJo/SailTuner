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

#include <iostream>
#include <stdint.h>

#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QUrl>

#include <iostream>

#include "Tuner.hpp"

using namespace std;

Tuner::Tuner()
{
	running = false;
	temperament_idx = 0;
	la = Scale::defaultLa;

	worker = new TunerWorker();

	worker->moveToThread(&workerThread);

	connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
	connect(&workerThread, &QThread::started, worker, &TunerWorker::Entry);

	connect(this, &Tuner::quit, worker, &TunerWorker::Quit, Qt::DirectConnection);
	connect(this, &Tuner::start, worker, &TunerWorker::Start);
	connect(this, &Tuner::stop, worker, &TunerWorker::Stop);
	connect(this, &Tuner::setTemperamentIndex, worker, &TunerWorker::SetTemperament);
	connect(this, &Tuner::setLa, worker, &TunerWorker::SetLa);

	connect(worker, &TunerWorker::resultUpdated, this, &Tuner::ResultUpdated);
	connect(worker, &TunerWorker::temperamentListUpdated, this, &Tuner::TemperamentListUpdated);

	workerThread.start();
}

Tuner::~Tuner()
{
	quit();
//	workerThread.quit();
	workerThread.wait(10);
}

bool Tuner::GetRunning()
{
	return running;
}

void Tuner::SetRunning(bool r)
{
	if (running == r) return;

	running = r;
	if (r) emit start();
	else emit stop();

	emit runningChanged();
}

double Tuner::GetFreq()
{
	return result.frequency;
}

int Tuner::GetNote()
{
	return result.note;
}

double Tuner::GetDeviation()
{
	return result.deviation;
}

int Tuner::GetOctave()
{
	return result.octave;
}

bool Tuner::GetFound()
{
	return result.found;
}

void Tuner::SetLa(double la)
{
	this->la = la;
	emit setLa(la);
	emit laChanged();
}

double Tuner::GetLa()
{
	return la;
}

unsigned int Tuner::GetTemperamentIndex()
{
	return temperament_idx;
}

void Tuner::SetTemperamentIndex(int idx)
{
	temperament_idx = idx;
	emit setTemperamentIndex(idx);
	emit temperamentChanged();
}

QStringList Tuner::GetTemperamentList() const
{
	return temperament_list;
}

void Tuner::ResultUpdated(const PitchDetection::PitchResult &result)
{
	const bool changed = (this->result.found ^ result.found);

	this->result = result;

	if (result.found) emit resultChanged();
	if (changed) emit foundChanged();
}

void Tuner::TemperamentListUpdated(const QStringList &list)
{
	temperament_list = list;
	emit temperamentListChanged();
}
