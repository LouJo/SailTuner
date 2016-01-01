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
	property int h_margin: (height - meter.height - toise.height) / 3

	CircleMeter {
		id: meter
		theme: parent.theme

		anchors.top: parent.top
		anchors.topMargin: h_margin

		width: parent.width
		height: width / 2
	}

	ScaleToise {
		id: toise
		anchors.top: meter.bottom
		anchors.topMargin: h_margin
		anchors.left: parent.left
		width: parent.width
		height: width / 10
	}
}
