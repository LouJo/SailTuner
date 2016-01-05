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

#include "ZeroCross.hpp"

#include <iostream>
#include <stdlib.h>
#include <math.h>

using namespace std;
using namespace patterns;

/// Maximum rest of division to be a multiple of
#define EPS_DIVISOR 0.1
/// Consider it is a correct pattern to avoid multiples of
#define CORRECT_TIME_DEVIATION 1
/// max factor of deviation to consider multiple to be dropped
#define MAX_TIME_DEVIATION_FACTOR_MULTIPLE 2
/// Correct pattern, good enough to drop every lower
#define CORRECT_TIME_DEVIATION_NO_LOWER 0.001

//#define CORRECT_ENERGY_DEVIATION 1

// local functions

/**
 * Return average and standart deviation of pattern approx. interval in values
 */
PatternMatch IsPattern(const vector<Pattern> &values, double interval)
{
	double current = 0;
	int nb = 0;
	double diff = interval;
	double sum_energy = 0, first_energy = -1;
	PatternMatch match;

	for (const Pattern &v : values) {
		if (fabs(diff - v.time) < fabs(diff)) {
			current += v.time;
			sum_energy += v.energy;
			diff = interval - current;
		}
		else {
			if (first_energy == -1) first_energy = sum_energy;

			match.time += current;
			match.time_deviation += diff * diff;
			match.energy_deviation += pow(first_energy - sum_energy, 2);
			current = v.time;
			sum_energy = v.energy;
			diff = interval - current;
			nb++;
		}
	}
	if (nb < 2) return PatternMatch(); // empty pattern
	match.time /= nb; // time is average
	match.time_deviation = sqrt(match.time_deviation) / nb;
	match.energy_deviation = sqrt(match.energy_deviation) / match.time;
	return match;
}

/**
 * Find best matching pattern beetween min and max
 * from an integer list
 *
 * @return best interval pattern, standart deviation for best pattern
 */
PatternMatch FindPattern(const vector<Pattern> &values, double pattern_min, double pattern_max)
{
	PatternMatch res, best;
	double interval = 0;

	for (const Pattern &v : values) {
		interval += v.time;
		if (interval < pattern_min) continue;
		if (interval > pattern_max) break;

		res = IsPattern(values, interval);
		//cout << "  " << res.time << " " << 16000/res.time << " " << res.time_deviation << " " << res.energy_deviation << endl;
		if (res.time && ((res.time_deviation < best.time_deviation && best.time_deviation > CORRECT_TIME_DEVIATION_NO_LOWER) || best.time == 0)) {
			if (best.time && best.time_deviation < CORRECT_TIME_DEVIATION  && best.time_deviation / res.time_deviation < MAX_TIME_DEVIATION_FACTOR_MULTIPLE) {
				double div = res.time / best.time;
				// it is a multiple of previous
				if (fabs(div - round(div)) < EPS_DIVISOR) continue;
				//cout << "... " << div - floor(div) << endl;
			}
			best = res;
		}
	}
	//cout << " -> " << best.time << " " << best.time_deviation << endl;
	if (best.time_deviation > CORRECT_TIME_DEVIATION) return PatternMatch(); // empty pattern
	return best;
}

/// Methods implementation

template<typename A> ZeroCross<A>::ZeroCross(const Config &config)
{
	rate = config.rate;
	nb_frame_to_analyse = config.nb_frame;
	freq = 0;
	sample_max = (double) rate / config.tonie_min;
	sample_min = (double) rate / config.tonie_max;
	Clear();
}

template<typename sample_t> void ZeroCross<sample_t>::Clear()
{
	last_sample = 0;
	pattern_current.time = -1;
	nb_frame_analysed = 0;
	freq = 0;
	last_zero = false;
	pattern.clear();
}

template<typename sample_t> void ZeroCross<sample_t>::ComputeFrame(sample_t x)
{
	double delta;

	// count frame only if ever crossed
	if (pattern_current.time != -1) {
		pattern_current.time++;
		pattern_current.energy += x * x;
	}
	// get only from negative to positive
	if (x > 0 && last_sample < 0) {
		if (last_zero) delta = 1;
		else delta = (double) x / (x - last_sample);

		if (pattern_current.time > 0) {
			pattern_current.time -= delta;
			pattern.push_back(pattern_current);
			//cout << nb_frame_current << " ";
		}
		pattern_current.time = delta;
		pattern_current.energy = x * x;
	}
	// drop 0 values to know sign change
	if (x) last_sample = x, last_zero = false;
	else last_zero = true;
	// compute if window suffisent
	if (nb_frame_analysed++ == nb_frame_to_analyse) {
		auto res = FindPattern(pattern, sample_min, sample_max);
		if (res.time) freq = rate / res.time;
		else freq = 0;
		nb_frame_analysed = 0;
		pattern.clear();
	}
}

template<typename sample_t> double ZeroCross<sample_t>::operator() (sample_t *ptr, int nbFrame)
{
	while(nbFrame--) ComputeFrame(*ptr++);
	return freq;
}

template<typename sample_t> void ZeroCross<sample_t>::operator() (sample_t v)
{
	ComputeFrame(v);
}

template<typename sample_t> double ZeroCross<sample_t>::Freq()
{
	return freq;
}

// instanciations
template class ZeroCross<int16_t>;
