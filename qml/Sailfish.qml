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
import harbour.sailtuner.objectsaver 1.0
import "."

/**
 * Sailfish main page
 *
 */

ApplicationWindow {
	id: app
	allowedOrientations: Orientation.All

	property bool userRunning: true
	property bool userPlaying: false

	property int dbFontSize: 100
	property QtObject tuner

	ObjectSaver {
		id: saver
		filename: "config.dat"
		object: Config
	}

	Tuner {
		id: tunerItem
		temperament_idx: Config.temperament_idx
		la: Config.la

		running: Qt.application.active && app.userRunning
		playing: Qt.application.active && app.userPlaying

		Component.onCompleted: app.tuner = tunerItem
	}

	initialPage: Component {
		Page {
			id: page
			allowedOrientations: Orientation.All
			property bool running: true
			property bool tunerRunning: running && status == PageStatus.Active

			SilicaFlickable {
				anchors.fill: parent

				PullDownMenu {
					MenuItem {
						text: qsTr("Configuration")
						onClicked: {
							var confpage = pageStack.push(Qt.resolvedUrl("ConfigurePageSailfish.qml"), { tuner: app.tuner })
							confpage.configChanged.connect(saver.save)
						}
					}
				}

				TunerScreen {
					id: screen
					anchors.fill: parent
					theme: Theme
					tuner: app.tuner

					onToggleRun: page.running ^= true
				}
			}
			onTunerRunningChanged: app.userRunning = tunerRunning

		}
	}

	Component {
		id: player

		Page {
			id: page
			allowedOrientations: Orientation.All
			property bool playing: false
			property bool tunerPlaying: playing && status == PageStatus.Active

			SilicaFlickable {
				anchors.fill: parent

				PullDownMenu {
					MenuItem {
						text: qsTr("Configuration")
						onClicked: {
							var confpage = pageStack.push(Qt.resolvedUrl("ConfigurePageSailfish.qml"), { tuner: app.tuner })
							confpage.configChanged.connect(saver.save)
						}
					}
				}

				PlayerScreen {
					id: screen
					anchors.fill: parent
					theme: Theme
					tuner: app.tuner

					onTogglePlay: page.playing ^= true
				}
			}
			onTunerPlayingChanged: app.userPlaying = tunerPlaying
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

	Component.onCompleted: pageStack.pushAttached(player)
}
