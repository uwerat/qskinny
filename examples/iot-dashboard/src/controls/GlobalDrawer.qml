/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Templates 2.0 as T2
import QtQuick.Controls 2.2 as QQC2
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.13

import "private"
import "templates/private"

/**
 * A drawer specialization intended for the global actions of the application
 * valid regardless of the application state (think about the menubar
 * of a desktop application).
 *
 * Example usage:
 * @code
 * import org.kde.kirigami 2.4 as Kirigami
 *
 * Kirigami.ApplicationWindow {
 *  [...]
 *     globalDrawer: Kirigami.GlobalDrawer {
 *         actions: [
 *            Kirigami.Action {
 *                text: "View"
 *                iconName: "view-list-icons"
 *                Kirigami.Action {
 *                        text: "action 1"
 *                }
 *                Kirigami.Action {
 *                        text: "action 2"
 *                }
 *                Kirigami.Action {
 *                        text: "action 3"
 *                }
 *            },
 *            Kirigami.Action {
 *                text: "Sync"
 *                iconName: "folder-sync"
 *            }
 *         ]
 *     }
 *  [...]
 * }
 * @endcode
 *
 */
OverlayDrawer {
    id: root
    edge: Qt.application.layoutDirection == Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
    handleClosedIcon.source: null
    handleOpenIcon.source: null
    handleVisible: (modal || !drawerOpen) && (typeof(applicationWindow)===typeof(Function) && applicationWindow() ? applicationWindow().controlsVisible : true) && (!isMenu || Settings.isMobile)

    enabled: !isMenu || Settings.isMobile

    /**
     * title: string
     * A title to be displayed on top of the drawer
     */
    property alias title: bannerImage.title

    /**
     * icon: var
     * An icon to be displayed alongside the title.
     * It can be a QIcon, a fdo-compatible icon name, or any url understood by Image
     */
    property alias titleIcon: bannerImage.titleIcon

    /**
     * bannerImageSource: string
     * An image to be used as background for the title and icon for
     * a decorative purpose.
     * It accepts any url format supported by Image
     */
    property alias bannerImageSource: bannerImage.source

    /**
     * actions: list<Action>
     * The list of actions can be nested having a tree structure.
     * A tree depth bigger than 2 is discouraged.
     *
     * Example usage:
     * @code
     * import org.kde.kirigami 2.4 as Kirigami
     *
     * Kirigami.ApplicationWindow {
     *  [...]
     *     globalDrawer: Kirigami.GlobalDrawer {
     *         actions: [
     *            Kirigami.Action {
     *                text: "View"
     *                iconName: "view-list-icons"
     *                Kirigami.Action {
     *                        text: "action 1"
     *                }
     *                Kirigami.Action {
     *                        text: "action 2"
     *                }
     *                Kirigami.Action {
     *                        text: "action 3"
     *                }
     *            },
     *            Kirigami.Action {
     *                text: "Sync"
     *                iconName: "folder-sync"
     *            }
     *         ]
     *     }
     *  [...]
     * }
     * @endcode
     */
    property list<QtObject> actions

    /**
     * header: Item
     * an item that will stay on top of the drawer,
     * and if the drawer contents can be scrolled,
     * this item will stay still and won't scroll.
     * Indended mainly for toolbars
     * @since 2.12
     */
    property Item header

    /**
     * bannerVisible: bool
     * if true the banner area, which can contain an image,
     * an icon and a title will be visible.
     * By default the banner will be visible only on mobile platforms
     * @since 2.12
     */
    property bool bannerVisible: Settings.isMobile
    /**
     * content: list<Item> default property
     * Any random Item can be instantiated inside the drawer and
     * will be displayed underneath the actions list.
     *
     * Example usage:
     * @code
     * import org.kde.kirigami 2.4 as Kirigami
     *
     * Kirigami.ApplicationWindow {
     *  [...]
     *     globalDrawer: Kirigami.GlobalDrawer {
     *         actions: [...]
     *         Button {
     *             text: "Button"
     *             onClicked: //do stuff
     *         }
     *     }
     *  [...]
     * }
     * @endcode
     */
    default property alias content: mainContent.data

    /**
     * topContent: list<Item> default property
     * Items that will be instantiated inside the drawer and
     * will be displayed on top of the actions list.
     *
     * Example usage:
     * @code
     * import org.kde.kirigami 2.4 as Kirigami
     *
     * Kirigami.ApplicationWindow {
     *  [...]
     *     globalDrawer: Kirigami.GlobalDrawer {
     *         actions: [...]
     *         topContent: [Button {
     *             text: "Button"
     *             onClicked: //do stuff
     *         }]
     *     }
     *  [...]
     * }
     * @endcode
     */
    property alias topContent: topContent.data

    /**
     * showContentWhenCollapsed: bool
     * If true, when the drawer is collapsed as a sidebar, the content items
     * at the bottom will be hidden (default false).
     * If you want to keep some items visible and some invisible, set this to
     * false and control the visibility/opacity of individual items,
     * binded to the collapsed property
     * @since 2.5
     */
    property bool showContentWhenCollapsed: false

    /**
     * showTopContentWhenCollapsed: bool
     * If true, when the drawer is collapsed as a sidebar, the top content items
     * at the top will be hidden (default false).
     * If you want to keep some items visible and some invisible, set this to
     * false and control the visibility/opacity of individual items,
     * binded to the collapsed property
     * @since 2.5
     */
    property bool showTopContentWhenCollapsed: false

    //TODO
    property bool showHeaderWhenCollapsed: false

    /**
     * resetMenuOnTriggered: bool
     *
     * On the actions menu, whenever a leaf action is triggered, the menu
     * will reset to its parent.
     */
    property bool resetMenuOnTriggered: true

    /**
     * currentSubMenu: Action
     *
     * Points to the action acting as a submenu
     */
    readonly property Action currentSubMenu: stackView.currentItem ? stackView.currentItem.current: null

    /**
     * isMenu: bool
     * When true the global drawer becomes a menu on the desktop. Defauls to false.
     * @since 2.11
     */
    property bool isMenu: false

    /**
     * Notifies that the banner has been clicked
     */
    signal bannerClicked()

    /**
     * When the sidebar is collapsible, this controls the visibility of
     * the collapse button
     * @since 2.12
     */
    property bool collapseButtonVisible: true

    /**
     * Reverts the menu back to its initial state
     */
    function resetMenu() {
        stackView.pop(stackView.get(0, T2.StackView.DontLoad));
        if (root.modal) {
            root.drawerOpen = false;
        }
    }

    //rightPadding: !Settings.isMobile && mainFlickable.contentHeight > mainFlickable.height ? Units.gridUnit : Units.smallSpacing

    Theme.colorSet: modal ? Theme.Window : Theme.View

    onHeaderChanged: {
        if (header) {
            header.parent = headerContainer
            header.Layout.fillWidth = true;
        }
    }

    contentItem: QQC2.ScrollView {
        id: scrollView
        //ensure the attached property exists
        Theme.inherit: true
        anchors.fill: parent
        implicitWidth: Math.min (Units.gridUnit * 20, root.parent.width * 0.8)
        QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff
        QQC2.ScrollBar.vertical.anchors {
            top: scrollView.top
            bottom: scrollView.bottom
            topMargin: headerParent.height + headerParent.y
        }

        Flickable {
            id: mainFlickable
            contentWidth: width
            contentHeight: mainColumn.Layout.minimumHeight
            topMargin: headerParent.height

            ColumnLayout {
                id: headerParent
                parent: mainFlickable
                anchors {
                    left: parent.left
                    right: parent.right
                    rightMargin: Math.min(0, -scrollView.width + mainFlickable.width)
                }
                spacing: 0
                y: bannerImage.visible ? Math.max(headerContainer.height, -mainFlickable.contentY) - height : 0

                Layout.fillWidth: true
                //visible: !bannerImage.empty || root.collapsible

                BannerImage {
                    id: bannerImage


                    visible: !bannerImage.empty && opacity > 0 && root.bannerVisible
                    opacity: !root.collapsed
                    fillMode: Image.PreserveAspectCrop

                    Behavior on opacity {
                        OpacityAnimator {
                            duration: Units.longDuration
                            easing.type: Easing.InOutQuad
                        }
                    }
                    //leftPadding: root.collapsible ? collapseButton.width + Units.smallSpacing*2 : topPadding
                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.bannerClicked()
                    }
                    EdgeShadow {
                        edge: Qt.BottomEdge
                        visible: bannerImageSource != ""
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.top
                        }
                    }
                }
                RowLayout {
                    id: headerContainer
                    Theme.inherit: false
                    Theme.colorSet: Theme.Window

                    Layout.fillWidth: true
                    visible: contentItem && opacity > 0
                    Layout.preferredHeight: implicitHeight * opacity
                    opacity: !root.collapsed || showHeaderWhenCollapsed
                    Behavior on opacity {
                        //not an animator as is binded
                        NumberAnimation {
                            duration: Units.longDuration
                            easing.type: Easing.InOutQuad
                        }
                    }
                }
            }


            ColumnLayout {
                id: mainColumn
                width: mainFlickable.width
                spacing: 0
                height: Math.max(root.height - headerParent.height, Layout.minimumHeight)

                ColumnLayout {
                    id: topContent
                    spacing: 0
                    Layout.alignment: Qt.AlignHCenter
                    Layout.leftMargin: root.leftPadding
                    Layout.rightMargin: root.rightPadding
                    Layout.bottomMargin: Units.smallSpacing
                    Layout.topMargin: root.topPadding
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredHeight: implicitHeight * opacity
                    //NOTE: why this? just Layout.fillWidth: true doesn't seem sufficient
                    //as items are added only after this column creation
                    Layout.minimumWidth: parent.width - root.leftPadding - root.rightPadding
                    visible: children.length > 0 && childrenRect.height > 0 && opacity > 0
                    opacity: !root.collapsed || showTopContentWhenCollapsed
                    Behavior on opacity {
                        //not an animator as is binded
                        NumberAnimation {
                            duration: Units.longDuration
                            easing.type: Easing.InOutQuad
                        }
                    }
                }

                T2.StackView {
                    id: stackView
                    clip: true
                    Layout.fillWidth: true
                    Layout.minimumHeight: currentItem ? currentItem.implicitHeight : 0
                    Layout.maximumHeight: Layout.minimumHeight
                    property ActionsMenu openSubMenu
                    initialItem: menuComponent
                    //NOTE: it's important those are NumberAnimation and not XAnimators
                    // as while the animation is running the drawer may close, and
                    //the animator would stop when not drawing see BUG 381576
                    popEnter: Transition {
                        NumberAnimation { property: "x"; from: (stackView.mirrored ? -1 : 1) * -stackView.width; to: 0; duration: 400; easing.type: Easing.OutCubic }
                    }

                    popExit: Transition {
                        NumberAnimation { property: "x"; from: 0; to: (stackView.mirrored ? -1 : 1) * stackView.width; duration: 400; easing.type: Easing.OutCubic }
                    }

                    pushEnter: Transition {
                        NumberAnimation { property: "x"; from: (stackView.mirrored ? -1 : 1) * stackView.width; to: 0; duration: 400; easing.type: Easing.OutCubic }
                    }

                    pushExit: Transition {
                        NumberAnimation { property: "x"; from: 0; to: (stackView.mirrored ? -1 : 1) * -stackView.width; duration: 400; easing.type: Easing.OutCubic }
                    }

                    replaceEnter: Transition {
                        NumberAnimation { property: "x"; from: (stackView.mirrored ? -1 : 1) * stackView.width; to: 0; duration: 400; easing.type: Easing.OutCubic }
                    }

                    replaceExit: Transition {
                        NumberAnimation { property: "x"; from: 0; to: (stackView.mirrored ? -1 : 1) * -stackView.width; duration: 400; easing.type: Easing.OutCubic }
                    }
                }
                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: root.actions.length>0
                    Layout.minimumHeight: Units.smallSpacing
                }

                ColumnLayout {
                    id: mainContent
                    Layout.alignment: Qt.AlignHCenter
                    Layout.leftMargin: root.leftPadding
                    Layout.rightMargin: root.rightPadding
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    //NOTE: why this? just Layout.fillWidth: true doesn't seem sufficient
                    //as items are added only after this column creation
                    Layout.minimumWidth: parent.width - root.leftPadding - root.rightPadding
                    visible: children.length > 0 && (opacity > 0 || mainContentAnimator.running)
                    opacity: !root.collapsed || showContentWhenCollapsed
                    Behavior on opacity {
                        OpacityAnimator {
                            id: mainContentAnimator
                            duration: Units.longDuration
                            easing.type: Easing.InOutQuad
                        }
                    }
                }
                Item {
                    Layout.minimumWidth: Units.smallSpacing
                    Layout.minimumHeight: root.bottomPadding
                }

                Component {
                    id: menuComponent

                    Column {
                        spacing: 0
                        property alias model: actionsRepeater.model
                        property Action current

                        property int level: 0
                        Layout.maximumHeight: Layout.minimumHeight

                        BasicListItem {
                            id: backItem
                            visible: level > 0
                            supportsMouseEvents: true
                            icon: (LayoutMirroring.enabled ? "go-previous-symbolic-rtl" : "go-previous-symbolic")

                            label: MnemonicData.richTextLabel
                            MnemonicData.enabled: backItem.enabled && backItem.visible
                            MnemonicData.controlType: MnemonicData.MenuItem
                            MnemonicData.label: qsTr("Back")

                            separatorVisible: false
                            onClicked: stackView.pop()
                        }
                        Shortcut {
                            sequence: backItem.MnemonicData.sequence
                            onActivated: backItem.clicked()
                        }

                        Repeater {
                            id: actionsRepeater

                            readonly property bool withSections: {
                                for (var i = 0; i < root.actions.length; i++) {
                                    let action = root.actions[i];
                                    if (!(action.hasOwnProperty("expandible") && action.expandible)) {
                                        return false;
                                    }
                                }
                                return true;
                            }

                            model: root.actions
                            delegate: Column {
                                width: parent.width
                                GlobalDrawerActionItem {
                                    id: drawerItem
                                    visible: root.collapsed || !(modelData.hasOwnProperty("expandible") && modelData.expandible) && (modelData.hasOwnProperty("visible") && modelData.visible)
                                    width: parent.width
                                    onCheckedChanged: {
                                        // move every checked item into view
                                        if (checked && topContent.height + backItem.height + (model.index + 1) * height - mainFlickable.contentY > mainFlickable.height) {
                                            mainFlickable.contentY += height
                                        }
                                    }
                                    Theme.colorSet: drawerItem.visible && !root.modal && !root.collapsed && actionsRepeater.withSections ? Theme.Window : parent.Theme.colorSet
                                    backgroundColor: Theme.backgroundColor
                                }
                                Item {
                                    id: headerItem
                                    visible: !root.collapsed && (modelData.hasOwnProperty("expandible") && modelData.expandible && !!modelData.children && modelData.children.length > 0)
                                    height: sectionHeader.implicitHeight
                                    width: parent.width
                                    ListSectionHeader {
                                        id: sectionHeader
                                        anchors.fill: parent
                                        Theme.colorSet: root.modal ? Theme.View : Theme.Window
                                        contentItem: RowLayout {
                                            Icon {
                                                height: header.height
                                                width: height
                                                source: modelData.icon.name || modelData.icon.source
                                            }
                                            Heading {
                                                id: header
                                                level: 3
                                                text: modelData.text
                                            }
                                            Item {
                                                Layout.fillWidth: true
                                            }
                                        }
                                    }
                                }
                                Repeater {
                                    id: __repeater
                                    model: headerItem.visible ? modelData.children : null
                                    delegate: GlobalDrawerActionItem {
                                        width: parent.width
                                        opacity: !root.collapsed
                                        leftPadding: actionsRepeater.withSections && !root.collapsed && !root.modal ? padding * 2 : padding * 4
                                    }
                                }
                                Separator {
                                    visible: __repeater.count > 0
                                    anchors {
                                        left: parent.left
                                        right: parent.right
                                    }
                                }
                            }
                        }
                    }
                }

                QQC2.ToolButton {
                    icon.name: root.collapsed ? "view-right-new" : "view-right-close"
                    Layout.fillWidth: root.collapsed
                    onClicked: root.collapsed = !root.collapsed
                    visible: root.collapsible && root.collapseButtonVisible
                    text: root.collapsed ? "" : qsTr("Close Sidebar")
                }
            }
        }
    }
}

