import QtQuick 2.0
import QtGraphicalEffects 1.0

/**
 * Led: a circle color
 */

RadialGradient {
	property color led_color: "red"
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
