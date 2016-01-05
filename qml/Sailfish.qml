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
import harbour.sailtuner.tuner 1.0

/**
 * Sailfish main page
 *
 */

ApplicationWindow {
	id: app
	allowedOrientations: Orientation.All

	property bool userRunning: true
	property int dbFontSize: 100
	property QtObject tuner

	initialPage: Component {
		Page {
			id: page
			allowedOrientations: Orientation.All
			signal togglePause()

			SilicaFlickable {
				anchors.fill: parent

				TunerScreen {
					anchors.fill: parent
					theme: Theme
					tuner: app.tuner
				}

			}

			Tuner {
				id: tunerObject
				running: Qt.application.active && app.userRunning
			}

			MouseArea {
				anchors.fill: parent
				onClicked: togglePause()
			}

			Component.onCompleted: {
				app.tuner = tunerObject
				togglePause.connect(app.togglePause)
			}
		}
	}

	cover: Component {
		CoverBackground {
			SimpleDisplay {
				theme: Theme
				tuner: app.tuner
			}
		}
	}

	function togglePause() {
		app.userRunning = app.userRunning ? false : true
	}
}
