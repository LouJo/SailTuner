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

#ifndef _PITCH_DETECTION_HPP
#define _PITCH_DETECTION_HPP

#include "audio/LinearFilter.hpp"
#include "audio/ZeroCross.hpp"
#include "scale/Scale.hpp"
#include "scale/Temperaments.hpp"

/**
 * Pitch detection algorithm
 *
 * Implements the pitch algorithm
 * and note finding, using audio and scale objects
 */
class PitchDetection {
	public:
	/**
	 * Structure for pitch detection result
	 */
	struct PitchResult {
		bool found;
		double deviation, frequency;
		int note, octave;

		PitchResult();
		void Set(int n, int o, double d, double f);
	};

	static const int rate = 16000;

	private:
	static const int defaultNbFrame = 1024;
	static const int defaultFreqMin = 50;
	static const int defaultFreqMax = 2000;
	/// number of analyses to confirm a note
	static const int nbConfirm = 3;
	/// number of analyses to drop a note
	static const int nbDefect = 20;
	/// number of deviation values for average
	static const int nbDeviationValues = 8;

	LinearFilter<int16_t> *high_filter;
	ZeroCross<int16_t> *cross;
	Scale *scale;
	Temperaments *temperaments;

	PitchResult result;
	bool updated;
	int note_found, octave_found, count_found, count_not_found;
	int nb_deviation, deviation_start;
	double deviation_sum;
	double deviation_values[nbDeviationValues];

	void SetFound(int note, int octave, double deviation);
	void SetNotFound();
	void ResetDeviation();
	void UpdateDeviation(double d);

	/// Compute an audio sample
	inline void ComputeFrame(int16_t v);

	public:
	PitchDetection();
	~PitchDetection();

	/// compute an audio buffer
	void AudioAnalyse(const int16_t *buffer, int size);
	/// Get results if updated
	bool GetResultUpdated(PitchResult &result);
	/// reset computing
	void Reset();
	/// Set la4 freq reference
	void SetLa(double freq);
	/// Set temperament
	void SetTemperament(int idx);
	/// Get temperament list
	QStringList GetTemperamentList() const;
	/// Get the note frequency from Scale
	double GetNoteFreq(int note, int octave) const;

	/// analyse a file for debug
	static void analyse_file(const char *filename);
};

#endif
