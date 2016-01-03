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

/**
 * Simple display for tuner test without gui
 */

Column {
	property QtObject tuner
	property QtObject theme

	anchors.horizontalCenter: parent.horizontalCenter
	anchors.verticalCenter: parent.verticalCenter

	Component {
		id: simpleText
		Text {
			font.pixelSize: theme.fontSizeExtraLarge
			color: theme.primaryColor
			anchors.horizontalCenter: parent.horizontalCenter
		}
	}

	Component.onCompleted: {
		var freq = simpleText.createObject(this)
		freq.text = Qt.binding(function () { return tuner ? tuner.freq.toFixed(2) + " Hz" : "NaN" })

		var note = simpleText.createObject(this)
		note.text = Qt.binding(function() { return tuner.noteName })

		var octave = simpleText.createObject(this)
		octave.text = Qt.binding(function() { return tuner.octave })

		var deviation = simpleText.createObject(this)
		deviation.text = Qt.binding(function() { return tuner.deviation.toFixed(3) })
	}
}
