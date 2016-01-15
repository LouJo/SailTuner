import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
	allowedOrientations: Orientation.All

	SilicaFlickable {
		anchors.fill: parent
		contentHeight: column.height

		Column {
			id: column
			width: parent.width
			spacing: 10

			PageHeader {
				title: qsTr("About")
			}

			Image {
				anchors.horizontalCenter: parent.horizontalCenter
				source: "file:///usr/share/icons/hicolor/128x128/apps/harbour-sailtuner.png" 
			}

			SectionHeader {
				anchors.horizontalCenter: parent.horizontalCenter
				text: "SailTuner"
				horizontalAlignment: Text.AlignHCenter
			}

			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: Theme.fontSizeSmall
				width: parent.width
				wrapMode: Text.WordWrap
				horizontalAlignment: Text.AlignHCenter
				color: Theme.primaryColor
				text: qsTr("Chromatic multi-temperaments tuner and notes player")
			}

			Label {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: Theme.fontSizeSmall
				text: "version 0.1.1"
			}

			SectionHeader {
				anchors.horizontalCenter: parent.horizontalCenter
				text: qsTr("Copyright")
				horizontalAlignment: Text.AlignHCenter
			}

			Label {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: Theme.fontSizeSmall
				text: "Louis-Joseph Fournier"
			}
			Label {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: Theme.fontSizeSmall
				text: "01-2016"
			}
			Label {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: Theme.fontSizeSmall
				text: qsTr("licence") + ": GPLv3"
			}

			SectionHeader {
				anchors.horizontalCenter: parent.horizontalCenter
				text: qsTr("Links")
				horizontalAlignment: Text.AlignHCenter
			}

			Label {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: Theme.fontSizeSmall
				text: qsTr("Source code and algo description")
			}

			Label {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: Theme.fontSizeSmall
				property string urlstring: "https://github.com/LouJo/SailTuner"
				text: "<a href=\"" + urlstring + "\">" +  urlstring + "<\a>"
				onLinkActivated: Qt.openUrlExternally(link)
			}

			Label {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: Theme.fontSizeSmall
				text: qsTr("Personnal blog")
			}

			Label {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: Theme.fontSizeSmall
				property string urlstring: "http://legolas.vefblog.net"
				text: "<a href=\"" + urlstring + "\">" +  urlstring + "<\a>"
				onLinkActivated: Qt.openUrlExternally(link)
			}
		}
	}
}
