/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
import QtQuick 2.7
import QtQuick.Controls 2.0
import org.kde.kirigami 2.9 as Kirigami

MouseArea {
    id: root
    default property Item contentItem
    property Flickable flickableItem
    clip: true

    //TODO: horizontalScrollBarPolicy is completely noop just for compatibility right now
    property int horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
    property int verticalScrollBarPolicy: Qt.ScrollBarAsNeeded
    property int topPadding: 0
    property int leftPadding: 0
    property int rightPadding: !Kirigami.Settings.hasTransientTouchInput && flickableItem.ScrollBar.vertical && flickableItem.ScrollBar.vertical.visible ? flickableItem.ScrollBar.vertical.width : 0
    property int bottomPadding: 0

    readonly property Item verticalScrollBar: flickableItem.ScrollBar.vertical ? flickableItem.ScrollBar.vertical : null

    Accessible.onScrollDownAction: flickableItem.Accessible.onScrollDownAction
    Accessible.onScrollUpAction: flickableItem.Accessible.onScrollUpAction

    Keys.onUpPressed: scroll(Kirigami.Units.gridUnit * 2)
    Keys.onDownPressed: scroll(-Kirigami.Units.gridUnit * 2)

    function scroll(y) {
        const minYExtent = flickableItem.topMargin - flickableItem.originY;
        const maxYExtent = flickableItem.height - (flickableItem.contentHeight + flickableItem.bottomMargin + flickableItem.originY);

        flickableItem.contentY = Math.min(-maxYExtent, Math.max(-minYExtent, flickableItem.contentY - y));
    }

    focus: true

    onVerticalScrollBarPolicyChanged: {
        if (flickableItem.ScrollBar.vertical) {
            flickableItem.ScrollBar.vertical.visible = verticalScrollBarPolicy != Qt.ScrollBarAlwaysOff;
        }
        scrollBarCreationTimer.restart();
    }
    onHorizontalScrollBarPolicyChanged: {
        if (flickableItem.ScrollBar.horizontal) {
            flickableItem.ScrollBar.horizontal.visible = horizontalScrollBarPolicy != Qt.ScrollBarAlwaysOff;
        }
        scrollBarCreationTimer.restart();
    }


    onContentItemChanged: {
        if (contentItem.hasOwnProperty("contentY")) {
            flickableItem = contentItem;
            if (typeof(flickableItem.keyNavigationEnabled) != "undefined") {
                flickableItem.keyNavigationEnabled = true;
                flickableItem.keyNavigationWraps = false;
            }
            contentItem.parent = flickableParent;
        } else {
            flickableItem = flickableComponent.createObject(flickableParent);
            contentItem.parent = flickableItem.contentItem;
        }

        flickableItem.interactive = true;
        flickableItem.anchors.fill = flickableParent;

        scrollBarCreationTimer.restart();
    }

    Timer {
        id: scrollBarCreationTimer
        interval: 0
        onTriggered: {
            //create or destroy the vertical scrollbar
            if ((!flickableItem.ScrollBar.vertical) &&
                verticalScrollBarPolicy != Qt.ScrollBarAlwaysOff) {
                flickableItem.ScrollBar.vertical = verticalScrollComponent.createObject(root);
            } else if (flickableItem.ScrollBar.vertical &&
                verticalScrollBarPolicy == Qt.ScrollBarAlwaysOff) {
                flickableItem.ScrollBar.vertical.destroy();
            }

            //create or destroy the horizontal scrollbar
            if ((!flickableItem.ScrollBar.horizontal) &&
                horizontalScrollBarPolicy != Qt.ScrollBarAlwaysOff) {
                flickableItem.ScrollBar.horizontal = horizontalScrollComponent.createObject(root);
            } else if (flickableItem.ScrollBar.horizontal &&
                horizontalScrollBarPolicy == Qt.ScrollBarAlwaysOff) {
                flickableItem.ScrollBar.horizontal.destroy();
            }
        }
    }
    Kirigami.WheelHandler {
        id: wheelHandler
        target: root.flickableItem
    }
    Item {
        id: flickableParent
        clip: true
        anchors {
            fill: parent
            leftMargin: root.leftPadding
            topMargin: root.topPadding
            rightMargin: root.rightPadding
            bottomMargin: root.bottomPadding
        }
    }
    Component {
        id: flickableComponent
        Flickable {
            anchors {
                fill: parent
            }
            contentWidth: root.contentItem ? root.contentItem.width : 0
            contentHeight: root.contentItem ? root.contentItem.height : 0
        }
    }
    Component {
        id: verticalScrollComponent
        ScrollBar {
            z: flickableParent.z + 1
            visible: root.contentItem.visible && size < 1
            interactive: !Kirigami.Settings.hasTransientTouchInput

            //NOTE: use this instead of anchors as crashes on some Qt 5.8 checkouts
            height: parent.height
            anchors {
                right: parent.right
                top: parent.top
            }
        }
    }
    Component {
        id: horizontalScrollComponent
        ScrollBar {
            z: flickableParent.z + 1
            visible: root.contentItem.visible && size < 1
            interactive: !Kirigami.Settings.hasTransientTouchInput

            //NOTE: use this instead of anchors as crashes on some Qt 5.8 checkouts
            height: parent.height - anchors.topMargin
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
        }
    }
}
