/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.6
import QtQuick.Window 2.6
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Controls.impl 2.3 as QQC2Impl
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.5

AbstractListItem {
    id: listItem
    supportsMouseEvents: (!isExpandible || root.collapsed)
    readonly property bool wideMode: width > height * 2
    readonly property bool isSeparator: modelData.hasOwnProperty("separator") && modelData.separator

    readonly property bool isExpandible: modelData && modelData.hasOwnProperty("expandible") && modelData.expandible

    checked: modelData.checked || (actionsMenu && actionsMenu.visible)
    width: parent.width

    contentItem: RowLayout {
        Icon {
            id: iconItem
            color: modelData.icon.color
            source: modelData.icon.name || modelData.icon.source
            property int size: Units.iconSizes.smallMedium
            Layout.minimumHeight: size
            Layout.maximumHeight: size
            Layout.minimumWidth: size
            selected: (listItem.highlighted || listItem.checked || (listItem.pressed && listItem.supportsMouseEvents))
            visible: source != undefined
        }
        QQC2Impl.MnemonicLabel {
            id: labelItem
            text:  width > height * 2 ? listItem.MnemonicData.mnemonicLabel : ""
            Layout.fillWidth: true
            mnemonicVisible: listItem.MnemonicData.active
            color: (listItem.highlighted || listItem.checked || (listItem.pressed && listItem.supportsMouseEvents)) ? listItem.activeTextColor : listItem.textColor
            elide: Text.ElideRight
            font: listItem.font
            // Work around Qt bug where NativeRendering breaks for non-integer scale factors
            // https://bugreports.qt.io/browse/QTBUG-67007
            renderType: Screen.devicePixelRatio % 1 !== 0 ? Text.QtRendering : Text.NativeRendering
            opacity: !root.collapsed
            Behavior on opacity {
                NumberAnimation {
                    duration: Units.longDuration/2
                    easing.type: Easing.InOutQuad
                }
            }
        }
        Separator {
            id: separatorAction

            visible: listItem.isSeparator
            Layout.fillWidth: true
        }

        Icon {
            Shortcut {
                sequence: listItem.MnemonicData.sequence
                onActivated: listItem.clicked()
            }
            isMask: true
            Layout.alignment: Qt.AlignVCenter
            Layout.leftMargin: !root.collapsed ? 0 : -width
            Layout.preferredHeight: !root.collapsed ? Units.iconSizes.smallMedium : Units.iconSizes.small/2
            selected: listItem.checked || listItem.pressed
            Layout.preferredWidth: Layout.preferredHeight
            source: (LayoutMirroring.enabled ? "go-next-symbolic-rtl" : "go-next-symbolic")
            visible: (!isExpandible || root.collapsed) && !listItem.isSeparator && modelData.hasOwnProperty("children") && modelData.children!==undefined && modelData.children.length > 0
        }
    }
    MnemonicData.enabled: listItem.enabled && listItem.visible
    MnemonicData.controlType: MnemonicData.MenuItem
    MnemonicData.label: modelData.text
    property ActionsMenu actionsMenu: ActionsMenu {
        x: Qt.application.layoutDirection === Qt.RightToLeft ? -width : listItem.width
        actions: modelData.hasOwnProperty("children") ? modelData.children : null
        submenuComponent: Component {
            ActionsMenu {}
        }
        onVisibleChanged: {
            if (visible) {
                stackView.openSubMenu = listItem.actionsMenu;
            } else if (stackView.openSubMenu === listItem.actionsMenu) {
                stackView.openSubMenu = null;
            }
        }
    }

    separatorVisible: false
    //TODO: animate the hide by collapse
    visible: (model ? model.visible || model.visible===undefined : modelData.visible) && opacity > 0
    opacity: !root.collapsed || iconItem.source.length > 0
    Behavior on opacity {
        NumberAnimation {
            duration: Units.longDuration/2
            easing.type: Easing.InOutQuad
        }
    }
    enabled: !isSeparator && ( (model && model.enabled !== undefined) ? model.enabled : modelData.enabled)

    hoverEnabled: (!isExpandible || root.collapsed) && !Settings.tabletMode
    sectionDelegate: isExpandible
    font.pointSize: isExpandible ? Theme.defaultFont.pointSize * 1.30 : Theme.defaultFont.pointSize
    height: implicitHeight * opacity

    data: [
        QQC2.ToolTip {
            visible: !listItem.isSeparator && (modelData.hasOwnProperty("tooltip") && modelData.tooltip.length || root.collapsed) && (!actionsMenu || !actionsMenu.visible) &&  listItem.hovered && text.length > 0
            text: modelData.hasOwnProperty("tooltip") && modelData.tooltip.length ? modelData.tooltip : modelData.text
            delay: Units.toolTipDelay
            timeout: 5000
            y: listItem.height/2 - height/2
            x: Qt.application.layoutDirection === Qt.RightToLeft ? -width : listItem.width
        }
    ]

    onHoveredChanged: {
        if (!hovered) {
            return;
        }
        if (stackView.openSubMenu) {
            stackView.openSubMenu.visible = false;

            if (!listItem.actionsMenu.hasOwnProperty("count") || listItem.actionsMenu.count>0) {
                if (listItem.actionsMenu.hasOwnProperty("popup")) {
                    listItem.actionsMenu.popup(listItem, listItem.width, 0)
                } else {
                    listItem.actionsMenu.visible = true;
                }
            }
        }
    }
    onClicked: {
        if (!supportsMouseEvents) {
            return;
        }
        modelData.trigger();
        if (modelData.hasOwnProperty("children") && modelData.children!==undefined && modelData.children.length > 0) {
            if (root.collapsed) {
                //fallbacks needed for Qt 5.9
                if ((!listItem.actionsMenu.hasOwnProperty("count") || listItem.actionsMenu.count>0) && !listItem.actionsMenu.visible) {
                    stackView.openSubMenu = listItem.actionsMenu;
                    if (listItem.actionsMenu.hasOwnProperty("popup")) {
                        listItem.actionsMenu.popup(listItem, listItem.width, 0)
                    } else {
                        listItem.actionsMenu.visible = true;
                    }
                }
            } else {
                stackView.push(menuComponent, {model: modelData.children, level: level + 1, current: modelData });
            }
        } else if (root.resetMenuOnTriggered) {
            root.resetMenu();
        }
        checked = Qt.binding(function() { return modelData.checked || (actionsMenu && actionsMenu.visible) });
    }
}
