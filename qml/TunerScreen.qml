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

	CircleMeter {
		theme: parent.theme
		anchors.fill: parent
		/*
		width: Math.min(parent.width, parent.height * 2)
		height: width / 2*/
	}
}
