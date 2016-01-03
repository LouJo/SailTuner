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
import QtGraphicalEffects 1.0

/**
 * Led: a circle color
 */

Rectangle {
	property color led_color: "red"
	property bool on: false

	radius: width / 2
	border.width: 1
	border.color: "#80888888"
	color: "#20888888"

	RadialGradient {
		anchors.fill: parent
		visible: parent.on
		gradient: Gradient {
			GradientStop {
				position: 0.0
				color: led_color
			}
			GradientStop {
				position: 0.5
				color: "transparent"
			}
		}
	}
}
