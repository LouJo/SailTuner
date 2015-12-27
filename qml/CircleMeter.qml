import QtQuick 2.0

/**
 * Meter in half circle
 */

Item {
	/// current level
	property double level: 0
	/// minimum level
	property double min: -1
	/// maximum level
	property double max: 1
	/// theme object
	property QtObject theme

	Canvas {
		id: canvas
		anchors.fill: parent
		onPaint: {
			var ctx = canvas.getContext('2d');
			ctx.strokeStyle = theme.primaryColor
			ctx.lineWidth = 1
			ctx.beginPath()
			ctx.moveTo(0,height - 1)
			ctx.bezierCurveTo(0, 0, width * 0.5 * (1 - Math.cos(Math.PI/4)), height  * Math.sin(Math.PI/4), width / 2, 0)
			//ctx.closePath()
			ctx.stroke()
			console.log("canvas done")
		}
	}
}
