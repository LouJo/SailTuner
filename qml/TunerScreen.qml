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
 * Main tuner screen
 *
 */

Item {
	id: main
	/// tuner object
	property QtObject tuner
	/// theme corresponding to Silica Theme object
	property QtObject theme

	anchors.fill: parent
	property int h_margin: (height - meter.height - toise.height) / 3

	// landscape / portrait
	property bool is_portrait: height > width

	// maximum absolute deviation to display green led
	property double deviation_ok: 0.05
	property color led_green: "green"
	property color led_red: "red"

	opacity: tuner.running ? 1 : 0.8

	property bool dev_is_ok: Math.abs(tuner.deviation) <= deviation_ok

	Led {
		anchors.top: meter.top
		anchors.left: meter.left
		anchors.topMargin: meter.width / 24 
		anchors.leftMargin: anchors.topMargin
		width: meter.width / 12
		height: width
		led_color: dev_is_ok ? led_green : led_red
		on: tuner.found && (dev_is_ok || tuner.deviation < 0)
	}

	Led {
		anchors.top: meter.top
		anchors.right: meter.right
		anchors.topMargin: meter.width / 24 
		anchors.rightMargin: anchors.topMargin
		width: meter.width / 12
		height: width
		led_color: dev_is_ok ? led_green : led_red
		on: tuner.found && (dev_is_ok || tuner.deviation > 0)
	}

	CircleMeter {
		id: meter
		theme: parent.theme

		anchors.top: parent.top
		anchors.topMargin: h_margin
		anchors.horizontalCenter: parent.horizontalCenter

		width: Math.min(parent.width, parent.height * 1.5)
		height: width / 2

		level: tuner.found ? tuner.deviation * 100 : -50
	}

	Item {
		id: note_info

		anchors.top: meter.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		property int toise_h: Math.max(width / 10, theme.fontSizeLarge * 1.8)
		property double margin_h: parent.is_portrait ? (height - toise_h * 2) / 7 : (height - toise_h) / 2

		Text {
			id: title_note
			visible: main.is_portrait
			text: qsTr("Note")

			font.pixelSize: theme.fontSizeMedium
			color: theme.highlightColor

			anchors.top: parent.top
			anchors.topMargin: parent.margin_h
			anchors.horizontalCenter: parent.horizontalCenter
		}

		ScaleToise {
			id: toise
			theme: main.theme

			anchors.top: main.is_portrait ? title_note.bottom : parent.top
			anchors.topMargin: parent.margin_h
			anchors.horizontalCenter: parent.horizontalCenter

			width: meter.width
			height: parent.toise_h

			note: tuner.note
			octave: tuner.octave
		}

		Text {
			id: title_octave
			visible: main.is_portrait
			text: qsTr("Octave")

			font.pixelSize: theme.fontSizeMedium
			color: theme.highlightColor

			anchors.top: toise.bottom
			anchors.topMargin: parent.margin_h
			anchors.horizontalCenter: parent.horizontalCenter
		}

		// octave toise
		Toise {
			anchors.top: main.is_portrait ? title_octave.bottom : toise.bottom
			anchors.topMargin: parent.margin_h
			anchors.horizontalCenter: parent.horizontalCenter

			visible: main.is_portrait

			theme: main.theme

			marks: [1, 2, 3, 4, 5, 6, 7, 8, 9]
			nb_marks_displayed: 4

			width: meter.width / 2
			height: parent.toise_h

			index: tuner.octave
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {
			tuner.running = tuner.running ^ true
		}
	}
}
