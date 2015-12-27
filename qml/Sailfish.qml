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
		Page {
			id: page
			allowedOrientations: Orientation.All

			SilicaFlickable {
				anchors.fill: parent

				SimpleDisplay {
					theme: Theme
					tuner: app.tuner
				}

			}

			Tuner {
				id: tunerObject
				running: Qt.application.active && app.userRunning
			}

			Component.onCompleted: {
				app.tuner = tunerObject
				//togglePause.connect(app.togglePause)
			}
		}
	}

	cover: Component {
		CoverBackground {
			SimpleDisplay {
				theme: Theme
				tuner: app.tuner
			}
		}
	}

	function togglePause() {
		app.userRunning = app.userRunning ? false : true
	}
}
