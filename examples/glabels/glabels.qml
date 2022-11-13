import QtQuick 2.0
import Skinny 1.0 as Qsk
import "qrc:/qml"

Qsk.Window
{
    visible: true
    width: 600
    height: 600

    color: "Beige"

    Qsk.LinearBox
    {
        orientation: Qt.Horizontal
        dimension: 3

        //margins: 10
        margins { left: 10; top: 10; right: 10; bottom: 10 }
        spacing: 20

        Repeater
        {
            model: [
                "image://shapes/rectangle/royalblue",
                "image://shapes/triangleright/thistle",
                "image://shapes/ellipse/khaki",
                "image://shapes/ring/sandybrown",
                "image://shapes/star/darkviolet",
                "image://shapes/hexagon/darkslategray"
            ]

            Qsk.GraphicLabel
            {
                source: modelData

                graphicStrutSize.width: 100 // height: according to aspect ratio
                fillMode: Qsk.GraphicLabel.PreserveAspectFit
                alignment: Qt.AlignCenter
                //mirror: true
            }
        }
    }
}
