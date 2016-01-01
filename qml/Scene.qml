import QtQuick 2.0

/**
 *  display tuner screen with qmlscene
 *  to work on qml files
 */

Item {
	width: 600
	height: 350

	DesktopTheme {
		id: theme
	}

	Item {
		id: tuner
		property int note: 2
		property int octave: 4
		property double freq: 440
		property double deviation: 0.1
	}

	TunerScreen {
		tuner: tuner
		theme: theme
	}
}
