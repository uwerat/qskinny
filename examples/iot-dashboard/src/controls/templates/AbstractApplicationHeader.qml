/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import "private"
import org.kde.kirigami 2.4
import QtQuick.Controls 2.4 as Controls

/**
 * An item that can be used as a title for the application.
 * Scrolling the main page will make it taller or shorter (trough the point of going away)
 * It's a behavior similar to the typical mobile web browser addressbar
 * the minimum, preferred and maximum heights of the item can be controlled with
 * * minimumHeight: default is 0, i.e. hidden
 * * preferredHeight: default is Units.gridUnit * 1.6
 * * preferredHeight: default is Units.gridUnit * 3
 *
 * To achieve a titlebar that stays completely fixed just set the 3 sizes as the same
 * @inherit QtQuick.Item
 */
Item {
    id: root
    z: 90
    property int minimumHeight: 0
    property int preferredHeight: Math.max(...(Array.from(mainItem.children).map(elm => elm.implicitHeight))) + topPadding + bottomPadding
    property int maximumHeight: Units.gridUnit * 3

    property int position: Controls.ToolBar.Header
    
    property PageRow pageRow: __appWindow ? __appWindow.pageStack: null
    property Page page: pageRow ? pageRow.currentItem : null

    default property alias contentItem: mainItem.data
    readonly property int paintedHeight: headerItem.y + headerItem.height - 1

    property int leftPadding: 0
    property int topPadding: 0
    property int rightPadding: 0
    property int bottomPadding: 0
    property bool separatorVisible: true

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    Theme.inherit: true

    //FIXME: remove
    property QtObject __appWindow: typeof applicationWindow !== "undefined" ? applicationWindow() : null;
    implicitHeight: preferredHeight
    height: Layout.preferredHeight

    /**
     * background: Item
     * This property holds the background item.
     * Note: the background will be automatically sized as the whole control
     */
    property Item background

    onBackgroundChanged: {
        background.z = -1;
        background.parent = headerItem;
        background.anchors.fill = headerItem;
    }

    Component.onCompleted: AppHeaderSizeGroup.items.push(this)

    onMinimumHeightChanged: implicitHeight = preferredHeight;
    onPreferredHeightChanged: implicitHeight = preferredHeight;

    opacity: height > 0 ? 1 : 0

    NumberAnimation {
        id: heightAnim
        target: root
        property: "implicitHeight"
        duration: Units.longDuration
        easing.type: Easing.InOutQuad
    }
    Connections {
        target: __appWindow
        onControlsVisibleChanged: {
            heightAnim.from = root.implicitHeight
            heightAnim.to = __appWindow.controlsVisible ? root.preferredHeight : 0;
            heightAnim.restart();
        }
    }

    Item {
        id: headerItem
        property real computedRootHeight: root.preferredHeight
        anchors {
            left: parent.left
            right: parent.right
            bottom: root.position === Controls.ToolBar.Header ? parent.bottom : undefined
            top: root.position === Controls.ToolBar.Footer ? parent.top : undefined
        }

        height: __appWindow && __appWindow.reachableMode && __appWindow.reachableModeEnabled ? root.maximumHeight : (root.minimumHeight > 0 ? Math.max(root.height, root.minimumHeight) : root.preferredHeight)

        //FIXME: see FIXME below
        Connections {
            target: root.page ? root.page.globalToolBarItem : null
            enabled: headerSlideConnection.passive && target
            onImplicitHeightChanged: root.implicitHeight = root.page.globalToolBarItem.implicitHeight
        }

        Connections {
            id: headerSlideConnection
            target: root.page ? root.page.flickable : null
            enabled: !passive
            property real oldContentY
            property bool updatingContentY: false

            //FIXME HACK: if we are in global mode, meaning if we are the toolbar showing the global breadcrumb (but the pages are showing their own toolbar), not to try to mess with page contentY.
            //A better solution is needed
            readonly property bool passive: root.pageRow && parent.parent == root.pageRow && root.pageRow.globalToolBar.actualStyle !== ApplicationHeaderStyle.TabBar && root.pageRow.globalToolBar.actualStyle != ApplicationHeaderStyle.Breadcrumb

            onContentYChanged: {
                if(root.page && root.page.flickable && root.page.flickable.contentHeight < root.page.height) {
                    return
                }                

                if ((root.pageRow ? root.pageRow.wideMode : (__appWindow && __appWindow.wideScreen)) || !Settings.isMobile || root.page.flickable.atYBeginning) {
                    root.implicitHeight = root.preferredHeight;
                } else {
                    var oldHeight = root.implicitHeight;

                    root.implicitHeight = Math.max(root.minimumHeight,
                                            Math.min(root.preferredHeight,
                                                 root.implicitHeight + oldContentY - root.page.flickable.contentY));

                    //if the implicitHeight is changed, use that to simulate scroll
                    if (oldHeight === implicitHeight) {
                        oldContentY = root.page.flickable.contentY;
                    }
                }
            }

            onMovementEnded: {
                if ((root.pageRow ? root.pageRow.wideMode : (__appWindow && __appWindow.wideScreen)) || !Settings.isMobile) {
                    return;
                }
                if (root.height > root.minimumHeight + (root.preferredHeight - root.minimumHeight)/2 ) {
                    heightAnim.to = root.preferredHeight;
                } else {
                    heightAnim.to = root.minimumHeight;
                }
                heightAnim.from = root.implicitHeight
                heightAnim.restart();
            }
        }
        Connections {
            target: pageRow
            onCurrentItemChanged: {
                if (!root.page) {
                    return;
                }
                if (root.page.flickable) {
                    headerSlideConnection.oldContentY = root.page.flickable.contentY;
                } else {
                    headerSlideConnection.oldContentY = 0;
                }

                root.implicitHeight = root.preferredHeight;
            }
        }

        Item {
            id: mainItem
            clip: childrenRect.width > width
            anchors {
                fill: parent
                leftMargin: root.leftPadding
                topMargin: root.topPadding
                rightMargin: root.rightPadding
                bottomMargin: root.bottomPadding
            }
        }
    }
}

