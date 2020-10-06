/*
 *  SPDX-FileCopyrightText: 2019 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.6
import org.kde.kirigami 2.12 as Kirigami

Kirigami.ShadowedRectangle {
    color: Kirigami.Theme.backgroundColor

    radius: Kirigami.Units.smallSpacing

    shadow.size: Kirigami.Units.largeSpacing
    shadow.color: Qt.rgba(0.0, 0.0, 0.0, 0.15)
    shadow.yOffset: Kirigami.Units.devicePixelRatio * 2

    border.width: Kirigami.Units.devicePixelRatio
    border.color: Qt.tint(Kirigami.Theme.textColor,
                          Qt.rgba(color.r, color.g, color.b, 0.6))
}

