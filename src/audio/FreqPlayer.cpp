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

#include <math.h>
#include <stdint.h>
#include <iostream>

#include "FreqPlayer.hpp"

#ifndef min
#define min(a,b) (a<b?a:b)
#endif

#ifndef max
#define max(a,b) (a>b?a:b)
#endif

template<typename sample_t> FreqPlayer<sample_t>::FreqPlayer(int _rate):
	rate(_rate),
	n_frame(0)
{
	k = K();
	k_update = -1; // invalid: don't update
	max_harmonic_freq = rate / 2;
	freq2volume();
}

template<typename sample_t> void FreqPlayer<sample_t>::Reset()
{
	n_frame = 0;
	last_frame = 0;
	if (k_update != -1) {
		k = k_update;
		k_update = -1;
		freq2volume();
	}
}

template<typename sample_t> double FreqPlayer<sample_t>::K() const
{
	return freq / rate * M_PI * 2;
}

template<typename sample_t> void FreqPlayer<sample_t>::SetFreq(double freq)
{
	this->freq = freq;
	k_update = K();
}

template<typename sample_t> void FreqPlayer<sample_t>::SetVolume(double volume)
{
	this->volume = volume;
}

template<typename sample_t> void FreqPlayer<sample_t>::SetWaveform(WAVEFORM form, int nb_harmonics)
{
	if (form == W_HARMONIC) this->nb_harmonics = nb_harmonics;
	waveform = form;
}

template<> int16_t FreqPlayer<int16_t>::sample_min() { return INT16_MIN; }
template<> double FreqPlayer<double>::sample_min() { return -1; }

template<> int16_t FreqPlayer<int16_t>::sample_max() { return INT16_MAX; }
template<> double FreqPlayer<double>::sample_max() { return 1; }

template<typename sample_t> double FreqPlayer<sample_t>::radius()
{
	return (double) (n_frame++) * k;
}

template<typename sample_t> sample_t FreqPlayer<sample_t>::AudioFrame()
{
	double r = radius();
	double v;
	int i;

	switch (waveform) {
	case W_SINUS:
		v = sin(r);
		break;

	case W_TRIANGLE:
		r = fmod(r, M_PI * 2);

		if (r <= M_PI / 2) v = r;
		else if (r <= M_PI * 1.5) v = M_PI - r;
		else v = r - M_PI * 2;

		v /= (M_PI / 2);
		break;

	case W_HARMONIC:
		v = 0;
		for (i = 1; i <= nb_harmonics; i++) if (freq * i < max_harmonic_freq)
			v += sin(r * i) / i;
		break;

	default:
		v = 0;
		break;
	}

	return max(min(v * sample_max() * volume, sample_max()), sample_min());
}

template<typename sample_t> void FreqPlayer<sample_t>::freq2volume()
{
	if (volume_adaptative) volume = min(volume_max, max(volume_min, (freq_volume_min - freq) / (freq_volume_min - freq_volume_max) * (volume_max - volume_min) + volume_min));
	std::cerr << "volume " << volume << std::endl;
}

template<typename sample_t> void FreqPlayer<sample_t>::WriteAudio(sample_t *out, int nb_frame, bool stop)
{
	sample_t v;
	const bool to_update = (k_update != -1) || stop;

	while (nb_frame--) {
		v = AudioFrame();
		if (to_update && (v >= 0 && last_frame < 0)) {
			// zero cross. update now.
			if (k_update != -1) {
				// update frequency
				n_frame = 0;
				k = k_update;
				k_update = -1;
				freq2volume();
				v = AudioFrame();
			}
			else if (stop) {
				std::cerr << "stop audio" << std::endl;
				// finish with 0s
				*out++ = 0;
				while (nb_frame--) *out++ = 0;
				last_frame = 0;
				return;
			}
		}
		*out++ = v;
		last_frame = v;
	}
}

// instanciation for int16
template class FreqPlayer<int16_t>;
