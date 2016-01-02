import QtQuick 2.0

/**
 * Toise
 *
 * Display marks with current centered
 */

Item {
	id: scale

	property QtObject theme
	
	// mark index
	property int index: 1
	// array of marks
	property variant marks: []
	// function to get mark color
	property var mark_color: function (id) { return "transparent" }

	property int h_margin: Math.max(height / 8, main_mark.border_d)

	property int nb_marks: marks.length

	property int nb_marks_displayed: nb_marks

	/// current mark is on the middle
	property double position: index - (nb_marks_displayed - 1) / 2

	property int first_mark: Math.floor(position) % nb_marks
	property double delta: position - Math.floor(position)

	Behavior on position {
		NumberAnimation {
			duration: 200
			easing.amplitude: nb_marks
		}
	}

	Row {
		id: toise
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.leftMargin: - cellWidth * delta
		anchors.bottom: parent.bottom
		anchors.right: parent.right
		anchors.topMargin: h_margin
		anchors.bottomMargin: h_margin

		property double cellWidth: parent.width / nb_marks_displayed

		Repeater {
			model: nb_marks_displayed + 1
			Rectangle {
				width: toise.cellWidth
				height: toise.height
				border.width: 1
				property int idx: (index + nb_marks + first_mark) % nb_marks
				color: mark_color(idx)

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					anchors.verticalCenter: parent.verticalCenter
					text: marks[idx]
					color: theme.primaryColor
					font.pixelSize: parent.height / 2
				}
			}
		}
	}

	// Screen for actual mark
	Rectangle {
		id: main_mark
		y: 0
		x: toise.cellWidth * (parent.nb_marks_displayed - 1) / 2  - border_d
		width: toise.cellWidth + border_d * 2
		height: parent.height

		property int border_d: 10

		color: "transparent"
		border.width: border_d
		border.color: "#a0777777"
	}
}
