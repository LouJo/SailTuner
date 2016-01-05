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
 * Toise
 *
 * Display marks with current centered
 */

Item {
	id: scale

	property QtObject theme
	
	// mark index
	property int index: 1
	// array of marks
	property variant marks: []
	// function to get mark color
	property var mark_color: function (id) { return "transparent" }

	property int h_margin: Math.max(height / 8, main_mark.border_d)

	property int nb_marks: marks.length

	property int nb_marks_displayed: nb_marks
	property bool is_pair: nb_marks_displayed % 2 == 0

	/// current mark is on the middle
	property double position: index - (nb_marks_displayed - 1) / 2

	property int first_mark: Math.floor(position) % nb_marks
	property double delta: position - Math.floor(position)

	Behavior on position {
		NumberAnimation {
			duration: 200
			easing.amplitude: nb_marks
		}
	}

	Row {
		id: toise
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.leftMargin: is_pair ? - cellWidth * delta : 0
		anchors.bottom: parent.bottom
		anchors.right: parent.right
		anchors.topMargin: h_margin
		anchors.bottomMargin: h_margin

		property double cellWidth: parent.width / nb_marks_displayed

		Repeater {
			model: is_pair ? nb_marks_displayed + 1 : nb_marks_displayed
			Rectangle {
				width: toise.cellWidth
				height: toise.height
				border.width: 1
				property int idx: (index + nb_marks + first_mark) % nb_marks
				color: mark_color(idx)

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					anchors.verticalCenter: parent.verticalCenter
					text: marks[idx]
					color: theme.primaryColor
					font.pixelSize: parent.height / 2
				}
			}
		}
	}

	// Screen for actual mark
	Rectangle {
		id: main_mark
		y: 0
		x: toise.cellWidth * (parent.nb_marks_displayed - 1) / 2  - border_d
		width: toise.cellWidth + border_d * 2
		height: parent.height

		property int border_d: 10

		color: "transparent"
		border.width: border_d
		border.color: "#a0777777"
	}
}
