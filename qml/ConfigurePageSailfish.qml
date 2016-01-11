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

	property int buttonHeight: Theme.fontSizeSmall * 1.2

	Column {
		width: parent.width
		spacing: Theme.paddingLarge

		DialogHeader {
			id: header
		}

		PageHeader {
			title: qsTr("Configuration")
		}

		Item {
			width: parent.width
			height: Theme.fontSizeLarge * 3
			property int margin_x: buttonHeight / 2

			TextField {
				id: la
				anchors.left: parent.left
				anchors.leftMargin: parent.margin_x
				text: Config.la
				label: qsTr("La")
				width: Theme.fontSizeMedium * 5
			}

			Button {
				id: minus
				width: buttonHeight * 2
				height: buttonHeight
				anchors.left: la.right
				anchors.leftMargin: parent.margin_x
				text: "-"
				preferredWidth: Theme.buttonWidthSmall
				onClicked: la.text = parseInt(la.text) - 1
			}

			Button {
				width: buttonHeight * 2
				height: buttonHeight
				anchors.left: minus.right
				anchors.leftMargin: parent.margin_x
				text: "+"
				preferredWidth: Theme.buttonWidthSmall
				onClicked: la.text = parseInt(la.text) + 1
			}
		}

		/// predefined la(s)
		Item {
			id: pre_la_parent
			width: parent.width
			height: Theme.fontSizeSmall * 3;
			property double case_l: width / la_tab.length
			property double case_margin: case_l / 8;
			property double case_width: case_l - case_margin * 2

			Repeater {
				model: la_tab.length

				Button {
					text: configurePage.la_tab[index]
					width: pre_la_parent.case_width
					height: buttonHeight
					preferredWidth: Theme.buttonWidthSmall
					x: index * pre_la_parent.case_l + pre_la_parent.case_margin
					y: 0
					down: la.text == configurePage.la_tab[index]
					onClicked: la.text = text
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
			checked: Config.notes_style == 1
		}
	}
	onAccepted: {
		Config.la = la.text
		Config.temperament_idx = combo.currentIndex
		Config.notes_style = notes_style.checked ? 1 : 0
		configChanged()
	}
}
