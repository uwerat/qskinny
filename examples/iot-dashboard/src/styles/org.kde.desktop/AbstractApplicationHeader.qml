/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.5
import org.kde.kirigami 2.4

import "../../templates" as T


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
        // Always use window bg color for toolbar, even if the page
        // it's located on uses a different color set
        Theme.colorSet: Theme.Window
        Theme.inherit: false
        color: Theme.backgroundColor
        Separator {
            visible: root.separatorVisible && (!root.page || !root.page.header || !root.page.header.visible || root.page.header.toString().indexOf("ToolBar") === -1)
            anchors {
                left: parent.left
                right: parent.right
                bottom: root.y <= 0 ? parent.bottom : undefined
                top: root.y <= 0 ? undefined :  parent.top
            }
        }
    }
}

