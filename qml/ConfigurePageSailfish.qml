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

import "." // Config singleton

Dialog {
	id: configurePage
	allowedOrientations: Orientation.All

	signal configChanged()

	property QtObject tuner;
	property variant la_tab: [392, 400, 415, 430, 440, 442]

	Column {
		width: parent.width

		DialogHeader {
			id: header
		}

		PageHeader {
			title: qsTr("Configuration")
		}

		TextField {
			id: la
			text: Config.la
			label: qsTr("La")
		}

		/// predefined la(s)
		Row {
			id: pre_la_parent
			width: parent.width

			Repeater {
				model: la_tab.length

				Rectangle {
					width: pre_la_parent.width / la_tab.length
					border.color: Theme.primaryColor
					border.width: 1
					color: la.text == la_tab[parent.index] ? Theme.highlightColor : "transparent"

					Text {
						id: la_freq
						font.pixelSize: Theme.fontSizeSmall
						text: tab_la[parent.index]
					}
					MouseArea {
						anchors.fill: parent
						onClicked: la.text = la_freq.text
					}
				}
			}
		}

		/// temperaments
		ComboBox {
			id: combo
			width: parent.width
			label: qsTr("Temperament")
			currentIndex: Config.temperament_idx
			property variant temp_list: tuner.temperament_list

			menu: ContextMenu {
				Repeater {
					model: combo.temp_list.length

					MenuItem {
						text: combo.temp_list[index]
					}
				}
			}
		}

		TextSwitch {
			id: notes_style
			text: qsTr("French notes")
			down: Config.note_style == 1
		}
	}
	onAccepted: {
		Config.la = la.text
		Config.temperament_idx = combo.currentIndex
		Config.notes_style = notes_style.checked ? 1 : 0
		configChanged()
	}
}
