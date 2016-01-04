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
	freq = deviation = 0;
	note = octave = 0;
	found = false;
	la = Scale::defaultLa;

	worker = new TunerWorker();

	temperaments = new Temperaments(":/data");

	if (temperaments->SetTemperament(0)) {
		worker->SetNotesFrequencies(temperaments->NotesFrequencies());
	}

	worker->moveToThread(&workerThread);

	connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
	connect(&workerThread, &QThread::started, worker, &TunerWorker::Entry);
	connect(this, &Tuner::quit, worker, &TunerWorker::Quit, Qt::DirectConnection);

	connect(this, &Tuner::start, worker, &TunerWorker::Start);
	connect(this, &Tuner::stop, worker, &TunerWorker::Stop);
	connect(this, &Tuner::setNotesFrequencies, worker, &TunerWorker::SetNotesFrequencies);
	connect(this, &Tuner::setLa, worker, &TunerWorker::SetLa);

	connect(worker, &TunerWorker::resultFound, this, &Tuner::ResultFound);
	connect(worker, &TunerWorker::resultNotFound, this, &Tuner::ResultNotFound);

	workerThread.start();
}

Tuner::~Tuner()
{
	quit();
//	workerThread.quit();
	workerThread.wait(10);
	delete temperaments;
}

bool Tuner::GetRunning()
{
	return running;
}

void Tuner::SetRunning(bool r)
{
	if (running == r) return;

	running = r;
	if (r) start();
	else stop();

	runningChanged();
}

double Tuner::GetFreq()
{
	return freq;
}

int Tuner::GetNote()
{
	return note;
}

double Tuner::GetDeviation()
{
	return deviation;
}

int Tuner::GetOctave()
{
	return octave;
}

bool Tuner::GetFound()
{
	return found;
}

void Tuner::SetLa(double la)
{
	this->la = la;
	setLa(la);
	laChanged();
}

double Tuner::GetLa()
{
	return la;
}

unsigned int Tuner::GetTemperamentIndex()
{
	return temperaments->GetCurrentIndex();
}

void Tuner::SetTemperamentIndex(unsigned int idx)
{
	if (temperaments->SetTemperament(idx)) {
		setNotesFrequencies(temperaments->NotesFrequencies());
		temperamentChanged();
	}
}

QStringList Tuner::GetTemperamentList() const
{
	return temperaments->GetNames();
}

void Tuner::ResultFound(int note, int octave, double deviation, double freq)
{
	this->note = note;
	this->octave = octave;
	this->deviation = deviation;
	this->freq = freq;
	resultChanged();

	if (!found) {
		foundChanged();
		found = true;
	}
}

void Tuner::ResultNotFound(double freq)
{
	this->freq = freq;
	found = false;
	foundChanged();
}
