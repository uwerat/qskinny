/*
 *  SPDX-FileCopyrightText: 2010 Marco Martin <notmart@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.5
import org.kde.kirigami 2.4
import QtQuick.Controls.Material 2.1 as Mat
import QtQuick.Controls.Material.impl 2.1 as MatImp
import "../../private"
import "../../templates" as T

/**
 * An item delegate Intended to support extra actions obtainable
 * by uncovering them by dragging away the item with the handle
 * This acts as a container for normal list items.
 * Any subclass of AbstractListItem can be assigned as the contentItem property.
 * @code
 * ListView {
 *     model: myModel
 *     delegate: SwipeListItem {
 *         Label {
 *             text: model.text
 *         }
 *         actions: [
 *              Action {
 *                  iconName: "document-decrypt"
 *                  onTriggered: print("Action 1 clicked")
 *              },
 *              Action {
 *                  iconName: model.action2Icon
 *                  onTriggered: //do something
 *              }
 *         ]
 *     }
 *
 * }
 * @endcode
 *
 * @inherit QtQuick.Item
 */
T.SwipeListItem {
    id: listItem

    background: DefaultListItemBackground {
        MatImp.Ripple {
            anchors.fill: parent
            clip: visible
            pressed: listItem.pressed
            anchor: listItem
            active: listItem.down || listItem.visualFocus
            color: Qt.rgba(0,0,0,0.2)
        }
    }
    implicitHeight: contentItem.implicitHeight + Units.smallSpacing * 6
}
