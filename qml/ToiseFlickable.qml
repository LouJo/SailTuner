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
 * ToiseFlickable
 *
 * A toise flickable with the mouse
 */

Toise {
	MouseArea {
		property int refX: 0
		property double refPos: parent.position

		anchors.fill: parent

		onPressed: {
			refX = mouseX
			refPos = position
		}
		onMouseXChanged: {
			if (!pressed) return
			var d = (refX - mouseX) / parent.cellWidth
			var p = refPos + d
			var i = Math.round(p + (nb_marks_displayed - 1) / 2)
			index = i % nb_marks
			position = p
			delta = position - Math.floor(position)
			first_mark = Math.floor(position) % nb_marks
		}
		onReleased: {
			var p = 0
			var d = position - Math.floor(position)
			if (d < 0.5) {
				p = Math.floor(position)
			}
			else {
				p = Math.ceil(position)
			}
			var i = Math.floor(p + (nb_marks_displayed - 1) / 2)
			index = i % nb_marks
			first_mark = p % nb_marks
			position = p
			delta = 0
		}
	}
}
