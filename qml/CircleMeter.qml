import QtQuick 2.0

/**
 * Meter in half circle
 */

Item {
	/// current level
	property double level: 0.5 
	/// minimum level
	property double min: -1
	/// maximum level
	property double max: 1
	/// numbers to write on the scale
	property variant marks: [-1, -0.5, 0, 0.5, 1, -0.2, 0.2, -0.8, 0.8]
	/// theme object
	property QtObject theme

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

	/// Ellipse
	Canvas {
		id: ellipse
		anchors.fill: parent

		property double mark_k_min: 0.90
		property double mark_k_max: 0.95
		property double mark_k_text: 0.85
		property int font_size: 16

		onPaint: {
			var ctx = getContext('2d');
			ctx.strokeStyle = theme.primaryColor
			ctx.lineWidth = 1
			ctx.beginPath()
			ctx.moveTo(0,height - 1)
			ctx.bezierCurveTo(0, -height * 0.33, width - 1, -height * 0.33, width - 1, height - 1)
			//ctx.closePath()
			ctx.stroke()
			console.log("ellipse done")
			ctx.font = font_size + "px sans-serif"
			ctx.textAlign = "center"

			for (var i = 0; i < marks.length; i++) {
				var a = angle(marks[i])
				ctx.beginPath()
				ctx.moveTo(getx(a, mark_k_min), gety(a, mark_k_min))
				ctx.lineTo(getx(a, mark_k_max), gety(a, mark_k_max))
				ctx.stroke()
				ctx.fillText(marks[i], getx(a, mark_k_text), gety(a, mark_k_text))
				ctx.strokeText()
			}
		}
	}

	Canvas {
		/// level arrow
		id: arrow
		anchors.fill: parent
		property double k: 0.99
		property double k_base: 0.1
		property double angle: parent.angle(level)

		onPaint: {
			var ctx = getContext('2d');
			ctx.strokeStyle = theme.primaryColor
			ctx.lineWidth = 1
			ctx.beginPath()
			ctx.moveTo(getx(angle, k_base), gety(angle, k_base))
			ctx.lineTo(getx(angle, k), gety(angle, k))
			ctx.stroke()

			console.log("angle: " + angle)
		}
	}
}
