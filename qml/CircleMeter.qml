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
	/// theme object
	property QtObject theme

	/// Ellipse
	Canvas {
		id: ellipse
		anchors.fill: parent
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
		}
	}

	Canvas {
		/// level arrow
		id: arrow
		anchors.fill: parent
		property double k: 0.99
		property double offset_base: 0.95
		property double angle: (level - min) / (max - min) * Math.PI - Math.PI / 2

		onPaint: {
			var ctx = getContext('2d');
			ctx.strokeStyle = theme.primaryColor
			ctx.lineWidth = 1
			ctx.beginPath()
			ctx.moveTo(width / 2, (height - 1) * offset_base)
			ctx.lineTo(width * 0.5 + width * 0.5 * k * Math.sin(angle), height * k * (1 - Math.cos(angle) + 1 - k))
			ctx.stroke()

			console.log("angle: " + angle)
		}
	}
}
