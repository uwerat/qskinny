/*
 *  SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.14 as Kirigami
import "private"

/**
 * This is a simple toolbar built out of a list of actions
 * each action is represented by a ToolButton, those that won't fit
 * the size will go in a menu under a button with the overflow ... icon
 *
 * @inherits Item
 * @since 2.5
 */
Controls.Control {
    id: root
    /**
    * actions: list<Action>
    * if the card should provide clickable actions, put them in this property,
    * they will be put in the footer as a list of ToolButtons plus an optional
    * overflow menu, when not all of them will fit in the available Card width.
    */
    property alias actions: layout.actions

    /**
    * actions: hiddenActions<Action>
    * This list of actions is for those you always want in the menu, even if there
    * is enough space.
    * @since 2.6
    * @deprecated since 2.14, use the AlwaysHide hint on actions instead.
    */
    property list<QtObject> hiddenActions
    onHiddenActionsChanged: print("ActionToolBar::hiddenActions is deprecated, use the AlwaysHide hint on your actions instead")

    /**
     * flat: bool
     * Wether we want our buttons to have a flat appearance. Default: true
     */
    property bool flat: true

    /**
     * display: enum
     * This controls the label position regarding the icon, is the same value to control individual Button components,
     * permitted values are:
     * * Button.IconOnly
     * * Button.TextOnly
     * * Button.TextBesideIcon
     * * Button.TextUnderIcon
     */
    property int display: Controls.Button.TextBesideIcon

    property alias alignment: layout.alignment

    /**
     * position enum
     * This property holds the position of the toolbar.
     * if this ActionToolBar is the contentItem of a QQC2 Toolbar, the position is binded to the ToolBar's position
     *
     * permitted values are:
     * *ToolBar.Header: The toolbar is at the top, as a window or page header.
     * *ToolBar.Footer: The toolbar is at the bottom, as a window or page footer.
     */
    property int position: parent && parent.hasOwnProperty("position")
            ? parent.position
            : Controls.ToolBar.Header

    /**
     * The maximum width of the contents of this ToolBar. If the toolbar's width is larger than this value, empty space will
     * be added on the sides, according to the Alignment property.
     *
     * The value of this property is derived from the ToolBar's actions and their properties.
     */
    readonly property alias maximumContentWidth: layout.implicitWidth

    /**
     * The name of the icon to use for the overflow menu button.
     *
     * @since 5.65
     * @since 2.12
     */
    property string overflowIconName: "overflow-menu"

    property alias visibleWidth: layout.visibleWidth

    /**
     * Exposes heightMode of the internal layout.
     *
     * \sa ToolBarLayout::heightMode
     */
    property alias heightMode: layout.heightMode

    implicitHeight: layout.implicitHeight
    implicitWidth: layout.implicitWidth

    Layout.minimumWidth: layout.minimumWidth
    Layout.preferredWidth: 0
    Layout.fillWidth: true

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    contentItem: Kirigami.ToolBarLayout {
        id: layout
        spacing: Kirigami.Units.smallSpacing
        layoutDirection: root.LayoutMirroring.enabled ? Qt.RightToLeft : Qt.LeftToRight

        fullDelegate: PrivateActionToolButton {
            flat: root.flat && !action.icon.color.a
            display: root.display
            action: Kirigami.ToolBarLayout.action
        }

        iconDelegate: PrivateActionToolButton {
            flat: root.flat && !action.icon.color.a
            display: Controls.Button.IconOnly
            action: Kirigami.ToolBarLayout.action

            showMenuArrow: false

            menuActions: {
                if (action.displayComponent) {
                    return [action]
                }

                if (action.children) {
                    return Array.prototype.map.call(action.children, i => i)
                }

                return []
            }
        }

        moreButton: PrivateActionToolButton {
            flat: root.flat

            action: Kirigami.Action {
                tooltip: qsTr("More Actions")
                icon.name: root.overflowIconName
                displayHint: Kirigami.DisplayHint.IconOnly | Kirigami.DisplayHint.HideChildIndicator
            }

            menuActions: {
                if (root.hiddenActions.length == 0) {
                    return root.actions
                } else {
                    result = []
                    result.concat(Array.prototype.map.call(root.actions, (i) => i))
                    result.concat(Array.prototype.map.call(hiddenActions, (i) => i))
                    return result
                }
            }

            menuComponent: ActionsMenu {
                submenuComponent: ActionsMenu {
                    Binding {
                        target: parentItem
                        property: "visible"
                        value: layout.hiddenActions.includes(parentAction)
                               && (parentAction.visible === undefined || parentAction.visible)
                    }
                }

                itemDelegate: ActionMenuItem {
                    visible: layout.hiddenActions.includes(action)
                             && (action.visible === undefined || action.visible)
                }

                loaderDelegate: Loader {
                    property var action
                    height: visible ? implicitHeight : 0
                    visible: layout.hiddenActions.includes(action)
                             && (action.visible === undefined || action.visible)
                }

                separatorDelegate: Controls.MenuSeparator {
                    property var action
                    visible: layout.hiddenActions.includes(action)
                             && (action.visible === undefined || action.visible)
                }
            }
        }
    }
}
