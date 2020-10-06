/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.3 as Controls
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.12 as Kirigami

/**
 * PassiveNotification is a type for small, passive and inline
notifications in the app.
 * used to show messages of limited importance that make sense only when
 * the user is using the application and wouldn't be suited as a global 
 * system-wide notification.
 * This is not a full-fledged notification system. the applciation should 
 * use this with care and only one notification should be visible at once per app.
*/
Controls.Popup {
    id: root

    x: Math.round(parent.width/2 - width/2)
    y: parent.height - height - Kirigami.Units.smallSpacing
    implicitWidth: Math.max(background ? background.implicitWidth : 0,
                            contentWidth + leftPadding + rightPadding) + leftInset + rightInset
    implicitHeight: Math.max(background ? background.implicitHeight : 0 ,
                             contentHeight + topPadding + bottomPadding)+ topInset + bottomInset
    height: implicitHeight
    width: implicitWidth

    topPadding: Kirigami.Units.smallSpacing
    leftPadding: Kirigami.Units.smallSpacing
    rightPadding: Kirigami.Units.smallSpacing
    bottomPadding: Kirigami.Units.smallSpacing

    modal: false
    closePolicy: Controls.Popup.NoAutoClose
    focus: false
    clip: false

    function showNotification(message, timeout, actionText, callBack) {
        if (!message) {
            return;
        }

        let interval = 7000;

        if (timeout == "short") {
            interval = 4000;
        } else if (timeout == "long") {
            interval = 12000;
        } else if (timeout > 0) {
            interval = timeout;
        }

        open();

        for (let i = 0; i < outerLayout.children.length - 3; ++i) {
            outerLayout.children[i].close();
        }

        let delegate = delegateComponent.createObject(outerLayout, {
            "text": message,
            "actionText": actionText || "",
            "callBack": callBack || (function(){}),
            "interval": interval
        });

        // Reorder items to have the last on top
        let children = outerLayout.children;
        for (let i in children) {
            children[i].Layout.row = children.length-1-i;
        }
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
    
    background: Item {}

    contentItem: GridLayout {
        id: outerLayout
        columns: 1
    }

    Component {
        id: delegateComponent
        Controls.Control {
            id: delegate
            property alias text: label.text
            property alias actionText: actionButton.text
            property alias interval: timer.interval
            property var callBack
            Layout.alignment: Qt.AlignCenter
            Layout.bottomMargin: -delegate.height
            opacity: 0
            function close() {
                closeAnim.running = true;
            }

            leftPadding: Kirigami.Units.largeSpacing
            rightPadding: Kirigami.Units.largeSpacing
            topPadding: Kirigami.Units.largeSpacing
            bottomPadding: Kirigami.Units.largeSpacing

            Component.onCompleted: openAnim.restart()
            ParallelAnimation {
                id: openAnim
                OpacityAnimator {
                    target: delegate
                    from: 0
                    to: 1
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.InOutQuad
                }
                NumberAnimation {
                    target: delegate
                    property: "Layout.bottomMargin"
                    from: -delegate.height
                    to: 0
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.InOutQuad
                }
            }

            SequentialAnimation {
                id: closeAnim
                ParallelAnimation {
                    OpacityAnimator {
                        target: delegate
                        from: 1
                        to: 0
                        duration: Kirigami.Units.longDuration
                        easing.type: Easing.InOutQuad
                    }
                    NumberAnimation {
                        target: delegate
                        property: "Layout.bottomMargin"
                        to: -delegate.height
                        duration: Kirigami.Units.longDuration
                        easing.type: Easing.InOutQuad
                    }
                }
                ScriptAction {
                    script: delegate.destroy();
                }
            }

            contentItem: RowLayout {
                id: mainLayout

                Kirigami.Theme.colorSet: root.Kirigami.Theme.colorSet
                width: mainLayout.width
                //FIXME: why this is not automatic?
                implicitHeight: Math.max(label.implicitHeight, actionButton.implicitHeight)
                HoverHandler {
                    id: hover
                }
                TapHandler {
                    acceptedButtons: Qt.LeftButton
                    onTapped: delegate.close();
                }
                Timer {
                    id: timer
                    running: root.visible && !hover.hovered
                    onTriggered: delegate.close();
                }

                Controls.Label {
                    id: label
                    Layout.maximumWidth: Math.min(root.parent.width - Kirigami.Units.largeSpacing * 4, implicitWidth)
                    elide: Text.ElideRight
                    wrapMode: Text.WordWrap
                    maximumLineCount: 4
                }

                Controls.Button {
                    id: actionButton
                    visible: text.length > 0
                    onClicked: {
                        delegate.close();;
                        if (delegate.callBack && (typeof delegate.callBack === "function")) {
                            delegate.callBack();
                        }
                    }
                }
            }
            background: Kirigami.ShadowedRectangle {
                Kirigami.Theme.colorSet: root.Kirigami.Theme.colorSet
                shadow {
                    size: Kirigami.Units.gridUnit/2
                    color: Qt.rgba(0, 0, 0, 0.4) 
                    yOffset: 2
                }
                radius: Kirigami.Units.smallSpacing * 2
                color: Kirigami.Theme.backgroundColor
                opacity: 0.6
            }
        }
    }

    Controls.Overlay.modal: Rectangle {
        color: Qt.rgba(0, 0, 0, 0.4)
    }

    Controls.Overlay.modeless: Item {}
}

