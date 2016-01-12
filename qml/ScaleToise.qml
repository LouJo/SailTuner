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

import QtQuick 2.0
import "."

/**
 * ScaleToise
 *
 * Display notes from scale, with current note centered.
 */

ToiseFlickable {
	id: scale

	// note
	property int note: 1
	// octave
	property int octave: 4
	// en or fr
	property int notes_style: Config.notes_style

	property variant notes_fr: [
		"do", "do#", "ré", "mib", "mi", "fa", "fa#", "sol", "sol#", "la", "sib", "si"]
	property variant notes_en: [
		"C", "C#", "D", "Eb", "E", "F", "F#", "G", "G#", "A", "Bb", "B"]
	property variant notes: [notes_en, notes_fr]

	// case colors
	property color colorAltered: "#40888888"
	property color colorNatural: "transparent"

	// Toise parameters
	index: note + notes_en.length * octave
	marks: notes[notes_style]
	nb_marks_displayed: width > 100 ? Math.min(nb_marks, width / theme.fontSizeLarge * 0.8) : 1

	mark_color: function(note) {
		if (isAltered(note)) return colorAltered;
		else return colorNatural;
	}

	function isAltered(i) {
		return (i < 4 && (i & 1)) || (i > 5 && !(i & 1))
	}

	// ToiseFlikcable parameters
	min: notes_en.length * 1 // ut 1
	max: notes_en.length * 9 - 1 // si 8
}
