import QtQuick 2.0
import Skinny 1.0 as Qsk
import Rects 1.0 

Qsk.Window
{
    visible: true

    color: "Beige"

    width: 800
    height: 300

    Qsk.LinearBox
    {
        dimension: 7

        margins: 10
        spacing: 10

        // without radius
        Rectangle
        {
            color: "DarkOliveGreen"
        }

        Rectangle
        {
            gradient
            {
                orientation: "Vertical"

                stops: [
                    { position: 0.0, color: "MediumAquamarine" },
                    { position: 1.0, color: "DarkSeaGreen" },
                ]
            }
        }

        Rectangle
        {
            border
            {
                color: "DarkSeaGreen"
                width: 20
            }
        }

        Rectangle
        {
            border
            {
                color: "DarkSlateGray"
                width: 20
            }

            color: "PowderBlue"
        }

        Rectangle
        {
            border
            {
                color: "LightGrey"
                width: 20
            }

            gradient
            {
                orientation: "Vertical"

                stops: [
                    { position: 0.0, color: "Peru" },
                    { position: 1.0, color: "Sienna" },
                ]
            }
        }

        Rectangle
        {
            border
            {
                color: "LightGrey"
                width: 20
            }

            gradient
            {
                orientation: "Horizontal"

                stops: [
                    { position: 0.0, color: "DodgerBlue" },
                    { position: 1.0, color: "SteelBlue" },
                ]
            }
        }

        Rectangle
        {
            border
            {
                color: "LightGrey"
                width: 20
            }

            gradient
            {
                orientation: "Diagonal"

                stops: [
                    { position: 0.0, color: "Navy" },
                    { position: 1.0, color: "DodgerBlue" },
                ]
            }
        }

        Rectangle
        {
            corner
            {
                mode: Qt.RelativeSize
                radius: 40
            }

            color: "DarkSlateBlue"
        }

        Rectangle
        {
            corner
            {
                mode: Qt.RelativeSize
                radius: 40
            }

            gradient
            {
                orientation: "Vertical"

                stops: [
                    { position: 0.0, color: "LightSteelBlue" },
                    { position: 1.0, color: "SteelBlue" },
                ]
            }
        }

        Rectangle
        {
            corner
            {
                mode: Qt.AbsoluteSize
                radius: 20
            }

            border
            {
                color: "Crimson"
                width: 20
            }
        }

        Rectangle
        {
            corner
            {
                mode: Qt.RelativeSize
                radius: 40
            }

            border
            {
                color: "DodgerBlue"
                width: 20
            }

            color: "Tan"
        }

        Rectangle
        {
            corner
            {
                mode: Qt.RelativeSize
                radius: 40
            }

            border
            {
                color: "Indigo"
                width: 20
            }

            gradient
            {
                orientation: "Horizontal"

                stops: [
                    { position: 0.0, color: "DeepPink" },
                    { position: 1.0, color: "HotPink" },
                ]
            }
        }

        Rectangle
        {
            corner
            {
                mode: Qt.RelativeSize
                radius: 80
            }

            border
            {
                color: "Indigo"
                width: 20
            }

            gradient
            {
                orientation: "Horizontal"

                stops: [
                    { position: 0.0, color: "DeepPink" },
                    { position: 0.5, color: "DarkOrange" },
                    { position: 1.0, color: "HotPink" },
                ]
            }
        }

        Rectangle
        {
            corner
            {
                mode: Qt.RelativeSize
                radius: 100
            }

            border
            {
                color: "Indigo"
                width: 20
            }

            gradient
            {
                orientation: "Vertical"

                stops: [
                    { position: 0.0, color: "DeepPink" },
                    { position: 0.5, color: "DarkOrange" },
                    { position: 1.0, color: "HotPink" },
                ]
            }
        }
    }
}
