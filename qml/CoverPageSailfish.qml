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
import Sailfish.Silica 1.0
import "."

/**
 * CoverPage
 *
 * Cover for Sailfish OS
 */

CoverBackground {
	id: cover

	property QtObject tuner
	property bool displayDeviation: true

	Item {
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.topMargin: parent.height / 6

		Label {
			id: coverLabel
			text: "Tuner"
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: parent.top
			anchors.topMargin: Theme.paddingLarge
			anchors.bottomMargin: Theme.paddingLarge
		}

		Text {
			id: coverNote
			text: NoteNames.name(tuner.note) + " " + tuner.octave

			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: coverLabel.bottom
			anchors.topMargin: Theme.paddingLarge

			color: Theme.primaryColor
			font.pixelSize: 35
		}

		Text {
			id: coverDeviation
			text: (tuner.deviation > 0 ? "+" : "") + (tuner.deviation * 101).toFixed(2) + "%"
			visible: tuner.found && cover.displayDeviation

			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: coverNote.bottom
			anchors.topMargin: Theme.paddingLarge

			color: Theme.secondaryColor
			font.pixelSize: 25
		}
	}
}
