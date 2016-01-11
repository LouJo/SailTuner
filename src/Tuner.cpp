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

	qRegisterMetaType<PitchDetection::PitchResult>("PitchDetection::PitchResult");

	worker = new TunerWorker();

	worker->moveToThread(&workerThread);

	connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
	connect(&workerThread, &QThread::started, worker, &TunerWorker::Entry);

	connect(worker, &TunerWorker::resultUpdated, this, &Tuner::ResultUpdated);
	connect(worker, &TunerWorker::temperamentListUpdated, this, &Tuner::TemperamentListUpdated);

	workerThread.start();
}

Tuner::~Tuner()
{
	worker->Quit();
	workerThread.quit();
	workerThread.wait(100);
}

bool Tuner::GetPlaying()
{
	return playing;
}

void Tuner::SetPlaying(bool p)
{
	if (p == playing) return;
	playing = p;
	worker->SetPlaying(p);
	emit playingChanged();
}

bool Tuner::GetRunning()
{
	return running;
}

void Tuner::SetRunning(bool r)
{
	if (running == r) return;

	running = r;
	if (r) worker->Start();
	else worker->Stop();

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

void Tuner::SetNote(int note)
{
	if (result.note == note) return;
	result.note = note;
	worker->SetNote(note);
	emit resultChanged();
}

double Tuner::GetDeviation()
{
	return result.deviation;
}

int Tuner::GetOctave()
{
	return result.octave;
}

void Tuner::SetOctave(int octave)
{
	if (result.octave == octave) return;
	result.octave = octave;
	worker->SetOctave(octave);
	emit resultChanged();
}

bool Tuner::GetFound()
{
	return result.found;
}

void Tuner::SetLa(double la)
{
	this->la = la;
	worker->SetLa(la);
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
	worker->SetTemperamentIndex(idx);
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
