import QtQuick 2.5
import Skinny 1.0
import LineEdit 1.0

Main
{
    id: main
    inputPanel: embeddedInputPanel

    Window
    {
        id: window

        visible: true
        color: "Gainsboro"

        width: 800
        height: 400

        LinearBox
        {
			orientation: Qt.Vertical
            defaultAlignment: Qt.AlignHCenter | Qt.AlignTop

			margins: 10
			spacing: 10

            LineEdit
            {
                sizePolicy: [ SizePolicy.Maximum ]

                text: "I am a line edit. Edit me."
            }

			InputPanel
			{
				id: embeddedInputPanel
				//visible: Qt.inputMethod.visible
			}
        }
    }
}
