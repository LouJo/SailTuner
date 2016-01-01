import QtQuick 2.0

/**
 * Main tuner screen
 *
 */

Item {
	/// tuner object
	property QtObject tuner
	/// theme corresponding to Silica Theme object
	property QtObject theme

	anchors.fill: parent
	property int h_free: parent.height - parent.width / 2

	CircleMeter {
		id: meter
		theme: parent.theme
		y: (h_free - toise.height) / 3
		width: Math.min(parent.width, parent.height * 2)
		height: width / 2
	}

	ScaleToise {
		id: toise
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		width: parent.width
		height: Math.min(h_free, width / 12)
		anchors.bottomMargin: (h_free - height) / 3
	}
}
