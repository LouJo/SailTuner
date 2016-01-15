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

pragma Singleton

/**
 * NoteNames
 *
 * Singleton to provide facilities to note names
 */

QtObject {
	property int notes_style: Config.notes_style

	property variant notes_fr: [
		"do", "do#", "ré", "mib", "mi", "fa", "fa#", "sol", "sol#", "la", "sib", "si"]
	property variant notes_en: [
		"C", "C#", "D", "Eb", "E", "F", "F#", "G", "G#", "A", "Bb", "B"]

	property variant notesTab: [notes_en, notes_fr]

	property int nb: notes_fr.length
	property variant notes: notesTab[notes_style]

	function name(note) { return notes[note]; }
}
