import QtQuick 2.0
import LJTuner 1.0

Item {
	width: 400
	height: 300

	Tuner {
		id: tuner
		running: true
	}

	DesktopTheme {
		id: theme
	}

	SimpleDisplay {
		theme: theme
		tuner: tuner
	}
}
