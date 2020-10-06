/*
 *  SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.6
import QtQuick.Controls 2.12 as QQC2
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.4 as Kirigami
import "../../templates/private" as TemplatesPrivate
import "../" as Private

Kirigami.AbstractApplicationHeader {
    id: header
    readonly property int leftReservedSpace: (buttonsLayout.visible && buttonsLayout.visibleChildren.length > 0 ? buttonsLayout.width : 0) + (leftHandleAnchor.visible ? leftHandleAnchor.width  : 0) + (menuButton.visible ? menuButton.width  : 0)
    readonly property int rightReservedSpace: rightHandleAnchor.visible ? backButton.background.implicitHeight : 0

    readonly property alias leftHandleAnchor: leftHandleAnchor
    readonly property alias rightHandleAnchor: rightHandleAnchor

    readonly property bool breadcrumbVisible: layerIsMainRow && breadcrumbLoader.active
    readonly property bool layerIsMainRow: root.layers.currentItem == root.contentItem
    readonly property Item currentItem: layerIsMainRow ? root.currentItem : root.layers.currentItem

    height: visible ? implicitHeight : 0
    minimumHeight: globalToolBar.minimumHeight
    preferredHeight: globalToolBar.preferredHeight
    maximumHeight: globalToolBar.maximumHeight
    separatorVisible: globalToolBar.separatorVisible

    Kirigami.Theme.textColor: currentItem ? currentItem.Kirigami.Theme.textColor : parent.Kirigami.Theme.textColor

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            id: leftHandleAnchor
            visible: (typeof applicationWindow() !== "undefined" && applicationWindow().globalDrawer && applicationWindow().globalDrawer.enabled && applicationWindow().globalDrawer.handleVisible &&
            (applicationWindow().globalDrawer.handle.handleAnchor == (Qt.application.layoutDirection == Qt.LeftToRight ? leftHandleAnchor : rightHandleAnchor))) &&
            breadcrumbLoader.pageRow.firstVisibleItem &&
            breadcrumbLoader.pageRow.firstVisibleItem.globalToolBarStyle == Kirigami.ApplicationHeaderStyle.ToolBar


            Layout.preferredHeight: Math.min(backButton.implicitHeight, parent.height)
            Layout.preferredWidth: height
        }

        Private.PrivateActionToolButton {
            id: menuButton
            visible: !Kirigami.Settings.isMobile && applicationWindow().globalDrawer && "isMenu" in applicationWindow().globalDrawer && applicationWindow().globalDrawer.isMenu
            icon.name: "application-menu"
            showMenuArrow: false

            Layout.preferredHeight: Math.min(backButton.implicitHeight, parent.height)
            Layout.preferredWidth: height
            Layout.leftMargin: Kirigami.Units.smallSpacing

            action: Kirigami.Action {
                children: applicationWindow().globalDrawer ? applicationWindow().globalDrawer.actions : []
            }
        }

        RowLayout {
            id: buttonsLayout
            Layout.fillHeight: true

            Layout.leftMargin: leftHandleAnchor.visible ? Kirigami.Units.smallSpacing : 0

            visible: (globalToolBar.showNavigationButtons != Kirigami.ApplicationHeaderStyle.NoNavigationButtons || root.layers.depth > 1) && (globalToolBar.actualStyle != Kirigami.ApplicationHeaderStyle.None)

            Layout.maximumWidth: visibleChildren.length > 0 ? implicitWidth : 0

            TemplatesPrivate.BackButton {
                id: backButton
                Layout.leftMargin: leftHandleAnchor.visible ? 0 : Kirigami.Units.smallSpacing
                Layout.preferredWidth: height
            }
            TemplatesPrivate.ForwardButton {
                Layout.preferredWidth: height
            }
        }

        QQC2.ToolSeparator {
            visible: (menuButton.visible || buttonsLayout.visible) && breadcrumbVisible && pageRow.depth > 1
        }

        Loader {
            id: breadcrumbLoader
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: -1
            Layout.preferredHeight: -1
            property Kirigami.PageRow pageRow: root

            opacity: layerIsMainRow && active
            enabled: opacity > 0

            active: (globalToolBar.actualStyle == Kirigami.ApplicationHeaderStyle.TabBar || globalToolBar.actualStyle == Kirigami.ApplicationHeaderStyle.Breadcrumb) && currentItem && currentItem.globalToolBarStyle != Kirigami.ApplicationHeaderStyle.None

            //TODO: different implementation?
            source: globalToolBar.actualStyle == Kirigami.ApplicationHeaderStyle.TabBar ? Qt.resolvedUrl("TabBarControl.qml") : Qt.resolvedUrl("BreadcrumbControl.qml")

            Behavior on opacity {
                OpacityAnimator {
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.InOutQuad
                }
            }
        }

        Item {
            id: rightHandleAnchor
            visible: (typeof applicationWindow() !== "undefined" &&
                    applicationWindow().contextDrawer &&
                    applicationWindow().contextDrawer.enabled &&
                    applicationWindow().contextDrawer.handleVisible &&
                    applicationWindow().contextDrawer.handle.handleAnchor == (Qt.application.layoutDirection == Qt.LeftToRight ? rightHandleAnchor : leftHandleAnchor) &&
                    breadcrumbLoader.pageRow && breadcrumbLoader.pageRow.lastVisibleItem.globalToolBarStyle == Kirigami.ApplicationHeaderStyle.ToolBar)
            Layout.fillHeight: true
            Layout.preferredWidth: height
        }
    }
    background.opacity: breadcrumbLoader.opacity
}

