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

#include "TunerWorker.hpp"

using namespace std;

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
	running(false),
	quit(false),
	la_to_update(0),
	temperament_to_update(-1)
{
}

TunerWorker::~TunerWorker()
{
	if (pitchDetection) delete pitchDetection;
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

void TunerWorker::Quit()
{
	mutex.lock();
	running = false;
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

void TunerWorker::Entry()
{
	cerr << __func__ << endl;

	pitchDetection = new PitchDetection();
	emit temperamentListUpdated(pitchDetection->GetTemperamentList());

	while (1) {
		// wait for running
		mutex.lock();
		if (!running) {
			blank_prevent(false);
			while (!running && !quit) condition.wait(&mutex);
			cerr << "wake-up" << endl;
			// reset operations on start
			if (!quit) pitchDetection->Reset();
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
		mutex.unlock();

		std::cout << __func__ << " do job" << std::endl;
	}

	cerr << __func__ << " quit" << endl;
/*
	// prevent screen blanking
	if (nb_sample_running >= nbSamplePreventRunning && running) {
		nb_sample_running = 0;
		blank_prevent(true);
	}*/
}

