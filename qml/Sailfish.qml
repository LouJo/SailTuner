import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.sailtuner.tuner 1.0

/**
 * Sailfish main page
 *
 */

ApplicationWindow {
	id: app
	allowedOrientations: Orientation.All

	property bool userRunning: true
	property int dbFontSize: 100
	property QtObject tuner

	initialPage: Component {
		ApplicationWindow {
			Tuner {
				id: tunerObject
				running: Qt.application.active && app.userRunning
			}

			property QtObject tuner: tunerObject

			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
				text: tuner.freq.toFixed(2) + " Hz"
				font.pixelSize: 90
				color: Theme.primaryColor
			}

			Component.onCompleted: {
				app.tuner = tunerObject
				//togglePause.connect(app.togglePause)
			}
		}
	}

	cover: Component {
		CoverBackground {
			Text {
				color: Theme.primaryColor
				font.pixelSize: 40
				text: app.tuner.freq.toFixed(2) + " Hz"
			}
		}
	}

	function togglePause() {
		app.userRunning = app.userRunning ? false : true
	}
}
