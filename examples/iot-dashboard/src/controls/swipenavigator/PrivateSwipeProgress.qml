/*
 *  SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import org.kde.kirigami 2.12 as Kirigami

Item {
    id: __progressRoot
    property var progress

    Rectangle {
        Accessible.ignored: true

        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }
        width: parent.width * __progressRoot.progress
        color: Kirigami.ColorUtils.adjustColor(Kirigami.Theme.positiveTextColor, {"alpha": 0.2*255})

        Rectangle {
            anchors {
                bottom: Kirigami.Settings.isMobile ? undefined : parent.bottom
                top: Kirigami.Settings.isMobile ? parent.top : undefined
                left: parent.left
                right: parent.right
            }

            color: Kirigami.Theme.positiveTextColor

            // Unlike most things, we don't want to scale with the em grid, so we don't use a Unit.
            height: 2
        }
    }


    Rectangle {
        Accessible.ignored: true

        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
        width: parent.width - (parent.width * __progressRoot.progress)
        color: Kirigami.ColorUtils.adjustColor(Kirigami.Theme.textColor, {"alpha": 0.1*255})

        Rectangle {
            anchors {
                bottom: Kirigami.Settings.isMobile ? undefined : parent.bottom
                top: Kirigami.Settings.isMobile ? parent.top : undefined
                left: parent.left
                right: parent.right
            }

            color: Kirigami.ColorUtils.adjustColor(Kirigami.Theme.textColor, {"alpha": 0.1*255})

            // Unlike most things, we don't want to scale with the em grid, so we don't use a Unit.
            height: 2
        }
    }
}