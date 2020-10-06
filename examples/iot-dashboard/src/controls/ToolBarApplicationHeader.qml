/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.5
import QtQuick.Controls 2.0 as Controls
import QtQuick.Layouts 1.2
import "private"
import org.kde.kirigami 2.4


/**
 * This Application header represents a toolbar that
 * will display the actions of the current page.
 * Both Contextual actions and the main, left and right actions
 */
ApplicationHeader {
    id: header

    preferredHeight: 42
    maximumHeight: preferredHeight
    headerStyle: ApplicationHeaderStyle.Titles

    //FIXME: needs a property definition to have its own type in qml
    property string _internal: ""

    Component.onCompleted: print("Warning: ToolbarApplicationHeader is deprecated, remove and use the automatic internal toolbar instead.")
    pageDelegate: Item {
        id: delegateItem
        readonly property bool current: __appWindow.pageStack.currentIndex === index
        implicitWidth: titleTextMetrics.width/2 + buttonTextMetrics.collapsedButtonsWidth

        RowLayout {
            id: titleLayout
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                right: actionsLayout.left
            }
            Separator {
                id: separator
                Layout.preferredHeight: parent.height * 0.6
            }

            Heading {
                id: title
                Layout.fillWidth: true

                Layout.preferredWidth: implicitWidth
                Layout.minimumWidth: Math.min(titleTextMetrics.width, delegateItem.width - buttonTextMetrics.requiredWidth)
                leftPadding: Units.largeSpacing
                opacity: delegateItem.current ? 1 : 0.4
                maximumLineCount: 1
                color: Theme.textColor
                elide: Text.ElideRight
                text: page ? page.title : ""
            }
        }

        TextMetrics {
            id: titleTextMetrics
            text: page ? page.title : ""
            font: title.font
        }
        TextMetrics {
            id: buttonTextMetrics
            text: (page.actions.left ? page.actions.left.text : "") + (page.actions.main ? page.actions.main.text : "") + (page.actions.right ? page.actions.right.text : "")
            readonly property int collapsedButtonsWidth: ctxActionsButton.width + (page.actions.left ? ctxActionsButton.width + Units.gridUnit : 0) + (page.actions.main ? ctxActionsButton.width + Units.gridUnit : 0) + (page.actions.right ? ctxActionsButton.width + Units.gridUnit : 0)
            readonly property int requiredWidth: width + collapsedButtonsWidth
        }

        RowLayout {
            id: actionsLayout
            anchors {
                verticalCenter: parent.verticalCenter
                right: ctxActionsButton.visible ? ctxActionsButton.left : parent.right
            }

            readonly property bool toobig: delegateItem.width - titleTextMetrics.width - Units.gridUnit < buttonTextMetrics.requiredWidth

            PrivateActionToolButton {
                Layout.alignment: Qt.AlignVCenter
                kirigamiAction: page && page.actions ? page.actions.left : null
                showText: !parent.toobig
            }
            PrivateActionToolButton {
                Layout.alignment: Qt.AlignVCenter
                Layout.rightMargin: Units.smallSpacing
                kirigamiAction: page && page.actions ? page.actions.main : null
                showText: !parent.toobig
                flat: false
            }
            PrivateActionToolButton {
                Layout.alignment: Qt.AlignVCenter
                kirigamiAction: page && page.actions ? page.actions.right : null
                showText: !parent.toobig
            }
        }

        PrivateActionToolButton {
            id: ctxActionsButton
            showMenuArrow: page.actions.contextualActions.length === 1
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
                rightMargin: Units.smallSpacing
            }
            Action {
                id: overflowAction
                icon.name: "overflow-menu"
                tooltip: qsTr("More Actions")
                visible: children.length > 0
                children: page && page.actions.contextualActions ? page.actions.contextualActions : null
            }

            kirigamiAction: page && page.actions.contextualActions.length === 1 ? page.actions.contextualActions[0] : overflowAction
        }
    }
}
