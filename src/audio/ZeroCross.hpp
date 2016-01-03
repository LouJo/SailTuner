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

#ifndef ZEROCROSS_H
#define ZEROCROSS_H

#include <stdint.h>
#include <vector>

namespace patterns {

	struct Pattern {
		double time, energy;
	};
	struct PatternMatch {
		double time, time_deviation, energy_deviation;
		PatternMatch() : time(0), time_deviation(0), energy_deviation(0) {}
	};

};

template<typename sample_t> class ZeroCross {
	public:
	struct Config {
		uint16_t rate;
		int nb_frame;
		int tonie_min, tonie_max;
	};

	private:
	int rate, nb_frame_to_analyse, nb_frame_analysed, diff_x;
	patterns::Pattern pattern_current;
	int16_t last_sample;
	double freq, sample_min, sample_max;
	bool last_zero;
	std::vector<patterns::Pattern> pattern;

	inline void ComputeFrame(sample_t x);

	public:
	ZeroCross(const Config &config);
	double operator() (sample_t *ptr, int nbFrame);
	void operator() (sample_t v);

	void Clear();
	double Freq();
};

#endif
