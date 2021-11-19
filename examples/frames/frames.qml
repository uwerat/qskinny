import QtQuick 2.0
import Skinny 1.0 as Qsk
import Frames 1.0

Qsk.Window
{
    visible: true
    color: "Gainsboro"

    width: 800
    height: 600

    Qsk.LinearBox
    {
        orientation: Qt.Horizontal
        dimension: 6

        //margins: 10 // only possible with Qt <= 6.1
        margins { left: 10; top: 10; right: 10; bottom: 10 }
        spacing: 10

        // panel

        Repeater
        {
            model: [0, 1, 2, 3, 4, 10]

            Frame
            {
                style: Frame.Plain
                color: "Silver"
                frameWidth: modelData
            }
        }

        Repeater
        {
            model: [0, 1, 2, 3, 4, 10]

            Frame
            {
                style: Frame.Raised
                color: "Silver"
                frameWidth: modelData
            }
        }

        Repeater
        {
            model: [0, 1, 2, 3, 4, 10]

            Frame
            {
                style: Frame.Sunken
                color: "Silver"
                frameWidth: modelData
            }
        }

        Repeater
        {
            model: [0, 1, 2, 3, 4, 10]

            Frame
            {
                style: Frame.Plain
                color: "Silver"
                frameWidth: modelData
                radius: 50
            }
        }

        Repeater
        {
            model: [0, 1, 2, 3, 4, 10]

            Frame
            {
                style: Frame.Raised
                color: "Silver"
                frameWidth: modelData
                radius: 50
            }
        }

        Repeater
        {
            model: [0, 1, 2, 3, 4, 10]

            Frame
            {
                style: Frame.Sunken
                color: "Silver"
                frameWidth: modelData
                radius: 50
            }
        }
    }
}
