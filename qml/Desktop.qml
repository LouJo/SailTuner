import QtQuick 2.0
import LJTuner 1.0

Item {
	anchors.fill: parent
	Tuner {
		id: tuner
		running: true
	}
	Text {
		text: tuner.freq.toFixed(2) + " Hz"
		font.pixelSize: parent.height / 4
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
	}
}
