/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.4

import "private"
import "templates/private"

/**
 * A drawer specialization that will show a list of actions that are
 * specific of the current page shown by the application
 *
 * Example usage:
 * @code
 * import org.kde.kirigami 2.4 as Kirigami
 *
 * Kirigami.ApplicationWindow {
 *  [...]
 *     contextDrawer: Kirigami.ContextDrawer {
 *         id: contextDrawer
 *     }
 *  [...]
 * }
 * @endcode
 *
 * @code
 * import org.kde.kirigami 2.4 as Kirigami
 *
 * Kirigami.Page {
 *   [...]
 *     contextualActions: [
 *         Kirigami.Action {
 *             iconName: "edit"
 *             text: "Action text"
 *             onTriggered: {
 *                 // do stuff
 *             }
 *         },
 *         Kirigami.Action {
 *             iconName: "edit"
 *             text: "Action text"
 *             onTriggered: {
 *                 // do stuff
 *             }
 *         }
 *     ]
 *   [...]
 * }
 * @endcode
 *
 * @inherit AbstractDrawer
 */
OverlayDrawer {
    id: root
    handleClosedIcon.source: null
    handleOpenIcon.source: null
    /**
     * title: string
     * A title for the action list that will be shown to the user when opens the drawer
     */
    property string title: qsTr("Actions")

    /**
     * actions: list<Action>
     * This can be any type of object that a ListView can accept as model.
     * It expects items compatible with either QAction or Kirigami Action
     */
    property var actions: page ? page.contextualActions : []

    property Page page: {
        if (applicationWindow().pageStack.layers && applicationWindow().pageStack.layers.depth > 1 && applicationWindow().pageStack.layers.currentItem.hasOwnProperty("contextualActions")) {
            return applicationWindow().pageStack.layers.currentItem;
        }
        else if (applicationWindow().pageStack.currentItem.hasOwnProperty("contextualActions")) {
            return applicationWindow().pageStack.currentItem;
        }
        else {
            return applicationWindow().pageStack.lastVisibleItem;
        }
    }

    // Disable for empty menus or when we have a global toolbar
    enabled: menu.count > 0 &&
            (typeof applicationWindow() === "undefined" || !applicationWindow().pageStack.globalToolBar || (applicationWindow().pageStack.lastVisibleItem && applicationWindow().pageStack.lastVisibleItem.globalToolBarStyle !== ApplicationHeaderStyle.ToolBar))
    edge: Qt.application.layoutDirection == Qt.RightToLeft ? Qt.LeftEdge : Qt.RightEdge
    drawerOpen: false

    /**
     * header: Component
     * Arbitrary content that will go on top of the list (by default is the title)
     * @since 2.7
     */
    property alias header: menu.header

    /**
     * footer: Component
     * Arbitrary content that will go on top of the list (by default is empty)
     * @since 2.7
     */
    property alias footer: menu.footer

    //list items go to edges, have their own padding
    leftPadding: 0
    rightPadding: 0
    bottomPadding: 0

    handleVisible: applicationWindow == undefined ? false : applicationWindow().controlsVisible

    onPeekingChanged: {
        if (page) {
            page.contextualActionsAboutToShow();
        }
    }
    contentItem: ScrollView {
        //this just to create the attached property
        Theme.inherit: true
        implicitWidth: Units.gridUnit * 20
        ListView {
            id: menu
            interactive: contentHeight > height
            model: {
                if (typeof root.actions == "undefined") {
                    return null;
                }
                if (root.actions.length === 0) {
                    return null;
                } else {

                    // Check if at least one action is visible
                    var somethingVisible = false;
                    for (var i=0; i<root.actions.length; i++) {
                        if (root.actions[i].visible) {
                            somethingVisible = true;
                            break;
                        }
                    }

                    if (!somethingVisible) {
                        return null;
                    }

                    return root.actions[0].text !== undefined &&
                        root.actions[0].trigger !== undefined ?
                            root.actions :
                            root.actions[0];
                }
            }
            topMargin: root.handle.y > 0 ? menu.height - menu.contentHeight : 0
            header: Item {
                height: heading.height
                width: menu.width
                Heading {
                    id: heading
                    anchors {
                        left: parent.left
                        right: parent.right
                        margins: Units.largeSpacing
                    }
                    elide: Text.ElideRight
                    level: 2
                    text: root.title
                }
            }
            delegate: Column {
                width: parent.width
                ContextDrawerActionItem {
                    width: parent.width
                }
                Repeater {
                    model: modelData.hasOwnProperty("expandible") && modelData.expandible ? modelData.children : null
                    delegate: ContextDrawerActionItem {
                        width: parent.width
                        leftPadding: Units.largeSpacing * 2
                        opacity: !root.collapsed
                    }
                }
            }
        }
    }
}
