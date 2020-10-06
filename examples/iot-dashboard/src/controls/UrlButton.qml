/*
 *  SPDX-FileCopyrightText: 2018 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.2
import org.kde.kirigami.private 2.6 as KirigamiPrivate
import QtQuick.Controls 2.1 as QQC2

/**
 * A link button that contains a URL
 *
 * It will open the url by default, allow to copy it if triggered with the
 * secondary mouse button.
 *
 * @since 5.63
 * @since org.kde.kirigami 2.6
 */
LinkButton
{
    id: button
    property string url
    text: url
    enabled: !!url
    visible: text.length > 0
    acceptedButtons: Qt.LeftButton | Qt.RightButton

    onPressed: if (mouse.button === Qt.RightButton) {
        menu.popup()
    }
    onClicked: if (mouse.button !== Qt.RightButton) {
        Qt.openUrlExternally(url)
    }

    QQC2.ToolTip {
        // If button's text has been overridden, show a tooltip to expose the raw URL
        visible: button.text != button.url && button.mouseArea.containsMouse
        text: url
    }

    QQC2.Menu {
        id: menu
        QQC2.MenuItem {
            text: qsTr("Copy link address")
            onClicked: KirigamiPrivate.CopyHelperPrivate.copyTextToClipboard(button.url)
        }
    }
}
