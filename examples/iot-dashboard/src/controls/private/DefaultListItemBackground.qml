/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1
import org.kde.kirigami 2.12

Rectangle {
    id: background
    color: listItem.checked || listItem.highlighted || (listItem.supportsMouseEvents && listItem.pressed && !listItem.checked && !listItem.sectionDelegate)
        ? listItem.activeBackgroundColor
        : (listItem.alternatingBackground && index%2 ? listItem.alternateBackgroundColor : listItem.backgroundColor)

    visible: listItem.ListView.view ? listItem.ListView.view.highlight === null : true
    Rectangle {
        id: internal
        property bool indicateActiveFocus: listItem.pressed || Settings.tabletMode || listItem.activeFocus || (listItem.ListView.view ? listItem.ListView.view.activeFocus : false)
        anchors.fill: parent
        visible: !Settings.tabletMode && listItem.supportsMouseEvents
        color: listItem.activeBackgroundColor
        opacity: (listItem.hovered || listItem.highlighted || listItem.activeFocus) && !listItem.pressed ? 0.5 : 0
        Behavior on opacity { NumberAnimation { duration: Units.longDuration } }
    }

    readonly property bool __separatorVisible: listItem.separatorVisible

    Separator {
        id: topSeparator
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.top
        }
        visible: background.__separatorVisible
        weight: Separator.Weight.Light
    }

    Separator {
        id: bottomSeparator
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        visible: background.__separatorVisible
        weight: topSeparator.weight
    }
}

