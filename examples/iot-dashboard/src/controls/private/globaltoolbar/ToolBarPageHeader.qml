/*
 *  SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.5
import QtQuick.Controls 2.0 as Controls
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.14
import "../" as Private


AbstractPageHeader {
    id: root

    implicitWidth: layout.implicitWidth + Units.smallSpacing * 2
    Layout.preferredHeight: Math.max(titleLoader.implicitHeight, toolBar.implicitHeight) + Units.smallSpacing * 2

    MouseArea {
        anchors.fill: parent
        onClicked: page.forceActiveFocus()
    }

    RowLayout {
        id: layout
        anchors.fill: parent
        anchors.rightMargin: Units.smallSpacing
        spacing: Units.smallSpacing

        Loader {
            id: titleLoader

            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            Layout.fillWidth: item ? item.Layout.fillWidth : undefined
            Layout.minimumWidth: item ? item.Layout.minimumWidth : undefined
            Layout.preferredWidth: item ? item.Layout.preferredWidth : undefined
            Layout.maximumWidth: item ? item.Layout.maximumWidth : undefined

            sourceComponent: page ? page.titleDelegate : null
        }

        ActionToolBar {
            id: toolBar

            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            Layout.fillHeight: true

            visible: actions.length > 0
            alignment: pageRow.globalToolBar.toolbarActionAlignment
            heightMode: ToolBarLayout.ConstrainIfLarger

            actions: {
                var result = []

                if (page) {
                    if (page.actions.main) {
                        page.actions.main.displayHint |= DisplayHint.KeepVisible
                        result.push(page.actions.main)
                    }
                    if (page.actions.left) {
                        page.actions.left.displayHint |= DisplayHint.KeepVisible
                        result.push(page.actions.left)
                    }
                    if (page.actions.right) {
                        page.actions.right.displayHint |= DisplayHint.KeepVisible
                        result.push(page.actions.right)
                    }
                    if (page.actions.contextualActions.length > 0) {
                        result = result.concat(Array.prototype.map.call(page.actions.contextualActions, function(item) { return item }))
                    }
                }

                return result
            }
        }
    }
}

