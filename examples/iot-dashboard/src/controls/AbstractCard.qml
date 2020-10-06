/*
 *  SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.6
import org.kde.kirigami 2.12
import "templates" as T
import "private"

/**
 * A AbstractCard is the base for cards. A Card is a visual object that serves
 * as an entry point for more detailed information. An abstractCard is empty,
 * providing just the look and the base properties and signals for an ItemDelegate.
 * It can be filled with any custom layout of items, its content is organized
 * in 3 properties: header, contentItem and footer.
 * Use this only when you need particular custom contents, for a standard layout
 * for cards, use the Card component.
 *
 * @see Card
 * @inherits T.AbstractCard
 * @since 2.4
 */
T.AbstractCard {
    id: root

    background: DefaultCardBackground {
        id: bg

        readonly property color pressedColor: ColorUtils.tintWithAlpha(Theme.backgroundColor, Theme.highlightColor, 0.3)
        readonly property color hoverColor: ColorUtils.tintWithAlpha(Theme.backgroundColor, Theme.highlightColor, 0.1)

        color: {
            if (root.showClickFeedback || highlighted) {
                if (root.highlighted || root.down) {
                    return bg.pressedColor
                } else if (root.hovered) {
                    return bg.hoverColor
                }
            }
            return Theme.backgroundColor
        }
        Behavior on color {
            ColorAnimation {
                duration: Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }
    }
}
