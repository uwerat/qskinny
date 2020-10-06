/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.5
import QtQuick.Controls 2.0 as QQC2
import QtQuick.Layouts 1.2
import "private"
import org.kde.kirigami 2.4


/**
 * An item that can be used as a title for the application.
 * Scrolling the main page will make it taller or shorter (trough the point of going away)
 * It's a behavior similar to the typical mobile web browser addressbar
 * the minimum, preferred and maximum heights of the item can be controlled with
 * * minimumHeight: default is 0, i.e. hidden
 * * preferredHeight: default is Units.gridUnit * 1.6
 * * maximumHeight: default is Units.gridUnit * 3
 *
 * To achieve a titlebar that stays completely fixed just set the 3 sizes as the same
 */
AbstractApplicationHeader {
    id: header

    /**
     * headerStyle: int
     * The way the separator between pages should be drawn in the header.
     * Allowed values are:
     * * Breadcrumb: the pages are hierarchical and the separator will look like a >
     * * TabBar: the pages are intended to behave like tabbar pages
     *    and the separator will look limke a dot.
     *
     * When the header is in wide screen mode, no separator will be drawn.
     */
    property int headerStyle: ApplicationHeaderStyle.Auto

    /**
     * backButtonEnabled: bool
     * if true, there will be a back button present that will make the pagerow scroll back when clicked
     */
    property bool backButtonEnabled: (!titleList.isTabBar && (!Settings.isMobile || Qt.platform.os == "ios"))

    onBackButtonEnabledChanged: {
        if (backButtonEnabled && !titleList.backButton) {
            var component = Qt.createComponent(Qt.resolvedUrl("private/BackButton.qml"));
            titleList.backButton = component.createObject(navButtons);
            component = Qt.createComponent(Qt.resolvedUrl("private/ForwardButton.qml"));
            titleList.forwardButton = component.createObject(navButtons, {"headerFlickable": titleList});
        } else if (titleList.backButton) {
            titleList.backButton.destroy();
            titleList.forwardButton.destroy();
        }
    }
    property Component pageDelegate: Component {
        Row {
            height: parent.height

            spacing: Units.smallSpacing

            x: Units.smallSpacing

            Icon {
                //in tabbar mode this is just a spacer
                visible: !titleList.wideMode && ((typeof(modelData) != "undefined" && modelData > 0) || titleList.internalHeaderStyle == ApplicationHeaderStyle.TabBar)
                anchors.verticalCenter: parent.verticalCenter
                height: Units.iconSizes.small
                width: height
                selected: header.background && header.background.color && header.background.color === Theme.highlightColor
                source: titleList.isTabBar ? "" : (LayoutMirroring.enabled ? "go-next-symbolic-rtl" : "go-next-symbolic")
            }

            Heading {
                id: title
                width: Math.min(parent.width, Math.min(titleList.width, implicitWidth)) + Units.smallSpacing
                anchors.verticalCenter: parent.verticalCenter
                opacity: current ? 1 : 0.4
                //Scaling animate NativeRendering is too slow
                renderType: Text.QtRendering
                color: header.background && header.background.color && header.background.color === Theme.highlightColor ? Theme.highlightedTextColor : Theme.textColor
                elide: Text.ElideRight
                text: page ? page.title : ""
                font.pointSize: -1
                font.pixelSize: Math.max(1, titleList.height * 0.7)
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.NoWrap
                Rectangle {
                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                    }
                    height: Units.smallSpacing
                    color: title.color
                    opacity: 0.6
                    visible: titleList.isTabBar && current
                }
            }
        }
    }

    Component.onCompleted: print("Warning: ApplicationHeader is deprecated, remove and use the automatic internal toolbar instead.")

    Rectangle {
        anchors {
            right: titleList.left
            verticalCenter: parent.verticalCenter
        }
        visible: titleList.x > 0 && !titleList.atXBeginning
        height: parent.height * 0.7
        color: Theme.highlightedTextColor
        width: Math.ceil(Units.smallSpacing / 6)
        opacity: 0.4
    }

    QQC2.StackView {
        id: stack
        anchors {
            fill: parent
            leftMargin: navButtons.width
            rightMargin: __appWindow.contextDrawer && __appWindow.contextDrawer.handleVisible && __appWindow.contextDrawer.handle && __appWindow.contextDrawer.handle.y == 0 ? __appWindow.contextDrawer.handle.width : 0
        }
        initialItem: titleList

        popEnter: Transition {
            YAnimator {
                from: -height
                to: 0
                duration: Units.longDuration
                easing.type: Easing.OutCubic
            }
        }
        popExit: Transition {
            YAnimator {
                from: 0
                to: height
                duration: Units.longDuration
                easing.type: Easing.OutCubic
            }
        }

        pushEnter: Transition {
            YAnimator {
                from: height
                to: 0
                duration: Units.longDuration
                easing.type: Easing.OutCubic
            }
        }

        pushExit: Transition {
            YAnimator {
                from: 0
                to: -height
                duration: Units.longDuration
                easing.type: Easing.OutCubic
            }
        }

        replaceEnter: Transition {
            YAnimator {
                from: height
                to: 0
                duration: Units.longDuration
                easing.type: Easing.OutCubic
            }
        }

        replaceExit: Transition {
            YAnimator {
                from: 0
                to: -height
                duration: Units.longDuration
                easing.type: Easing.OutCubic
            }
        }
    }
    Separator {
        id: separator
        height: parent.height * 0.6
        visible: navButtons.width > 0
        anchors {
            verticalCenter: parent.verticalCenter
            left: navButtons.right
        }
    }
    Separator {
        height: parent.height * 0.6
        visible: stack.anchors.rightMargin > 0
        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: stack.anchors.rightMargin
        }
    }
    Repeater {
        model: pageRow.layers.depth -1
        delegate: Loader {
            sourceComponent: header.pageDelegate
            readonly property Page page: pageRow.layers.get(modelData+1)
            readonly property bool current: true;
            Component.onCompleted: stack.push(this)
            Component.onDestruction: stack.pop()
        }
    }

    Row {
        id: navButtons
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            topMargin: Units.smallSpacing
            bottomMargin: Units.smallSpacing
        }
        Item {
            height: parent.height
            width: (applicationWindow().header && applicationWindow().header.toString().indexOf("ToolBarApplicationHeader") === 0) && __appWindow.globalDrawer && __appWindow.globalDrawer.handleVisible && __appWindow.globalDrawer.handle && __appWindow.globalDrawer.handle.y === 0 ? __appWindow.globalDrawer.handle.width : 0
        }
    }

    Flickable {
        id: titleList
        readonly property bool wideMode: pageRow.hasOwnProperty("wideMode") ? pageRow.wideMode : __appWindow.wideScreen
        property int internalHeaderStyle: header.headerStyle == ApplicationHeaderStyle.Auto ? (titleList.wideMode ? ApplicationHeaderStyle.Titles : ApplicationHeaderStyle.Breadcrumb) : header.headerStyle
        //if scrolling the titlebar should scroll also the pages and vice versa
        property bool scrollingLocked: (header.headerStyle == ApplicationHeaderStyle.Titles || titleList.wideMode)
        //uses this to have less strings comparisons
        property bool scrollMutex
        property bool isTabBar: header.headerStyle == ApplicationHeaderStyle.TabBar

        property Item backButton
        property Item forwardButton
        clip: true


        boundsBehavior: Flickable.StopAtBounds
        readonly property alias model: mainRepeater.model
        contentWidth: contentItem.width
        contentHeight: height

        readonly property int currentIndex: pageRow && pageRow.currentIndex !== undefined ? pageRow.currentIndex : 0
        readonly property int count: mainRepeater.count

        function gotoIndex(idx) {
            //don't actually scroll in widescreen mode
            if (titleList.wideMode || contentItem.children.length < 2) {
                return;
            }
            listScrollAnim.running = false
            var pos = titleList.contentX;
            var destPos;
            titleList.contentX = Math.max((contentItem.children[idx].x + contentItem.children[idx].width) - titleList.width, Math.min(titleList.contentX, contentItem.children[idx].x));
            destPos = titleList.contentX;
            listScrollAnim.from = pos;
            listScrollAnim.to = destPos;
            listScrollAnim.running = true;
        }

        NumberAnimation {
            id: listScrollAnim
            target: titleList
            property: "contentX"
            duration: Units.longDuration
            easing.type: Easing.InOutQuad
        }
        Timer {
            id: contentXSyncTimer
            interval: 0
            onTriggered: {
                titleList.contentX = pageRow.contentItem.contentX - pageRow.contentItem.originX + titleList.originX;
            }
        }
        onCountChanged: contentXSyncTimer.restart();
        onCurrentIndexChanged: gotoIndex(currentIndex);
        onModelChanged: gotoIndex(currentIndex);
        onContentWidthChanged: gotoIndex(currentIndex);

        onContentXChanged: {
            if (movingHorizontally && !titleList.scrollMutex && titleList.scrollingLocked && !pageRow.contentItem.moving) {
                titleList.scrollMutex = true;
                pageRow.contentItem.contentX = titleList.contentX - titleList.originX + pageRow.contentItem.originX;
                titleList.scrollMutex = false;
            }
        }
        onHeightChanged: {
            titleList.returnToBounds()
        }
        onMovementEnded: {
            if (titleList.scrollingLocked) {
                //this will trigger snap as well
                pageRow.contentItem.flick(0,0);
            }
        }
        onFlickEnded: movementEnded();

        NumberAnimation {
            id: scrollTopAnimation
            target: pageRow.currentItem && pageRow.currentItem.flickable ? pageRow.currentItem.flickable : null
            property: "contentY"
            to: 0
            duration: Units.longDuration
            easing.type: Easing.InOutQuad
        }

        Row {
            id: contentItem
            spacing: 0
            Repeater {
                id: mainRepeater
                model: pageRow.depth
                delegate: MouseArea {
                    id: delegate
                    readonly property int currentIndex: index
                    readonly property var currentModelData: modelData
                    clip: true

                    width: {
                        //more columns shown?
                        if (titleList.scrollingLocked && delegateLoader.page) {
                            return delegateLoader.page.width - (index == 0 ? navButtons.width : 0) - (index == pageRow.depth-1  ? stack.anchors.rightMargin : 0);
                        } else {
                            return Math.min(titleList.width, delegateLoader.implicitWidth + Units.smallSpacing);
                        }
                    }

                    height: titleList.height
                    onClicked: {
                        if (pageRow.currentIndex === modelData) {
                            //scroll up if current otherwise make current
                            if (!pageRow.currentItem.flickable) {
                                return;
                            }
                            if (pageRow.currentItem.flickable.contentY > -__appWindow.header.height) {
                                scrollTopAnimation.to = -pageRow.currentItem.flickable.topMargin;
                                scrollTopAnimation.running = true;
                            }

                        } else {
                            pageRow.currentIndex = modelData;
                        }
                    }

                    Loader {
                        id: delegateLoader
                        height: parent.height
                        x: titleList.wideMode || headerStyle == ApplicationHeaderStyle.Titles ? (Math.min(delegate.width - implicitWidth, Math.max(0, titleList.contentX - delegate.x))) : 0
                        width: parent.width - x

                        Connections {
                            target: delegateLoader.page
                            Component.onDestruction: delegateLoader.sourceComponent = null
                        }

                        sourceComponent: header.pageDelegate

                        readonly property Page page: pageRow.get(modelData)
                        //NOTE: why not use ListViewCurrentIndex? because listview itself resets
                        //currentIndex in some situations (since here we are using an int as a model,
                        //even more often) so the property binding gets broken
                        readonly property bool current: pageRow.currentIndex === index
                        readonly property int index: parent.currentIndex
                        readonly property var modelData: parent.currentModelData
                    }
                }
            }
        }
        Connections {
            target: titleList.scrollingLocked ? pageRow.contentItem : null
            onContentXChanged: {
                if (!titleList.dragging && !titleList.movingHorizontally && !titleList.scrollMutex) {
                    titleList.contentX = pageRow.contentItem.contentX - pageRow.contentItem.originX + titleList.originX;
                }
            }
        }
    }
}
