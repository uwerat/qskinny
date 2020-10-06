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
        property int thickness: Math.floor(Units.devicePixelRatio)*2
        Rectangle {
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                //horizontalCenterOffset: -parent.width/2
                topMargin: (parent.height/2 - iconRoot.thickness/2) * drawer.position
            }
            antialiasing: drawer.position != 0
            transformOrigin: Item.Center
            width: (1 - drawer.position) * height + drawer.position * (Math.sqrt(2*(parent.width*parent.width)))
            height: iconRoot.thickness
            color: canvas.color
            rotation: 45 * drawer.position
        }

        Rectangle {
            anchors.centerIn: parent
            width: height
            height: iconRoot.thickness
            color: canvas.color
        }


        Rectangle {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
             //   topMargin: -iconRoot.thickness/2 * drawer.position
                bottomMargin: (parent.height/2 - iconRoot.thickness/2) * drawer.position
            }
            antialiasing: drawer.position != 0
            transformOrigin: Item.Center
            width: (1 - drawer.position) * height + drawer.position * (Math.sqrt(2*(parent.width*parent.width)))
            height: iconRoot.thickness
            color: canvas.color
            rotation: -45 * drawer.position
        }
    }
}

