/*
 *  SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import org.kde.kirigami 2.12 as Kirigami

Rectangle {
    Accessible.ignored: true

    anchors {
        bottom: Kirigami.Settings.isMobile ? undefined : parent.bottom
        top: Kirigami.Settings.isMobile ? parent.top : undefined
        left: parent.left
        right: parent.right
    }

    color: {
        if (state == "highlighted") {
            return Kirigami.Theme.activeTextColor
        } else if (state == "requestingAttention") {
            return Kirigami.Theme.negativeTextColor
        }
        return "transparent"
    }

    // Unlike most things, we don't want to scale with the em grid, so we don't use a Unit.
    height: 2
}