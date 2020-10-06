/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.5
import org.kde.kirigami 2.5

import "private"
import "templates" as T


/**
 * An item that can be used as a title for the application.
 * Scrolling the main page will make it taller or shorter (trough the point of going away)
 * It's a behavior similar to the typical mobile web browser addressbar
 * the minimum, preferred and maximum heights of the item can be controlled with
 * * minimumHeight: default is 0, i.e. hidden
 * * preferredHeight: default is Units.gridUnit * 1.6
 * * maximumHeight: default is Units.gridUnit * 3
 *
 * To achieve a titlebar that stays completely fixed just set the 3 sizes as the same
 */
T.AbstractApplicationHeader {
    id: root

    Theme.inherit: false
    Theme.colorSet: Theme.Header

    background: Rectangle {
        color: Theme.backgroundColor
        EdgeShadow {
            id: shadow
            visible: root.separatorVisible
            anchors {
                right: parent.right
                left: parent.left
                top: parent.bottom
            }
            edge: Qt.TopEdge
            opacity: (!root.page.header || root.page.header.toString().indexOf("ToolBar") === -1)
            Behavior on opacity {
                OpacityAnimator {
                    duration: Units.longDuration
                    easing.type: Easing.InOutQuad
                }
            }
        }
        Behavior on opacity {
            OpacityAnimator {
                duration: Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }
    }
}

