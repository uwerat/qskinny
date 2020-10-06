/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0 as Controls

import org.kde.kirigami 2.4

Controls.ToolButton {
    id: button

    icon.name: (LayoutMirroring.enabled ? "go-next-symbolic-rtl" : "go-next-symbolic")

    enabled: applicationWindow().pageStack.currentIndex < applicationWindow().pageStack.depth-1

    visible: applicationWindow().pageStack.layers.depth == 1 && applicationWindow().pageStack.contentItem.contentWidth > applicationWindow().pageStack.width && (globalToolBar.showNavigationButtons === true || (globalToolBar.showNavigationButtons & ApplicationHeaderStyle.ShowForwardButton))
    width: height
    height: parent.height

    onClicked: applicationWindow().pageStack.goForward();



    Controls.ToolTip {
        visible: button.hovered
        text: qsTr("Navigate Forward")
        delay: Units.toolTipDelay
        timeout: 5000
        y: button.height
    }
}
