/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <notmart@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.7
import org.kde.kirigami 2.4
import QtQuick.Controls.Material 2.1 as Mat
import QtQuick.Controls.Material.impl 2.1 as MatImp
import "../../private"
import "../../templates" as T

T.AbstractListItem {
    id: listItem

    background: DefaultListItemBackground {

        MatImp.Ripple {
            anchors.fill: parent
            clip: visible
            visible: listItem.supportsMouseEvents
            pressed: listItem.pressed
            anchor: listItem
            active: listItem.down || listItem.visualFocus
            color: Qt.rgba(0,0,0,0.2)
        }
    }
    implicitHeight: contentItem.implicitHeight + Units.smallSpacing * 6
}
