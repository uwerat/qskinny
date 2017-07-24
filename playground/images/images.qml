import QtQuick 2.0
import Skinny 1.0 as Qsk
import Images 1.0
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

        margins: 10
        spacing: 10

        Repeater
        {
            model: [
                "image://shapes/rectangle/royalblue",
                "image://shapes/triangleup/thistle",
                "image://shapes/ellipse/khaki",
                "image://shapes/ring/sandybrown",
                "image://shapes/star/darkviolet",
                "image://shapes/triangleright/darkslategray"
            ]

            // We could also use Qt.Image with binding sourceWidth/sourceHeight
            // to width/height, but Qsk.Image avoids several pointless
            // image updates for this use case

            Image
            {
                source: modelData

                cache: false // does not make much sense for textures

                // vector graphics are ususally scalable and requesting
                // the image in a different size from the image provider
                // makes a lot more sense, than scaling the texture/pixmap.
                // Changing the sourceSize does this and sourceSizeAdjustment
                // automatically modifies the sourceSize according to
                // the size of the item

                sourceSizeAdjustment: true
            }
        }
    }
}
