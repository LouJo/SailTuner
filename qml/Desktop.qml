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
import LJTuner 1.0
import LJUtils 1.0

import "."

Item {
	width: 600
	height: 500

	Tuner {
		id: tuner
		running: false
		temperament_idx: Config.temperament_idx
		la: Config.la
	}

	ObjectSaver {
		id: saver
		filename: "config.dat"
		object: Config
	}

	DesktopTheme {
		id: theme
	}

	Image {
		anchors.fill: parent
		source: "../images/bg_portrait.png"

		TunerScreen {
			anchors.fill: parent
			theme: theme
			tuner: tuner
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked: tuner.running = (tuner.running ^ true)
	}
}
