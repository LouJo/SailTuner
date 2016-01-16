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
import ".."

/**
 *  display tuner screen with qmlscene
 *  to work on qml files
 */

Rectangle {
	width: 800
	height: 400

	color: "black"

	DesktopTheme {
		id: theme
	}
	ArrowOnly {
		level: 10
		lineWidth: 10
		anchors.fill: parent
		theme: theme
	}

}
