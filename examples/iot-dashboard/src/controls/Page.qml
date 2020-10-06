/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.5
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.10 as Kirigami
import "private"
import QtQuick.Templates 2.1 as T2
import QtQuick.Controls 2.1 as QQC2

/**
 * Page is a container for all the app pages: everything pushed to the
 * ApplicationWindow's pageStack should be a Page.
 *
 * For content that should be scrollable, such as ListViews, use ScrollablePage instead.
 *
 * @see ScrollablePage
 * @inherit QtQuick.Templates.Page
 */
QQC2.Page {
    id: root

    /**
     * leftPadding: int
     * default contents padding at left
     */
    leftPadding: Kirigami.Units.gridUnit

    /**
     * topPadding: int
     * default contents padding at top
     */
    topPadding: Kirigami.Units.gridUnit

    /**
     * rightPadding: int
     * default contents padding at right
     */
    rightPadding: Kirigami.Units.gridUnit

    /**
     * bottomPadding: int
     * default contents padding at bottom
     */
    bottomPadding: actionButtons.item ? actionButtons.height : Kirigami.Units.gridUnit

    /**
     * flickable: Flickable
     * if the central element of the page is a Flickable
     * (ListView and Gridview as well) you can set it there.
     * normally, you wouldn't need to do that, but just use the
     * ScrollablePage element instead
     * @see ScrollablePage
     * Use this if your flickable has some non standard properties, such as not covering the whole Page
     */
    property Flickable flickable

    /**
     * actions.contextualActions: list<QtObject>
     * Defines the contextual actions for the page:
     * an easy way to assign actions in the right sliding panel
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
     *     actions.contextualActions: [
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
     */
    //TODO: remove
    property alias contextualActions: actionsGroup.contextualActions

    /**
     * actions.main: Action
     * An optional single action for the action button.
     * it can be a Kirigami.Action or a QAction
     *
     * Example usage:
     *
     * @code
     * import org.kde.kirigami 2.4 as Kirigami
     * Kirigami.Page {
     *     actions.main: Kirigami.Action {
     *         iconName: "edit"
     *         onTriggered: {
     *             // do stuff
     *         }
     *     }
     * }
     * @endcode
     */
    //TODO: remove
    property alias mainAction: actionsGroup.main

    /**
     * actions.left: Action
     * An optional extra action at the left of the main action button.
     * it can be a Kirigami.Action or a QAction
     *
     * Example usage:
     *
     * @code
     * import org.kde.kirigami 2.4 as Kirigami
     * Kirigami.Page {
     *     actions.left: Kirigami.Action {
     *         iconName: "edit"
     *         onTriggered: {
     *             // do stuff
     *         }
     *     }
     * }
     * @endcode
     */
    //TODO: remove
    property alias leftAction: actionsGroup.left

    /**
     * actions.right: Action
     * An optional extra action at the right of the main action button.
     * it can be a Kirigami.Action or a QAction
     *
     * Example usage:
     *
     * @code
     * import org.kde.kirigami 2.4 as Kirigami
     * Kirigami.Page {
     *     actions.right: Kirigami.Action {
     *         iconName: "edit"
     *         onTriggered: {
     *             // do stuff
     *         }
     *     }
     * }
     * @endcode
     */
    //TODO: remove
    property alias rightAction: actionsGroup.right

    /**
     * Actions properties are grouped.
     *
     * @code
     * import org.kde.kirigami 2.4 as Kirigami
     * Kirigami.Page {
     *     actions {
     *         main: Kirigami.Action {...}
     *         left: Kirigami.Action {...}
     *         right: Kirigami.Action {...}
     *         contextualActions: [
     *             Kirigami.Action {...},
     *             Kirigami.Action {...}
     *         ]
     *     }
     * }
     * @endcode
     */
    readonly property alias actions: actionsGroup

    /**
     * contextualActionsAboutToShow: signal
     * Emitted when a visualization for the actions is about to be shown,
     * such as the toolbar menu or the contextDrawer
     * @since 2.7
     */
    signal contextualActionsAboutToShow

    /**
     * isCurrentPage: bool
     *
     * Specifies if it's the currently selected page in the window's pages row, or if layers
     * are used whether this is the topmost item on the layers stack. If the page is
     * not attached to either a column view or a stack view, expect this to be true.
     *
     * @since 2.1
     */
    readonly property bool isCurrentPage: Kirigami.ColumnView.view
            ? (Kirigami.ColumnView.index == Kirigami.ColumnView.view.currentIndex && Kirigami.ColumnView.view.parent.currentItem === Kirigami.ColumnView.view)
            : (parent && parent instanceof QQC2.StackView
                ? parent.currentItem === root
                : true)

    /**
     * overlay: Item
     * an item which stays on top of every other item in the page,
     * if you want to make sure some elements are completely in a
     * layer on top of the whole content, parent items to this one.
     * It's a "local" version of ApplicationWindow's overlay
     * @since 2.5
     */
    readonly property alias overlay: overlayItem

    /**
     * icon: variant
     *
     * The icon that represents this page.
     */
    property ActionIconGroup icon: ActionIconGroup {}

    /**
     * needsAttention: bool
     *
     * Whether this page needs user attention.
     */
    property bool needsAttention

    /**
     * progress: real
     *
     * Progress of a task this page is doing. Set to undefined to indicate
     * that there are no ongoing tasks.
     */
    property var progress: undefined

    /**
     * titleDelegate: Component
     * The delegate which will be used to draw the page title. It can be customized to put any kind of Item in there.
     * @since 2.7
     */
    property Component titleDelegate: Kirigami.Heading {
        id: title
        level: 1
        Layout.fillWidth: true
        Layout.maximumWidth: implicitWidth + 1 // The +1 is to make sure we do not trigger eliding at max width
        Layout.minimumWidth: 0
        opacity: root.isCurrentPage ? 1 : 0.4
        maximumLineCount: 1
        elide: Text.ElideRight
        text: root.title
    }

    /**
     * emitted When the application requests a Back action
     * For instance a global "back" shortcut or the Android
     * Back button has been pressed.
     * The page can manage the back event by itself,
     * and if it set event.accepted = true, it will stop the main
     * application to manage the back event.
     */
    signal backRequested(var event);


    // Look for sheets and cose them
    //FIXME: port Sheets to Popup?
    onBackRequested: {
        for(var i in root.resources) {
            var item = root.resources[i];
            if (item.hasOwnProperty("close") && item.hasOwnProperty("sheetOpen") && item.sheetOpen) {
                item.close()
                event.accepted = true;
                return;
            }
        }
    }

    /**
     * globalToolBarItem: Item
     * The item used as global toolbar for the page
     * present only if we are in a PageRow as a page or as a layer,
     * and the style is either Titles or ToolBar
     * @since 2.5
     */
    readonly property Item globalToolBarItem: globalToolBar.item

    /**
     * The style for the automatically generated global toolbar: by default the Page toolbar is the one set globally in the PageRow in its globalToolBar.style property.
     * A single page can override the application toolbar style for itself.
     * It is discouraged to use this, except very specific exceptions, like a chat
     * application which can't have controls on the bottom except the text field.
     */
    property int globalToolBarStyle: {
        if (globalToolBar.row && !globalToolBar.stack) {
            return globalToolBar.row.globalToolBar.actualStyle;
        } else if (globalToolBar.stack) {
            return Kirigami.Settings.isMobile ? Kirigami.ApplicationHeaderStyle.Titles : Kirigami.ApplicationHeaderStyle.ToolBar;
        } else {
            return Kirigami.ApplicationHeaderStyle.None;
        }
    }

    //NOTE: contentItem will be created if not existing (and contentChildren of Page would become its children) This with anchors enforces the geometry we want, where globalToolBar is a super-header, on top of header
    contentItem: Item {
        anchors {
            top: root.header
                    ? root.header.bottom
                    : (globalToolBar.visible ? globalToolBar.bottom : parent.top)
            topMargin: root.topPadding + root.spacing
            bottom: root.footer ? root.footer.top : parent.bottom
            bottomMargin: root.bottomPadding + root.spacing
        }
    }

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
    }

    implicitHeight: (header ? header.implicitHeight : 0) + (footer ? footer.implicitHeight : 0) + contentItem.implicitHeight + topPadding + bottomPadding
    implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding

    //FIXME: on material the shadow would bleed over
    clip: root.header != null;

    onHeaderChanged: {
        if (header) {
            header.anchors.top = Qt.binding(function() {return globalToolBar.visible ? globalToolBar.bottom : root.top});
        }
    }

    Component.onCompleted: {
        headerChanged();
        parentChanged(root.parent);
    }
    onParentChanged: {
        if (!parent) {
            return;
        }
        globalToolBar.stack = null;
        globalToolBar.row = null;

        if (root.Kirigami.ColumnView.view) {
            globalToolBar.row = root.Kirigami.ColumnView.view.__pageRow;
        }
        if (root.T2.StackView.view) {
            globalToolBar.stack = root.T2.StackView.view;
            globalToolBar.row = root.T2.StackView.view ? root.T2.StackView.view.parent : null;
        }
        if (globalToolBar.row) {
            root.globalToolBarStyleChanged.connect(globalToolBar.syncSource);
            globalToolBar.syncSource();
        }
    }

    //in data in order for them to not be considered for contentItem, contentChildren, contentData
    data: [
        PageActionPropertyGroup {
            id: actionsGroup
        },

        Item {
            id: overlayItem
            parent: root
            z: 9997
            anchors {
                fill: parent
                topMargin: globalToolBar.height
            }
        },
        //global top toolbar if we are in a PageRow (in the row or as a layer)
        Loader {
            id: globalToolBar
            z: 9999
            height: item ? item.implicitHeight : 0
            anchors {
                left:  parent.left
                right: parent.right
                top: parent.top
            }
            property Kirigami.PageRow row
            property T2.StackView stack

            visible: active
            active: (row || stack) && (root.globalToolBarStyle === Kirigami.ApplicationHeaderStyle.ToolBar || root.globalToolBarStyle == Kirigami.ApplicationHeaderStyle.Titles)

            function syncSource() {
                if (row && active) {
                    setSource(Qt.resolvedUrl(root.globalToolBarStyle === Kirigami.ApplicationHeaderStyle.ToolBar ? "private/globaltoolbar/ToolBarPageHeader.qml" : "private/globaltoolbar/TitlesPageHeader.qml"),
                    //TODO: find container reliably, remove assumption
                    {"pageRow": Qt.binding(function() {return row}),
                    "page": root,
                    "current": Qt.binding(function() {return stack || row.currentIndex === root.Kirigami.ColumnView.level})});
                }
            }
        },
        //bottom action buttons
        Loader {
            id: actionButtons
            z: 9999
            parent: root
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            //if the page doesn't inherit, assume it has custom colors we want to use them here too
            Kirigami.Theme.inherit: !root.Kirigami.Theme.inherit
            Kirigami.Theme.colorSet: Kirigami.Theme.Button

            //It should be T2.Page, Qt 5.7 doesn't like it
            property Item page: root
            height: item ? item.implicitHeight : 0
            active: typeof applicationWindow !== "undefined" && (!globalToolBar.row || root.globalToolBarStyle !== Kirigami.ApplicationHeaderStyle.ToolBar) &&
                    root.actions && (root.actions.main || root.actions.left || root.actions.right || root.actions.contextualActions.length) &&
                //Legacy
                    (typeof applicationWindow === "undefined" ||
                    (!applicationWindow().header || applicationWindow().header.toString().indexOf("ToolBarApplicationHeader") === -1) &&
                    (!applicationWindow().footer || applicationWindow().footer.toString().indexOf("ToolBarApplicationHeader") === -1))
            source: Qt.resolvedUrl("./private/ActionButton.qml")
        }
    ]

    Layout.fillWidth: true
}
