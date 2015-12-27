import QtQuick 2.0

/**
 * Simple display for tuner test without gui
 */

Column {
	property QtObject tuner
	property QtObject theme

	anchors.horizontalCenter: parent.horizontalCenter
	anchors.verticalCenter: parent.verticalCenter

	Component {
		id: simpleText
		Text {
			font.pixelSize: theme.fontSizeExtraLarge
			color: theme.primaryColor
			anchors.horizontalCenter: parent.horizontalCenter
		}
	}

	Component.onCompleted: {
		var freq = simpleText.createObject(this)
		freq.text = Qt.binding(function () { return tuner ? tuner.freq.toFixed(2) + " Hz" : "NaN" })

		var note = simpleText.createObject(this)
		note.text = Qt.binding(function() { return tuner.noteName })

		var octave = simpleText.createObject(this)
		octave.text = Qt.binding(function() { return tuner.octave })

		var deviation = simpleText.createObject(this)
		deviation.text = Qt.binding(function() { return tuner.deviation.toFixed(3) })
	}
}
