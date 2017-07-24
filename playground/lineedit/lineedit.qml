import QtQuick 2.5
import Skinny 1.0
import LineEdit 1.0

Main
{
	// inputPanel: embeddedInputPanel

    Window
    {
        id: window

        visible: true

        width: 600
        height: 600

        LinearBox
        {
            defaultAlignment: Qt.AlignCenter

            LineEdit
            {
                sizePolicy: [ SizePolicy.Fixed, SizePolicy.Fixed ]

                text: "I am a line edit. Edit me."
            }
        }
    }
}
