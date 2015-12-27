import QtQuick 2.0

/**
 *  display tuner screen with qmlscene
 *  to work on qml files
 */

Item {
	width: 600
	height: 400

	Item {
		id: theme
		property color primaryColor: "#000000"
		property color secondaryColor: "#444444"
		property int paddingSmall: 4
		property int paddingLarge: 20
		property int fontSizeSmall: 10
		property int fontSizeMedium: 16
		property int fontSizeLarge: 25
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
