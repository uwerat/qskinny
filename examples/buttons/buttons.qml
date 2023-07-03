import Skinny 1.0 as Qsk
import QtQuick 2.5
import "qrc:/qml"

Qsk.Window
{
    id: window
    visible: true

    width: 600
    height: 600

    Component.onCompleted:
    {
        // very much standard: we should find a better way

        var hint = sizeConstraint();
        setMinimumWidth( hint.width )
        setMinimumHeight( hint.height )
    }

    Qsk.Shortcut
    {
        sequence : "Ctrl+X"
        onActivated: console.log( "Ctrl+X" )
    }

    Qsk.LinearBox
    {
        orientation: Qt.Horizontal
        dimension: 3
        panel : true // to have a themed background

        // padding: 10 // only possible with Qt <= 6.1
        padding { left: 10; top: 10; right: 10; bottom: 10 } 
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
            iconSource: "image://shapes/Ring/Khaki"

            checkable: true
        }

        TestButton
        {
            text: "Check Me"

            section: Qsk.Aspect.Header
            checkable: true
        }

        TestButton
        {
            text: "Push Me"

            section: Qsk.Aspect.Header
        }

        TestButton
        {
            text: "Disabled"

            section: Qsk.Aspect.Header
            enabled: false
        }

        TestButton
        {
            iconSource: "image://shapes/Diamond/SandyBrown"

            iconStrutSize
            {
                // no strutSize, so that the icon is adjustd
                width: -1
                height : -1
            }

            shape
            {
                sizeMode: Qt.RelativeSize
                scalingMode: Qsk.BoxShapeMetrics.SymmetricByMaximum
                radius: 100
            }
        }

        TestButton
        {
            text: "Push Me"

            shape
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
