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

#include <assert.h>
#include <math.h>
#include <iostream>
#include <string.h>

#include "Scale.hpp"

const char * Scale::noteNames[] = {
	"do", "do#", "ré", "mib", "mi", "fa", "fa#", "sol",
	"sol#", "la", "sib", "si"
};

const char * Scale::NoteName(int note)
{
	if (note < 0 || note >= nbNote) return "";
	return noteNames[note];
}

Scale::Scale()
{
	actualLa = defaultLa;
	freq_setted = false;
}

void Scale::updateScale()
{
	actualFactor = actualLa / defaultLa;
	for (int i = 0; i < nbNote; i++) {
		actualNoteFreq[i] = actualFactor * noteFreq[i];
//		std::cerr << " " << noteFreq[i] << " " << actualNoteFreq[i] << std::endl;
	}
	actualRange[0] = actualNoteFreq[0] * pow(halfToneFactor(0, -1),0.5);
	actualRange[1] = actualNoteFreq[nbNote - 1] * pow(halfToneFactor(nbNote - 1, 1), 0.5);

	//std::cerr << actualRange[0] << " " << actualRange[1] << std::endl;
}

double Scale::halfToneFactor(int note, int sign)
{
	double a, b;
	a = noteFreq[note];

	if (sign < 0) {
		if (note == 0) b = noteFreq[nbNote - 1] / 2;
		else b = noteFreq[note - 1];
	}
	else {
		if (note == nbNote - 1) b = noteFreq[0] * 2;
		else b = noteFreq[note + 1];
	}
	return b / a;
}

int Scale::findOctave(double &freq)
{
	int octave = cmpOctave;

	while (freq > actualRange[1]) freq /= 2, octave++;
	while (freq < actualRange[0]) freq *= 2, octave--;

	return octave;
}

int Scale::FindNote(double freq, int &octave, double &deviation)
{
	assert (freq > 0);
	if (!freq_setted) {
		std::cerr << "Scale " << __func__ << ": notes not setted" << std::endl;
		ConstructEqualTemperament();
	}

	int note = 0;
	octave = findOctave(freq);

	//std::cerr << octave << "  " << freq << std::endl;
	assert(freq >= actualRange[0] && freq <= actualRange[1]);

	while (actualNoteFreq[note] < freq && note < nbNote - 1) note++;

	if (note > 0) {
		if (freq / actualNoteFreq[note - 1] < actualNoteFreq[note] / freq) note--;
	}

	int sign = freq > actualNoteFreq[note] ? 1 : -1;

	if (sign == 1) deviation = log(freq / actualNoteFreq[note]) / log(halfToneFactor(note, sign));
	else deviation = log(actualNoteFreq[note] / freq) / log(halfToneFactor(note, sign));

	return note;
}

void Scale::ConstructEqualTemperament()
{
	const int la = 9;
	int i;
	noteFreq[la] = defaultLa;

	for (i = 0; i < nbNote; i++) if (i != la) {
		noteFreq[i] = defaultLa * pow(2, (double) (i - la) / 12);
		//std::cerr << noteFreq[i] << std::endl;
	}
	freq_setted = true;
	updateScale();
}

double Scale::GetLa()
{
	return actualLa;
}

void Scale::SetNotesFrequencies(const double freq[nbNote])
{
	std::cout << __func__ << std::endl;
	freq_setted = true;
	memcpy(noteFreq, freq, sizeof(double) * nbNote);
	updateScale();
}

void Scale::SetLa(double la)
{
	assert(la > 0);
	actualLa = la;
	if (freq_setted) updateScale();
}

double Scale::GetNoteFreq(int note, int octave)
{
	assert(note >= 0 && note < nbNote);
	double f = actualNoteFreq[note];
	octave -= 4;
	if (octave < 0) f /= 1 << octave;
	else if (octave > 0) f *= 1 << octave;

	return f;
}
