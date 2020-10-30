import QtQuick 2.5
import Skinny 1.0
import Theme 1.0 // things beyond supersimple are way more convenient with C++

Main
{
    id: main

    property var accentColors: [ "red", "lightgreen", "#66336699" ]

    Theme
    {
        accent: accentColors[ tabBar.currentIndex < 0 ? 0 : tabBar.currentIndex ]
        skin: listBox.entries[ listBox.selectedRow ]
    }

    Window
    {
        id: window

        visible: true
        color: "Gainsboro"

        width: 600
        height: 600

        Component.onCompleted:
        {
            // very much standard: we should find a better way

            var hint = sizeConstraint();
            setMinimumWidth( hint.width )
            setMinimumHeight( hint.height )
        }

        LinearBox
        {
            orientation: Qt.Horizontal

            spacing: 20
            margins: 8 // so that we can see the focus frame

            SimpleListBox
            {
                id: listBox

                preferredWidthFromColumns: true
                sizePolicy.horizontalPolicy: SizePolicy.Fixed

                entries: main.skinList
                selectedRow: 1
            }

            LinearBox
            {
                margins: 10
                orientation: Qt.Vertical
                defaultAlignment: Qt.AlignCenter
                extraSpacingAt: Qt.BottomEdge

                TabBar
                {
                    id: tabBar
                    currentIndex: 1

                    Instantiator
                    {
                        onObjectAdded: {
                            tabBar.insertTab( index, object );
                        }

                        model: accentColors

                        delegate: TabButton
                        {
                            id: tabButton1
                            sizePolicy.horizontalPolicy: SizePolicy.MinimumExpanding
                            text: modelData
                        }
                    }
                }

                LinearBox
                {
                    orientation: Qt.Horizontal
                    dimension: 3

                    spacing: 20
                    margins: 20

                    sizePolicy
                    {
                        horizontalPolicy: SizePolicy.Fixed
                        verticalPolicy: SizePolicy.Fixed
                    }

                    PushButton
                    {
                        text: "normal"
                        focus: true
                    }

                    PushButton
                    {
                        text: checked ? "checked" : "unchecked"
                        checked: true
                        checkable: true
                    }

                    PushButton
                    {
                        text: "disabled"
                        enabled: false
                    }

                    PushButton
                    {
                        text: "flat"
                        flat: true
                    }

                    PushButton
                    {
                        text: ( checked ? "\u2714 " : "" ) + "flat"
                        flat: true
                        checkable: true
                        checked: true
                    }

                    PushButton
                    {
                        text: "flat"
                        flat: true
                        enabled: false
                    }
                }

                LinearBox
                {
                    spacing: 5
                    orientation: Qt.Horizontal

                    sizePolicy
                    {
                        horizontalPolicy: SizePolicy.MinimumExpanding
                        verticalPolicy: SizePolicy.Fixed
                    }

                    Slider
                    {
                        id: slider

                        minimum: 0
                        maximum: 100
                        value: 42

                        snap: true
                        stepSize: 1
                    }

                    TextLabel
                    {
                        id: sliderValue

                        FontMetrics
                        {
                            id: fontMetrics
                        }

                        property rect textRect: fontMetrics.boundingRect("100")
                        preferredSize: Qt.size( textRect.width, textRect.height )
                        sizePolicy: SizePolicy.Fixed

                        text: slider.value
                    }
                }
            }
        }

        FocusIndicator
        {
        }
    }
}
