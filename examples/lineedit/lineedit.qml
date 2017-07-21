import Skinny 1.0
import QtQuick 2.5

Main
{
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
