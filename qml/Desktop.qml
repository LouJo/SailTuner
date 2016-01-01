import QtQuick 2.0
import LJTuner 1.0

Item {
	width: 600
	height: 500

	Tuner {
		id: tuner
		running: true
	}

	DesktopTheme {
		id: theme
	}

	TunerScreen {
		anchors.fill: parent
		theme: theme
		tuner: tuner
	}
}
