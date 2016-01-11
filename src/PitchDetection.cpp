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

#include <iostream>
#include <fstream>

#include "PitchDetection.hpp"

using namespace std;

// high 10hz / 16k
static double a10[] = { 1        , -2.99214602,  2.98432286, -0.99217678 };
static double b10[] = { 0.99608071, -2.98824212,  2.98824212, -0.99608071 };

/// Pitch detection result methods

PitchDetection::PitchResult::PitchResult() :
	found(false),
	deviation(0),
	frequency(0),
	note(0),
	octave(0)
{
}

void PitchDetection::PitchResult::Set(int n, int o, double d, double f)
{
	note = n;
	octave = o;
	deviation = d;
	frequency = f;
	found = true;
}

/// Pitch detection methods

PitchDetection::PitchDetection()
{
	high_filter = new LinearFilter<int16_t>(3, a10, b10);

	ZeroCross<int16_t>::Config cross_config({rate, defaultNbFrame, defaultFreqMin, defaultFreqMax});
	cross = new ZeroCross<int16_t>(cross_config);

	scale = new Scale();
	temperaments = new Temperaments(":/data");

	/*
	 * done by worker before first frame,
	 * and protected in scale: construct equal if nothing
	 * on first analyse
	 */
	/*
	if (temperaments->SetTemperament(0)) {
		scale->SetNotesFrequencies(temperaments->NotesFrequencies());
	}
	else {
		scale->ConstructEqualTemperament();
	}
*/
	Reset();
}

PitchDetection::~PitchDetection()
{
	delete high_filter;
	delete cross;
	delete scale;
	delete temperaments;
}

/// reset analyse values
void PitchDetection::Reset()
{
	result.found = false;
	updated = false;
	count_found = count_not_found = 0;
	note_found = octave_found = -1;
	ResetDeviation();
	high_filter->Clear();
	cross->Clear();
}

void PitchDetection::ComputeFrame(int16_t v)
{
	v = (*high_filter)(v);
	(*cross)(v);
}

void PitchDetection::ResetDeviation()
{
	// reset deviation values
	nb_deviation = 0;
	deviation_start = 0;
	deviation_sum = 0;
}

void PitchDetection::UpdateDeviation(double d)
{
	if (nb_deviation == nbDeviationValues) {
		deviation_sum -= deviation_values[deviation_start];
		deviation_start = (deviation_start + 1) % nbDeviationValues;
		nb_deviation--;
	}
	deviation_values[(deviation_start + nb_deviation) % nbDeviationValues] = d;
	nb_deviation++;
	deviation_sum += d;
}

void PitchDetection::SetFound(int n, int o, double d)
{
	if (n != note_found || o != octave_found) {
		note_found = n;
		octave_found = o;
		count_found = 0;
		SetNotFound();

		ResetDeviation();
		UpdateDeviation(d);
	}
	else if (count_found++ >= nbConfirm) {
		count_not_found = 0;
		UpdateDeviation(d);

		result.Set(n, o, deviation_sum / nb_deviation, cross->Freq());
		updated = true;
	}
	else {
		UpdateDeviation(d);
	}
}

void PitchDetection::SetNotFound()
{
	if (count_not_found++ >= nbDefect) {
		count_found = 0;
		if (result.found) {
			result.found = false;
			ResetDeviation();
			updated = true;
		}
	}
}

void PitchDetection::SetLa(double la)
{
	scale->SetLa(la);
}

void PitchDetection::SetTemperament(int idx)
{
	if (idx < 0) return;
	if (temperaments->SetTemperament(idx)) {
		scale->SetNotesFrequencies(temperaments->NotesFrequencies());
	}
}

void PitchDetection::AudioAnalyse(const int16_t *ptr, int nb_frame)
{
	// compute every audio frame
	while (nb_frame--) ComputeFrame(*ptr++);

	// find note, octave, deviation
	if (cross->Freq()) {
		int n, o = 0;
		double d = 0;
		n = scale->FindNote(cross->Freq(), o, d);
		SetFound(n, o, d);
	}
	else { // no freq
		SetNotFound();
	}
}

bool PitchDetection::GetResultUpdated(PitchResult &res)
{
	if (!updated) return false;
	res = this->result;
	updated = false;
	return true;
}

QStringList PitchDetection::GetTemperamentList() const
{
	return temperaments->GetNames();
}

double PitchDetection::GetNoteFreq(int note, int octave) const
{
	return scale->GetNoteFreq(note, octave);
}

/// for analyse_file console logs
static void display_results(const PitchDetection::PitchResult &res)
{
	if (res.found)
		cout << res.frequency << " " << Scale::NoteName(res.note) << " " << res.octave << " " << res.deviation << endl;
	else
		cout << res.frequency << endl;
}

/// analyse a file (static function)
void PitchDetection::analyse_file(const char *filename)
{
	cout << "analyse file " << filename << endl;
	ifstream fin;
	fin.open(filename);
	if (!fin.is_open()) {
		cerr << "file not opened" << endl;
		return;
	}

	const int nb_frame = 1024;
	PitchDetection *pitch = new PitchDetection();
	pitch->SetTemperament(0);
	int16_t buffer[nb_frame];
	PitchResult result;

	while (1) {
		fin.read((char*) buffer, sizeof(buffer));
		pitch->AudioAnalyse(buffer, sizeof(buffer) >> 1);

		if (pitch->GetResultUpdated(result)) display_results(result);
		else cout << "." << endl;

		if (fin.eof()) break;
	}
	fin.close();
	delete pitch;
}
