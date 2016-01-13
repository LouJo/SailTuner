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
	id: toise_flickable
	property bool flik_enable: true

	property int multiple: index % nb_marks

	property double p_min: index2pos(min)
	property double p_max: index2pos(max)

	signal released()
	//animation_enabled: false

	MouseArea {
		property int refX: 0
		property double refPos: parent.position

		enabled: flik_enable

		anchors.fill: parent

		onPressed: {
			animation_enabled = false
			refX = mouseX
			refPos = position
		}
		onMouseXChanged: {
			if (!pressed) return
			var d = (refX - mouseX) / parent.cellWidth
			var p = refPos + d
			p = Math.max(Math.min(p, p_max), p_min)
			var i = Math.round(p + (nb_marks_displayed - 1) / 2)
			index = i
			position = p
			delta = position - Math.floor(position)
			first_mark = Math.floor(position) % nb_marks
		}
		onReleased: {
			updateFlickable()
			animation_enabled = true
			toise_flickable.released()
		}
	}

	function updateFlickable() {
		var p = index2pos(index)
		delta = Qt.binding(function() { return position - Math.floor(position) })
		first_mark = Qt.binding(function() { return Math.floor(position) % nb_marks })
		position = p
	}
}
