import QtQuick 2.0

/**
 * Main tuner screen
 *
 */

Item {
	id: main
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
		anchors.horizontalCenter: parent.horizontalCenter

		width: Math.min(parent.width, parent.height * 1.5)
		height: width / 2

		level: tuner.found ? tuner.deviation * 100 : -50
	}

	Item {
		anchors.top: meter.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		ScaleToise {
			id: toise
			theme: main.theme

			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter

			width: meter.width
			height: Math.max(width / 10, theme.fontSizeLarge * 1.8)

			note: tuner.note + toise.nb_notes * tuner.octave
		}
	}
}
