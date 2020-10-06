/*
 *  SPDX-FileCopyrightText: 2018 Eike Hein <hein@kde.org>
 *  SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>
 *  SPDX-FileCopyrightText: 2018 Kai Uwe Broulik <kde@privat.broulik.de>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.7
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.5 as Kirigami

import "../../private"
import "../../templates" as T

/**
 * An inline message item with support for informational, positive,
 * warning and error types, and with support for associated actions.
 *
 * InlineMessage can be used to give information to the user or
 * interact with the user, without requiring the use of a dialog.
 *
 * The InlineMessage item is hidden by default. It also manages its
 * height (and implicitHeight) during an animated reveal when shown.
 * You should avoid setting height on an InlineMessage unless it is
 * already visible.
 *
 * Optionally an icon can be set, defaulting to an icon appropriate
 * to the message type otherwise.
 *
 * Optionally a close button can be shown.
 *
 * Actions are added from left to right. If more actions are set than
 * can fit, an overflow menu is provided.
 *
 * Example:
 * @code
 * InlineMessage {
 *     type: Kirigami.MessageType.Error
 *
 *     text: "My error message"
 *
 *     actions: [
 *         Kirigami.Action {
 *             iconName: "edit"
 *             text: "Action text"
 *             onTriggered: {
 *                 // do stuff
 *             }
 *         },
 *         Kirigami.Action {
 *             iconName: "edit"
 *             text: "Action text"
 *             onTriggered: {
 *                 // do stuff
 *             }
 *         }
 *     ]
 * }
 * @endcode
 *
 * @since 5.45
 */

T.InlineMessage {
    id: root

    background: Rectangle {
        id: bgBorderRect

        color: {
            if (root.type == Kirigami.MessageType.Positive) {
                return Kirigami.Theme.positiveTextColor;
            } else if (root.type == Kirigami.MessageType.Warning) {
                return Kirigami.Theme.neutralTextColor;
            } else if (root.type == Kirigami.MessageType.Error) {
                return Kirigami.Theme.negativeTextColor;
            }

            return Kirigami.Theme.activeTextColor;
        }

        radius: Kirigami.Units.smallSpacing / 2

        Rectangle {
            id: bgFillRect

            anchors.fill: parent
            anchors.margins: Kirigami.Units.devicePixelRatio

            color: Kirigami.Theme.backgroundColor

            radius: bgBorderRect.radius * 0.60
        }

        Rectangle {
            anchors.fill: bgFillRect

            color: bgBorderRect.color

            opacity: 0.20

            radius: bgFillRect.radius
        }

        layer.enabled: true
        layer.effect: DropShadow {
            horizontalOffset: 0
            verticalOffset: 1
            radius: 12
            samples: 32
            color: Qt.rgba(0, 0, 0, 0.5)
        }
    }
}
