/*
 *  SPDX-FileCopyrightText: 2019 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.6
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.5

BasicListItem {
    id: listItem

    readonly property bool isSeparator: modelData.hasOwnProperty("separator") && modelData.separator

    readonly property bool isExpandible: modelData && modelData.hasOwnProperty("expandible") && modelData.expandible

    checked: modelData.checked
    icon: modelData.icon
    separatorVisible: false
    reserveSpaceForIcon: !isSeparator
    reserveSpaceForLabel: !isSeparator

    label: model ? (model.text ? model.text : model.tooltip) : (modelData.text ? modelData.text : modelData.tooltip)
    hoverEnabled: (!isExpandible || root.collapsed) && !Settings.tabletMode
    sectionDelegate: isExpandible
    font.pointSize: isExpandible ? Theme.defaultFont.pointSize * 1.30 : Theme.defaultFont.pointSize

    enabled: !isExpandible && !isSeparator && (model ? model.enabled : modelData.enabled)
    visible: model ? model.visible : modelData.visible
    opacity: enabled || isExpandible ? 1.0 : 0.6

    Separator {
        id: separatorAction

        visible: listItem.isSeparator
        Layout.fillWidth: true
    }

    ActionsMenu {
        id: actionsMenu
        y: Settings.isMobile ? -height : listItem.height
        z: 99999999
        actions: modelData.children
        submenuComponent: Component {
            ActionsMenu {}
        }
    }

    Loader {
        Layout.fillWidth: true
        Layout.fillHeight: true
        sourceComponent: modelData.displayComponent
        onStatusChanged: {
            for (var i in parent.children) {
                var child = parent.children[i];
                if (child == this) {
                    child.visible = status === Loader.Ready;
                    break;
                } else {
                    child.visible = status !== Loader.Ready;
                }
            }
        }
        Component.onCompleted: statusChanged()
    }

    Icon {
        isMask: true
        Layout.alignment: Qt.AlignVCenter
        Layout.preferredHeight: Units.iconSizes.small/2
        selected: listItem.checked || listItem.pressed
        Layout.preferredWidth: Layout.preferredHeight
        source: "go-up-symbolic"
        visible: !isExpandible  && !listItem.isSeparator && modelData.children!== undefined && modelData.children.length > 0
    }

    onPressed: {
        if (modelData.children.length > 0) {
            actionsMenu.open();
        }
    }
    onClicked: {
        if (modelData.children.length === 0) {
            root.drawerOpen = false;
        }

        if (modelData && modelData.trigger !== undefined) {
            modelData.trigger();
        // assume the model is a list of QAction or Action
        } else if (menu.model.length > index) {
            menu.model[index].trigger();
        } else {
            console.warning("Don't know how to trigger the action")
        }
    }
}
