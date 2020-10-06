/*
 *  SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.5
import QtQuick.Controls 2.0 as Controls
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.4



AbstractPageHeader {
    id: root

    Loader {
        id: titleLoader

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }
        height: Math.min(root.height, item
            ? (item.Layout.preferredHeight > 0 ? item.Layout.preferredHeight : item.implicitHeight)
            : 0)

        sourceComponent: page ? page.titleDelegate : null
    }
}

