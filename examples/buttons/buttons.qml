import Skinny 1.0 as Qsk
import QtQuick 2.5
import "qrc:/qml"

Qsk.Window
{
    id: window
    visible: true
    // visibility: QskWindow.Minimized

    width: 600
    height: 600
    color: "Beige"

    Qsk.Shortcut
    {
        sequence : "Ctrl+X"
        onActivated: console.log( "Ctrl+X" )
    }

    Qsk.LinearBox
    {
        orientation: Qt.Horizontal
        dimension: 3

        margins: 10
        spacing: 10

        TestButton
        {
            text: "Push Me"
            focus: true
        }

        TestButton
        {
            text: "Disabled"
            enabled: false
        }

        TestButton
        {
            text: "The quick brown fox jumps over the lazy dog !"
            clip: true

            textOptions
            {
                elideMode: Qt.ElideNone
                wrapMode: Qsk.TextOptions.WordWrap
            }
        }

        TestButton
        {
            text: "The <b><font color='red'>quick</font></b> brown fox jumps over the lazy dog !"
            textOptions
            {
                format: Qsk.TextOptions.AutoText
                elideMode: Qt.ElideRight
            }

            onClicked:
            {
                textOptions.format = textOptions.format === Qsk.TextOptions.AutoText
                    ? Qsk.TextOptions.PlainText : Qsk.TextOptions.AutoText
            }
        }

        TestButton
        {
            text: "Autorepeater"

            autoRepeat: true
            autoRepeatDelay: 500
            autoRepeatInterval: 200
        }

        TestButton
        {
            text: "Check Me"
            graphicSource: "image://shapes/Ring/Khaki"

            checkable: true
        }

        TestButton
        {
            text: "Check Me"

            flat: true

            checkable: true
        }

        TestButton
        {
            text: "Push Me"

            flat: true
        }

        TestButton
        {
            text: "Disabled"

            flat: true
            enabled: false
        }


        TestButton
        {
            graphicSource: "image://shapes/Diamond/SandyBrown"

            corner
            {
                sizeMode: Qt.RelativeSize
                aspectRatioMode: Qt.IgnoreAspectRatio
                radius: 100
            }
        }

        TestButton
        {
            text: "Push Me"

            corner
            {
                sizeMode: Qt.AbsoluteSize
                radius: 0.0
            }
        }
    }

    Qsk.FocusIndicator
    {
    }
}
