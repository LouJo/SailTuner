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
		anchors.horizontalCenter: parent.horizontalCenter

		width: Math.min(parent.width, parent.height * 1.5)
		height: width / 2

		level: tuner.deviation * 100
	}

	ScaleToise {
		id: toise
		theme: parent.theme

		anchors.top: meter.bottom
		anchors.topMargin: h_margin
		anchors.horizontalCenter: parent.horizontalCenter

		width: meter.width
		height: width / 10

		note: tuner.note + toise.nb_notes * tuner.octave
	}
}
