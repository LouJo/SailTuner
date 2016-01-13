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

#ifndef _FREQ_PLAYER_HPP
#define _FREQ_PLAYER_HPP

/**
 * FreqPlayer
 *
 * Play a note at given frequency
 * Waveform style can be: sinus
 */
template<typename sample_t> class FreqPlayer {
	public:
	/// waveform style
	enum WAVEFORM { W_SINUS };

	private:
	/// current frequency
	double freq;
	/// current volume (linear)
	double volume;
	/// audio rate
	int rate;
	/// current frame nb
	int n_frame;
	/// wave form
	WAVEFORM waveform;
	/// pre computed factor
	double k, k_update;

	/// return k computed
	double K() const;
	/// return the max sample_t
	sample_t max();
	/// return the current radius
	double radius();

	public:
	FreqPlayer(int rate);
	~FreqPlayer() {}

	/// reset current sound
	void Reset();
	/// get next audio frame
	inline sample_t AudioFrame();
	/// write audio buffer
	void WriteAudio(sample_t *out, int nb_frame);
	/// set current frequency
	void SetFreq(double freq);
	/// set current volume
	void SetVolume(double volume);
};

#endif
