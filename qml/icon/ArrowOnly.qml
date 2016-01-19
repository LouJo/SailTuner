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
 * Meter in half circle
 */

Item {
	property double lineWidth: 7
	property double r_arrow_base: 0.08
	property double r_arrow_bottom: 0.070

	/// current level
	property double level: 0.5 
	/// minimum level
	property double min: -50
	/// maximum level
	property double max: 50

	/// theme object
	property QtObject theme

	property double r_circle_min: 0.85
	property double r_circle_max: 1

	property double _height: width / 2

	property double amin: angle(min)
	property double amax: angle(max)

	/// positions helper functions
	function angle(level) {
		return (level - min) / (max - min) * Math.PI - Math.PI / 2
	}
	function getx(angle, k) {
		return width * 0.5 + width * 0.5 * k * Math.sin(angle)
	}
	function gety(angle, k) {
		// k: [0,1]
		return _height - _height * k * Math.cos(angle)
	}
	function dist(x1, y1, x2, y2) {
		return Math.sqrt(Math.pow(x2 - x1, 2) + Math.pow(y2 - y1, 2))
	}

	// return the 2 orthoganals points from b with ref a, with dist d from b
	function orth(ax, ay, bx, by, d) {
		var D = dist(ax, ay, bx, by)
		var k = d / D
		var kx = k * (bx - ax);
		var ky = k * (by - ay);
		return [
			bx + ky, by - kx,
			bx - ky, by + kx
		];
	}

	/// objects draw

	function arc(ctx, k) {
		ctx.beginPath()
		ctx.arc(width/2, height + 1, k * width / 2, 0, Math.PI, 1)
		ctx.stroke()
		return
	}

	function line_mark(ctx, value, r_min, r_max) {
		var a = angle(value)
		ctx.beginPath()
		ctx.moveTo(getx(a, r_min), gety(a, r_min))
		ctx.lineTo(getx(a, r_max), gety(a, r_max))
		ctx.stroke()
	}

	/// Ellipse
	Canvas {
		id: ellipse
		anchors.fill: parent

		property double r_text: 0.92

		property double l_marker: 0.035
		property double h_marker: 6

		property double l_submarker: 0.020
		property double h_submarker: 2

		property int font_size: 20

		onPaint: {
			var ctx = getContext('2d');
			ctx.strokeStyle = theme.primaryColor
			/*
			ctx.lineWidth = 1
			arc(ctx, r_circle_min)
			arc(ctx, r_circle_max)

			ctx.font = font_size + "px sans-serif"
			ctx.textAlign = "center"
			//ctx.fillStyle = theme.secondaryColor

			// first sub marks
			ctx.strokeStyle = theme.primaryColor
			ctx.lineWidth = h_submarker
			for (var j = marks[0] - submarks_int; j > min; j -= submarks_int) {
				line_mark(ctx, j, r_circle_min - l_submarker, r_circle_min + l_submarker)
			}

			ctx.fillStyle = theme.primaryColor
			for (var i = 0; i < marks.length; i++) {
				ctx.strokeStyle = theme.secondaryColor
				ctx.lineWidth = h_marker
				line_mark(ctx, marks[i], r_circle_min - l_marker, r_circle_min + l_marker)
				var a = angle(marks[i])
				ctx.fillText(marks[i], getx(a, r_text), gety(a, r_text) + 4)
				ctx.strokeText()
				// sub marks
				ctx.strokeStyle = theme.primaryColor
				ctx.lineWidth = h_submarker
				var j_max = (i == marks.length - 1 ? max : marks[i+1])
				for (var j = marks[i] + submarks_int; j < j_max; j += submarks_int) {
					line_mark(ctx, j, r_circle_min - l_submarker, r_circle_min + l_submarker)
				}
			}*/
/*
			// "beetween" marks
			ctx.lineWidth = 1
			ctx.strokeStyle = theme.primaryColor
			for (var i = 0; i < marks.length - 1; i++) {
				line_mark(ctx, (marks[i] + marks[i+1])/2, r_circle_min, r_circle_max)
			}*/

			// center: arrow base
			ctx.lineWidth = lineWidth 
			ctx.strokeStyle = theme.secondaryColor
			arc(ctx, r_arrow_base)
/*
			// bottom line
			ctx.lineWidth = 1
			ctx.strokeStyle = theme.primaryColor
			ctx.beginPath()
			ctx.moveTo(0, _height - 1)
			ctx.lineTo(width - 1, _height - 1)
			ctx.stroke()*/
		}
	}

	Canvas {
		/// level arrow
		id: arrow
		anchors.fill: parent
		property double k: 0.82
		property double angle: parent.angle(level)

		property double r_base: parent.r_arrow_bottom
		property double r_circle_base: r_arrow_base * 0.6
		property double k_head: 0.1 // arrow width factor of base
		property double lref: width / 2

		property bool is_line: false // simple line (true) or designed (false)
		property bool has_gradient: false // beautiful gradient

		onPaint: {
			var ctx = getContext('2d');
			ctx.clearRect(0,0,width,height)

			ctx.lineWidth = lineWidth 
			ctx.strokeStyle = theme.primaryColor

			// line only
			if (is_line) {
				var x = getx(angle, r_arrow_base)
				var y = gety(angle, r_arrow_base)
				var hx = getx(angle, k)
				var hy = gety(angle, k)
				ctx.beginPath()
				ctx.moveTo(x,y)
				ctx.lineTo(hx,hy)
				ctx.stroke()
				return
			}

			// arrow base center
			var x = getx(0, r_arrow_base + r_base)
			var y = gety(0, r_arrow_base + r_base)
			// arrow base radius
			var d_base = r_base * width / 2
			// arrow head width
			var d_head = d_base * k_head

			// base internal circle
			ctx.strokeStyle = theme.secondaryColor
			ctx.beginPath()
			ctx.arc(x, y + 0 *(r_base - r_circle_base) * lref, r_circle_base * lref, 0, Math.PI * 2)
			ctx.stroke()

			// base
			ctx.beginPath()
			ctx.strokeStyle = theme.primaryColor

			var x1 = x - d_base * Math.cos(angle)
			var y1 = y - d_base * Math.sin(angle)
			ctx.moveTo(x1, y1)
			ctx.arc(x, y, d_base, Math.PI + angle, angle, 1)
			// head
			var hx = getx(angle, k)
			var hy = gety(angle, k)
			var bout = orth(x, y, hx, hy, d_head)
			ctx.lineTo(bout[2], bout[3])
			ctx.lineTo(bout[0], bout[1])
			ctx.closePath()
			ctx.stroke()

			// gradient
			if (has_gradient) {
				var middle1 = orth(x, y, x + (hx - x) * 0.7, y + (hy - y) * 0.7, d_base + (d_head - d_base) * 0.7)
				var grd = ctx.createLinearGradient(middle1[0], middle1[1], middle1[2], middle1[3])
				grd.addColorStop(1, 'rgba(128,128,128,0.2)')
				grd.addColorStop(0, "transparent")
				ctx.fillStyle = grd
				ctx.fill()
			}
		}
	}

/*
	Behavior on level {
		NumberAnimation {
			duration: 50
			easing.amplitude: max - min
		}
	}
	*/
	onLevelChanged: {
		arrow.requestPaint()
	}

	function redraw() {
		ellipse.requestPaint()
		arrow.requestPaint()
	}
}
