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
