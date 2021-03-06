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
	enum WAVEFORM { W_SINUS, W_TRIANGLE, W_HARMONIC };

	private:
	/// frequencies for min and max volumes
	static constexpr double freq_volume_min = 1200;
	static constexpr double freq_volume_max = 120;

	/// current frequency
	double freq = 440;
	/// current volume (linear)
	double volume = 0.5;
	/// volume min and max
	double volume_min = 0.5;
	double volume_max = 1;
	/// if volume is adaptative to freq (lower -> volume higher)
	bool volume_adaptative = true;
	/// audio rate
	int rate;
	/// current frame nb
	int n_frame;
	/// wave form
	WAVEFORM waveform = W_HARMONIC;
	/// pre computed factor
	double k, k_update;
	// last frame written
	sample_t last_frame;
	/// number of hamonics for waveforms (W_HARMONIC only)
	int nb_harmonics = 5;
	/// max harmonic to play given current frequency
	double max_harmonic_freq;

	/// return k computed
	double K() const;
	/// return the max sample_t
	sample_t sample_min();
	/// return the max sample_t
	sample_t sample_max();
	/// return the current radius
	double radius();
	/// return volume if adaptative
	void freq2volume();

	public:
	FreqPlayer(int rate);
	~FreqPlayer() {}

	/// reset current sound
	void Reset();
	/// get next audio frame
	inline sample_t AudioFrame();
	/// write audio buffer
	void WriteAudio(sample_t *out, int nb_frame, bool stop = false);
	/// set current frequency
	void SetFreq(double freq);
	/// set current volume
	void SetVolume(double volume);
	/// set waveform
	void SetWaveform(WAVEFORM form, int nb_harmonics);
};

#endif
