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

	property double r_arrow_base: 0.1

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
		return height - height * k * Math.cos(angle)
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
		}
	}

	Canvas {
		/// level arrow
		id: arrow
		anchors.fill: parent
		property double k: 0.82
		property double k_base: r_arrow_base
		property double angle: parent.angle(level)

		onPaint: {
			var ctx = getContext('2d');
			ctx.clearRect(0,0,width,height)
			ctx.strokeStyle = theme.primaryColor
			ctx.lineWidth = 1
			ctx.beginPath()
			ctx.moveTo(getx(angle, k_base), gety(angle, k_base))
			ctx.lineTo(getx(angle, k), gety(angle, k))
			ctx.stroke()
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
			ctx.clearRect(0,0,width,height)

			var a0 = angle(marks[reg[0]]) // angle of mark
			var a1 = angle(reg[1]) // min angle of region
			var a2 = angle(reg[2]) // max angle of region

			// gradient
			var x_center = getx(a0, r_avg);
			var y_center = gety(a0, r_avg);
			var grd = ctx.createRadialGradient(x_center, y_center, 2, x_center, y_center,
				(reg[2] - reg[1]) * (width + height) / 2 / (max - min))
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
