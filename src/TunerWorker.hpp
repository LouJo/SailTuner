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

#include <fstream>

#include "audio/LinearFilter.hpp"
#include "audio/ZeroCross.hpp"
#include "scale/Scale.hpp"

/**
 * Worker class to work with audio datas
 *
 * Implements the pitch algorithm
 * and note finding
 */
class TunerWorker : public QObject {
	Q_OBJECT

	private:
	static const int rate = 16000;
	static const int defaultNbFrame = 1024;
	static const int defaultFreqMin = 50;
	static const int defaultFreqMax = 2000;
	static const int nbSamplePreventRunning = rate * 40; // 40 seconds
	/// number of analyses to confirm a note
	static const int nbConfirm = 3;
	/// number of analyses to drop a note
	static const int nbDefect = 20;
	/// number of deviation values for average
	static const int nbDeviationValues = 8;

	static const char *filename_record;

	LinearFilter<int16_t> *high_filter;
	ZeroCross<int16_t> *cross;
	Scale *scale;
	std::ofstream file_record;

	QMutex mutex;
	QWaitCondition condition;

	bool running, found, quit;
	int nb_sample_running;
	int note_found, octave_found, count_found, count_not_found;
	int nb_deviation, deviation_start;
	double deviation_sum;
	double deviation_values[nbDeviationValues];

	// to update vars
	double la_to_update;
	const double *freq_to_update;

	inline void ComputeFrame(int16_t v);
	void SetFound(int note, int octave, double deviation);
	void SetNotFound();
	void Reset();
	void ResetDeviation();
	void UpdateDeviation(double d);
	void AudioAnalyse(const int16_t *buffer, int size);

	public:
	/// analyse a file for debug
	static void analyse_file(const char *filename);
	/// write a file with raw audio
	static void set_record(const char *filename_record);

	/// constructor
	TunerWorker();
	~TunerWorker();

	public slots:
	void Start();
	void Stop();
	void SetNotesFrequencies(const double *notes_freq);
	void SetLa(double la);
	void Entry();
	void Quit();

	signals:
	void resultFound(int note, int octave, double deviation, double frequency);
	void resultNotFound(double freq);
};

#endif
