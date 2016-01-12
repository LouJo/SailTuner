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

	// min and max values
	property double min: 0
	property double max: 100

	property int h_margin: Math.max(height / 8, main_mark.border_d)

	property int nb_marks: marks.length

	property int nb_marks_displayed: nb_marks
	property bool is_pair: nb_marks_displayed % 2 == 0

	function index2pos(i) { return i - (nb_marks_displayed - 1) / 2 }

	/// current mark is on the middle
	property double position: index2pos(index)

	property int first_mark: Math.floor(position) % nb_marks
	property double delta: position - Math.floor(position)
	property int idx_modulo: index % nb_marks

	property alias cellWidth: toise.cellWidth
	property bool animation_enabled: true

	Behavior on position {
		enabled: animation_enabled
		NumberAnimation {
			duration: 200
			easing.amplitude: nb_marks
		}
	}

	Row {
		id: toise
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.leftMargin: - cellWidth * delta
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
				property int idx_ref: Math.floor(position) + index
				property int idx: (index + first_mark + nb_marks) % nb_marks
				color: mark_color(idx)

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					anchors.verticalCenter: parent.verticalCenter
					text: marks[idx]
					color: idx == idx_modulo ? theme.primaryColor : theme.secondaryColor
					font.pixelSize: parent.height / 2
					opacity: idx_ref >= min && idx_ref <= max ? 1 : 0.2
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
