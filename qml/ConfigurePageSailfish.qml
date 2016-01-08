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

import ".." // Config singleton

Dialog {
	id: configurePage
	allowedOrientations: Orientation.All

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
					width: pre_la_parent.width / tab_la.length
					border.color: Theme.primaryColor
					border.width: 1

					Text {
						font.pixelSize: Theme.fontSizeSmall
						text: tab_la[parent.index]
						onClick: la.text = text
					}
				}
			}
		}
	}
}
