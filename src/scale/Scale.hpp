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

#ifndef _SCALE_HPP
#define _SCALE_HPP

/**
 * Note recognition within a temperament
 */
class Scale {
	public:
	static const int defaultLa = 440;
	private:
	static const int nbNote = 12;
	static const int cmpOctave = 4;

	static const char *noteNames[nbNote];

	double noteFreq[nbNote], actualNoteFreq[nbNote];
	double actualLa, actualFactor;
	double actualRange[2]; // freq range for default octave
	bool freq_setted;

	/// update scale after temperament or default la change
	void updateScale();

	/**
	 * give the half-tone frequency factor
	 * beetween note and previous or next
	 */
	double halfToneFactor(int note, int sign);

	/// find octave and update freq in range
	int findOctave(double &freq);

	public:
	Scale();
	~Scale() {}

	void ConstructEqualTemperament();

	/// Set notes frequencies from a temperament
	void SetNotesFrequencies(const double freq[nbNote]);

	/// Get ref freq for la4
	double GetLa();

	/// Set ref freq for la4
	void SetLa(double la);

	/// get note frequency
	double GetNoteFreq(int note, int octave);

	/**
	 * Find nearest note, octave, and deviation
	 */
	int FindNote(double freq, int &octave, double &deviation);

	static const char * NoteName(int note);
};

#endif
