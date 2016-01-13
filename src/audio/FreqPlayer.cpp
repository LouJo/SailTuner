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

template<typename sample_t> FreqPlayer<sample_t>::FreqPlayer(int _rate):
	freq(440),
	volume(0.5),
	rate(_rate),
	n_frame(0),
	waveform(W_SINUS)
{
	k = K();
	k_update = -1; // invalid: don't update
}

template<typename sample_t> void FreqPlayer<sample_t>::Reset()
{
	n_frame = 0;
	if (k_update != -1) {
		k = k_update;
		k_update = -1;
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

template<> int16_t FreqPlayer<int16_t>::max() { return INT16_MAX; }
template<> double FreqPlayer<double>::max() { return 1; }

template<typename sample_t> double FreqPlayer<sample_t>::radius()
{
	double ret = (double) (n_frame++) * k;

	/* to update frequency factor, wait current radius to go to beginning
	 * in interval [0, 2PI]
	 */
	if (k_update != -1) {
		double a = fmod(ret, M_PI * 1);
		double b = fmod(n_frame * k, M_PI * 2);
		if (b < a) { // next frame go back to beginning of circle
			n_frame = 0;
			k = k_update;
			k_update = -1;
		}
	}
	return ret;
}

template<typename sample_t> sample_t FreqPlayer<sample_t>::AudioFrame()
{
	switch (waveform) {
	case W_SINUS:
	return (double) sin(radius()) * max() * volume;

	default:
	return 0;
	}
}

template<typename sample_t> void FreqPlayer<sample_t>::WriteAudio(sample_t *out, int nb_frame)
{
	while (nb_frame--) *out++ = AudioFrame();
}

// instanciation for int16
template class FreqPlayer<int16_t>;
