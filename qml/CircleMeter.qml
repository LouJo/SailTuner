import QtQuick 2.0

/**
 * Meter in half circle
 */

Item {
	/// current level
	property double level: 0.5 
	/// minimum level
	property double min: -50
	/// maximum level
	property double max: 50
	/// numbers to write on the scale
	property variant marks: [-40, -20, 0, 20, 40]
	/// interval of little marks
	property double submarks_int: 5
	/// marks regions colors
	property variant region_color: ["#F88E48", "#F8DE48", "#99E882", "#F8DE48", "#F88E48"]
	/// theme object
	property QtObject theme

	property double r_circle_min: 0.85
	property double r_circle_max: 1

	property double r_arrow_base: 0.05
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
		ctx.moveTo(getx(amin, k), gety(amin, k))
		for (var i = amin + 0.01; i <= amax; i+=0.01) {
			ctx.lineTo(getx(i,k), gety(i,k))
		}
		ctx.stroke()
	}

	function arc_part(ctx, k, a1, a2) {
		ctx.lineTo(getx(a1,k), gety(a1,k))
		var eps = 0.01
		if (a2 > a1) {
			for (var i = a1 + eps; i < a2; i+=eps) {
				ctx.lineTo(getx(i,k), gety(i,k))
			}
		}
		else {
			for (var i = a1 - eps; i > a2; i-=eps) {
				ctx.lineTo(getx(i,k), gety(i,k))
			}
		}
		ctx.lineTo(getx(a2,k), gety(a2,k))
	}

	function line_mark(ctx, value, r_min, r_max) {
		var a = angle(value)
		ctx.beginPath()
		ctx.moveTo(getx(a, r_min), gety(a, r_min))
		ctx.lineTo(getx(a, r_max), gety(a, r_max))
		ctx.stroke()
	}

	function find_region() {
		var l1 = min
		var l2
		for (var i = 0; i < marks.length; i++) {
			if (i == marks.length - 1) l2 = max
			else l2 = (marks[i] + marks[i+1]) / 2

			if (level <= l2) return [i, l1, l2]
			l1 = l2
		}
		return [0,0,0]
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
			}

			// "beetween" marks
			ctx.lineWidth = 1
			ctx.strokeStyle = theme.primaryColor
			for (var i = 0; i < marks.length - 1; i++) {
				line_mark(ctx, (marks[i] + marks[i+1])/2, r_circle_min, r_circle_max)
			}

			// center: arrow base
			ctx.lineWidth = 1
			ctx.strokeStyle = theme.secondaryColor
			arc(ctx, r_arrow_base)

			// bottom line
			ctx.lineWidth = 1
			ctx.strokeStyle = theme.primaryColor
			ctx.beginPath()
			ctx.moveTo(0, _height - 1)
			ctx.lineTo(width - 1, _height - 1)
			ctx.stroke()
		}
	}

	Canvas {
		/// level arrow
		id: arrow
		anchors.fill: parent
		property double k: 0.82
		property double angle: parent.angle(level)

		property double r_base: 0.055
		property double r_circle_base: r_arrow_base * 0.6
		property double k_head: 0.1 // arrow width factor of base
		property double lref: width / 2

		onPaint: {
			var ctx = getContext('2d');
			ctx.clearRect(0,0,width,height)

			ctx.lineWidth = 1

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
			var middle1 = orth(x, y, x + (hx - x) * 0.7, y + (hy - y) * 0.7, d_base + (d_head - d_base) * 0.7)
			var grd = ctx.createLinearGradient(middle1[0], middle1[1], middle1[2], middle1[3])
			grd.addColorStop(1, 'rgba(128,128,128,0.2)')
			grd.addColorStop(0, "transparent")
			ctx.fillStyle = grd
			ctx.fill()
		}
	}

	Canvas {
		/// region colors
		id: regions
		property variant reg_drawed
		anchors.fill: parent
		property variant reg: find_region()
		property double r_avg: (r_circle_min + r_circle_max) / 2
		z: -4
		onPaint: {
			var ctx = getContext('2d');
			ctx.clearRect(0,0,width,_height)

			var a0 = angle(marks[reg[0]]) // angle of mark
			var a1 = angle(reg[1]) // min angle of region
			var a2 = angle(reg[2]) // max angle of region

			// gradient
			var x_center = getx(a0, r_avg);
			var y_center = gety(a0, r_avg);
			var grd = ctx.createRadialGradient(x_center, y_center, 2, x_center, y_center,
				(reg[2] - reg[1]) * (width + _height) / 2 / (max - min))
			grd.addColorStop(0, region_color[reg[0]])
			grd.addColorStop(1, "transparent")

			ctx.fillStyle = grd
			ctx.beginPath()
			ctx.moveTo(getx(a1, r_circle_min), gety(a1, r_circle_min))
			arc_part(ctx, r_circle_min, a1, a2)
			arc_part(ctx, r_circle_max, a2, a1)
			ctx.lineTo(getx(a1, r_circle_min), gety(a1, r_circle_min))
			ctx.fill()
			reg_drawed = reg
		}

		function update_level() {
			if (!reg_drawed || level > reg_drawed[2] || level < reg_drawed[1]) {
				reg = find_region()
				requestPaint()
			}
		}
	}

	Behavior on level {
		NumberAnimation {
			duration: 50
			easing.amplitude: max - min
		}
	}

	onLevelChanged: {
		arrow.requestPaint()
		regions.update_level()
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {
			level = Math.random() * (max - min) + min
		}
	}
}
