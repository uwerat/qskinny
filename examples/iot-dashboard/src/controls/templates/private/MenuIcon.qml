/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.4

Item {
    id: canvas
    width: height
    height: Units.iconSizes.smallMedium
    property OverlayDrawer drawer
    property color color: Theme.textColor
    opacity: 0.8
    layer.enabled: true

    Item {
        id: iconRoot
        anchors {
            fill: parent
            margins: Units.smallSpacing
        }
        readonly property int thickness: Math.floor(Units.devicePixelRatio)*2
        readonly property real drawerPosition: drawer ? drawer.position : 0

        Rectangle {
            anchors {
                right: parent.right
                top: parent.top
                topMargin: -iconRoot.thickness/2 * iconRoot.drawerPosition
            }
            antialiasing: iconRoot.drawerPosition != 0
            transformOrigin: Item.Right
            width: (1 - iconRoot.drawerPosition) * parent.width + iconRoot.drawerPosition * (Math.sqrt(2*(parent.width*parent.width)))
            height: iconRoot.thickness
            color: canvas.color
            rotation: -45 * iconRoot.drawerPosition
        }

        Rectangle {
            anchors.centerIn: parent
            width: parent.width - parent.width * iconRoot.drawerPosition
            height: iconRoot.thickness
            color: canvas.color
        }

        Rectangle {
            anchors {
                right: parent.right
                bottom: parent.bottom
                bottomMargin: -iconRoot.thickness/2 * iconRoot.drawerPosition
            }
            antialiasing: iconRoot.drawerPosition != 0
            transformOrigin: Item.Right
            width: (1 - iconRoot.drawerPosition) * parent.width + iconRoot.drawerPosition * (Math.sqrt(2*(parent.width*parent.width)))
            height: iconRoot.thickness
            color: canvas.color
            rotation: 45 * iconRoot.drawerPosition
        }
    }
}

