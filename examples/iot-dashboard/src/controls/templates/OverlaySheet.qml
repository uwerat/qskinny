/*
 *  SPDX-FileCopyrightText: 2016-2020 Marco Martin <notmart@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */


import QtQuick 2.11
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import org.kde.kirigami 2.14
import QtGraphicalEffects 1.0
import QtQuick.Templates 2.0 as T2
import "private"
import "../private"

/**
 * An overlay sheet that covers the current Page content.
 * Its contents can be scrolled up or down, scrolling all the way up or
 * all the way down, dismisses it.
 * Use this for big, modal dialogs or information display, that can't be
 * logically done as a new separate Page, even if potentially
 * are taller than the screen space.
 * @inherits: QtQuick.QtObject
 */
QtObject {
    id: root

    Theme.colorSet: Theme.View
    Theme.inherit: false

    /**
     * contentItem: Item
     * This property holds the visual content item.
     *
     * Note: The content item is automatically resized inside the
     * padding of the control.
     * Conversely, the Sheet will be sized based on the size hints
     * of the contentItem, so if you need a custom size sheet,
     * redefine contentWidth and contentHeight of your contentItem
     */
    default property Item contentItem

    /**
     * sheetOpen: bool
     * If true the sheet is open showing the contents of the OverlaySheet
     * component.
     */
    property bool sheetOpen

    /**
     * leftPadding: int
     * default contents padding at left
     */
    property int leftPadding: Units.largeSpacing

    /**
     * topPadding: int
     * default contents padding at top
     */
    property int topPadding: Units.largeSpacing

    /**
     * rightPadding: int
     * default contents padding at right
     */
    property int rightPadding: Units.largeSpacing

    /**
     * bottomPadding: int
     * default contents padding at bottom
     */
    property int bottomPadding: Units.largeSpacing

    /**
     * leftInset: real
     * padding that gets applied to both the content *and* the background
     * @since 2.12
     */
    property real leftInset: 0

    /**
     * topInset: real
     * padding that gets applied to both the content *and* the background
     * @since 2.12
     */
    property real topInset: 0

    /**
     * rightInset: real
     * padding that gets applied to both the content *and* the background
     * @since 2.12
     */
    property real rightInset: 0

    /**
     * bottomInset: real
     * padding that gets applied to both the content *and* the background
     * @since 2.12
     */
    property real bottomInset: 0

    /**
     * header: Item
     * an optional item which will be used as the sheet's header,
     * always kept on screen
     * @since 5.43
     */
    property Item header

    /**
     * header: Item
     * an optional item which will be used as the sheet's footer,
     * always kept on screen
     * @since 5.43
     */
    property Item footer
    /**
     * background: Item
     * This property holds the background item.
     *
     * Note: If the background item has no explicit size specified,
     * it automatically follows the control's size.
     * In most cases, there is no need to specify width or
     * height for a background item.
     */
    property Item background

    /**
     * showCloseButton: bool
     * whether to show the close button in the top-right corner
     * @since 5.44
     */
    property bool showCloseButton: !Settings.isMobile

    property Item parent


    function open() {
        openAnimation.running = true;
        root.sheetOpen = true;
        mainItem.visible = true;
    }

    function close() {
        if (root.sheetOpen) {
            closeAnimation.running = true;
        }
    }

    onBackgroundChanged: {
        background.parent = contentLayout.parent;
        background.anchors.fill = contentLayout;
        background.anchors.margins = -1
        background.z = -1;
    }
    onContentItemChanged: {
        if (contentItem instanceof Flickable) {
            scrollView.flickableItem = contentItem;
            contentItem.parent = scrollView;
            contentItem.anchors.fill = scrollView;
            scrollView.contentItem = contentItem;
        } else {
            contentItem.parent = contentItemParent;
            scrollView.contentItem = flickableContents;
            contentItem.anchors.left = contentItemParent.left;
            contentItem.anchors.right = contentItemParent.right;
        }
        scrollView.flickableItem.interactive = false;
        scrollView.flickableItem.flickableDirection = Flickable.VerticalFlick;
    }
    onSheetOpenChanged: {
        if (sheetOpen) {
            open();
        } else {
            closeAnimation.running = true;
            Qt.inputMethod.hide();
        }
    }
    onHeaderChanged: {
        header.parent = headerParent;
        header.anchors.fill = headerParent;

        //TODO: special case for actual ListViews
    }
    onFooterChanged: {
        footer.parent = footerParent;
        footer.anchors.fill = footerParent;
    }

    Component.onCompleted: {
        if (!root.parent && typeof applicationWindow !== "undefined") {
            root.parent = applicationWindow().overlay
        }
    }

    readonly property Item rootItem: MouseArea {
        id: mainItem
        Theme.colorSet: root.Theme.colorSet
        Theme.inherit: root.Theme.inherit
        z: 101
        //we want to be over any possible OverlayDrawers, including handles
        parent: {
            if (root.parent && root.parent.ColumnView.view && (root.parent.ColumnView.view == root.parent || root.parent.ColumnView.view == root.parent.parent)) {
                return root.parent.ColumnView.view.parent;
            } else if (root.parent && root.parent.overlay) {
                root.parent.overlay;
            } else {
                return root.parent;
            }
        }

        anchors.fill: parent

        visible: false
        drag.filterChildren: true
        hoverEnabled: true
        clip: true

        onClicked: {
            var pos = mapToItem(contentLayout, mouse.x, mouse.y);
            if (!contentLayout.contains(pos)) {
                root.close();
            }
        }

        readonly property int contentItemPreferredWidth: root.contentItem.Layout.preferredWidth > 0 ? root.contentItem.Layout.preferredWidth : root.contentItem.implicitWidth

        readonly property int contentItemMaximumWidth: width > Units.gridUnit * 30 ? width * 0.95 : width

        onHeightChanged: {
            var focusItem;

            focusItem = Window.activeFocusItem;

            if (!focusItem) {
                return;
            }

            //NOTE: there is no function to know if an item is descended from another,
            //so we have to walk the parent hierarchy by hand
            var isDescendent = false;
            var candidate = focusItem.parent;
            while (candidate) {
                if (candidate === root) {
                    isDescendent = true;
                    break;
                }
                candidate = candidate.parent;
            }
            if (!isDescendent) {
                return;
            }

            var cursorY = 0;
            if (focusItem.cursorPosition !== undefined) {
                cursorY = focusItem.positionToRectangle(focusItem.cursorPosition).y;
            }


            var pos = focusItem.mapToItem(flickableContents, 0, cursorY - Units.gridUnit*3);
            //focused item already visible? add some margin for the space of the action buttons
            if (pos.y >= scrollView.flickableItem.contentY && pos.y <= scrollView.flickableItem.contentY + scrollView.flickableItem.height - Units.gridUnit * 8) {
                return;
            }
            scrollView.flickableItem.contentY = pos.y;
        }

        ParallelAnimation {
            id: openAnimation
            property int margins: Units.gridUnit * 5
            NumberAnimation {
                target: outerFlickable
                properties: "contentY"
                from: -outerFlickable.height
                to: Math.max(0, outerFlickable.height - outerFlickable.contentHeight + headerItem.height + footerItem.height) + outerFlickable.topMargin/2 - contentLayout.height/2
                duration: Units.longDuration
                easing.type: Easing.OutQuad
            }
            OpacityAnimator {
                target: mainItem
                from: 0
                to: 1
                duration: Units.longDuration
                easing.type: Easing.InQuad
            }
        }

        NumberAnimation {
            id: resetAnimation
            target: outerFlickable
            properties: "contentY"
            from: outerFlickable.contentY
            to: outerFlickable.visibleArea.yPosition < (1 - outerFlickable.visibleArea.heightRatio)/2 || scrollView.flickableItem.contentHeight < outerFlickable.height
                ? Math.max(0, outerFlickable.height - outerFlickable.contentHeight + headerItem.height + footerItem.height) + outerFlickable.topMargin/2 - contentLayout.height/2
                : outerFlickable.contentHeight - outerFlickable.height + outerFlickable.topEmptyArea + headerItem.height + footerItem.height
            duration: Units.longDuration
            easing.type: Easing.OutQuad
        }

        SequentialAnimation {
            id: closeAnimation
            ParallelAnimation {
                NumberAnimation {
                    target: outerFlickable
                    properties: "contentY"
                    to: outerFlickable.visibleArea.yPosition < (1 - outerFlickable.visibleArea.heightRatio)/2 ? -mainItem.height : outerFlickable.contentHeight
                    duration: Units.longDuration
                    easing.type: Easing.InQuad
                }
                OpacityAnimator {
                    target: mainItem
                    from: 1
                    to: 0
                    duration: Units.longDuration
                    easing.type: Easing.InQuad
                }
            }
            ScriptAction {
                script: {
                    scrollView.flickableItem.contentY = -mainItem.height;
                    mainItem.visible = root.sheetOpen = false;
                }
            }
        }
        Rectangle {
            anchors.fill: parent
            color: "black"
            opacity: 0.3 * Math.min(
                (Math.min(outerFlickable.contentY + outerFlickable.height, outerFlickable.height) / outerFlickable.height),
                (2 + (outerFlickable.contentHeight - outerFlickable.contentY - outerFlickable.topMargin - outerFlickable.bottomMargin)/outerFlickable.height))
        }

        FocusScope {
            id: flickableContents

            readonly property real listHeaderHeight: scrollView.flickableItem ? -scrollView.flickableItem.originY : 0

            y: (scrollView.contentItem != flickableContents ? -scrollView.flickableItem.contentY - listHeaderHeight  - (headerItem.visible ? headerItem.height : 0): 0)

            width: mainItem.contentItemPreferredWidth <= 0 ? mainItem.width : Math.max(mainItem.width/2, Math.min(mainItem.contentItemMaximumWidth, mainItem.contentItemPreferredWidth))

            height: scrollView.contentItem == flickableContents ? root.contentItem.height + topPadding + bottomPadding : 0
            Connections {
                target: enabled ? flickableContents.Window.activeFocusItem : null
                enabled: flickableContents.focus && flickableContents.Window.activeFocusItem && flickableContents.Window.activeFocusItem.hasOwnProperty("text")
                onTextChanged: {
                    if (Qt.inputMethod.cursorRectangle.y + Qt.inputMethod.cursorRectangle.height > mainItem.Window.height) {
                        scrollView.flickableItem.contentY += (Qt.inputMethod.cursorRectangle.y + Qt.inputMethod.cursorRectangle.height) - mainItem.Window.height
                    }
                }
            }

            Item {
                id: contentItemParent
                anchors {
                    fill: parent
                    leftMargin: leftPadding
                    topMargin: topPadding
                    rightMargin: rightPadding
                    bottomMargin: bottomPadding
                }
            }
        }

        Connections {
            target: scrollView.flickableItem
            onContentHeightChanged: {
                if (openAnimation.running) {
                    openAnimation.running = false;
                    open();
                }
            }
        }

        Flickable {
            id: outerFlickable
            anchors.fill: parent
            contentWidth: width
            topMargin: height
            bottomMargin: height
            // +1: we need the flickable to be always interactive
            contentHeight: Math.max(height+1, scrollView.flickableItem.contentHeight + topEmptyArea)

            readonly property int topEmptyArea: Math.max(height-scrollView.flickableItem.contentHeight, Units.gridUnit * 3)

            property int oldContentY: NaN
            property bool lastMovementWasDown: false
            property real startDraggingPos
            WheelHandler {
                target: outerFlickable
                scrollFlickableTarget: false
            }
            onContentYChanged: {
                if (scrollView.userInteracting) {
                    return;
                }

                let startPos = -scrollView.flickableItem.topMargin - flickableContents.listHeaderHeight;
                let pos = contentY - topEmptyArea - flickableContents.listHeaderHeight;
                let endPos = scrollView.flickableItem.contentHeight - scrollView.flickableItem.height + scrollView.flickableItem.bottomMargin - flickableContents.listHeaderHeight;

                if (endPos - pos > 0) {
                    contentLayout.y = Math.round(Math.max(root.topInset, scrollView.flickableItem.topMargin - pos - flickableContents.listHeaderHeight));
                } else if (scrollView.flickableItem.topMargin - pos < 0) {
                    contentLayout.y = Math.round(endPos - pos + root.topInset);
                }

                scrollView.flickableItem.contentY = Math.max(
                    startPos, Math.min(pos, endPos));

                lastMovementWasDown = contentY < oldContentY;
                oldContentY = contentY;
            }

            onFlickEnded: {
                if (openAnimation.running || closeAnimation.running) {
                    return;
                }
                if (scrollView.flickableItem.atYBeginning ||scrollView.flickableItem.atYEnd) {
                    resetAnimation.restart();
                }
            }

            onDraggingChanged: {
                if (dragging) {
                    startDraggingPos = contentY;
                    return;
                }

                let shouldClose = false;

                // close
                if (scrollView.flickableItem.atYBeginning) {
                    if (startDraggingPos - contentY > Units.gridUnit * 4 &&
                        contentY < -Units.gridUnit * 4 &&
                        lastMovementWasDown) {
                        shouldClose = true;
                    }
                }

                if (scrollView.flickableItem.atYEnd) {
                    if (contentY - startDraggingPos > Units.gridUnit * 4 &&
                        contentY > contentHeight - height + Units.gridUnit * 4  &&
                        !lastMovementWasDown) {
                        shouldClose = true;
                    }
                }

                if (shouldClose) {
                    closeAnimation.restart();
                } else if (scrollView.flickableItem.atYBeginning || scrollView.flickableItem.atYEnd) {
                    resetAnimation.restart();
                }
            }

            onHeightChanged: {
                if (scrollView.flickableItem.contentHeight < height) {
                    contentYChanged();
                }
            }

            ColumnLayout {
                id: contentLayout
                spacing: 0
                // Its events should be filtered but not scrolled
                parent: outerFlickable
                anchors.horizontalCenter: parent.horizontalCenter
                width: mainItem.contentItemPreferredWidth <= 0 ? mainItem.width : Math.max(mainItem.width/2, Math.min(mainItem.contentItemMaximumWidth, mainItem.contentItemPreferredWidth)) - root.leftInset - root.rightInset
                height: Math.min(implicitHeight, parent.height) - root.topInset - root.bottomInset

                // Even though we're not actually using any shadows here,
                // we're using a ShadowedRectangle instead of a regular
                // rectangle because it allows fine-grained control over which
                // corners to round, which we need here
                ShadowedRectangle {
                    id: headerItem
                    Layout.fillWidth: true
                    //Layout.margins: 1
                    visible: root.header || root.showCloseButton
                    implicitHeight: Math.max(headerParent.implicitHeight, closeIcon.height) + Units.smallSpacing * 2
                    z: 2
                    corners.topLeftRadius: Units.smallSpacing
                    corners.topRightRadius: Units.smallSpacing
                    Theme.colorSet: Theme.Header
                    Theme.inherit: false
                    color: Theme.backgroundColor
                    Item {
                        id: headerParent
                        implicitHeight: header ? header.implicitHeight : 0
                        anchors {
                            fill: parent
                            leftMargin: root.leftPadding
                            margins: Units.smallSpacing
                            rightMargin: closeIcon.width + Units.smallSpacing
                        }
                    }
                    Icon {
                        id: closeIcon
                        anchors {
                            right: parent.right
                            top: parent.top
                            margins: Units.smallSpacing
                            rightMargin: root.rightPadding
                            verticalCenter: headerItem.verticalCenter
                        }
                        z: 3
                        visible: root.showCloseButton
                        width: Units.iconSizes.smallMedium
                        height: width
                        source: closeMouseArea.containsMouse ? "window-close" : "window-close-symbolic"
                        active: closeMouseArea.containsMouse
                        MouseArea {
                            id: closeMouseArea
                            hoverEnabled: true
                            anchors.fill: parent
                            onClicked: root.close();
                        }
                    }
                    Separator {
                        anchors {
                            right: parent.right
                            left: parent.left
                            top: parent.bottom
                        }
                    }
                }

                ScrollView {
                    id: scrollView

                    property bool userInteracting: false
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    implicitHeight: flickableItem.contentHeight
                    Layout.maximumHeight: flickableItem.contentHeight
                }

                Connections {
                    target: scrollView.flickableItem
                    property real oldContentY: 0
                    onContentYChanged: {
                        if (outerFlickable.moving) {
                            oldContentY = scrollView.flickableItem.contentY;
                            return;
                        }
                        scrollView.userInteracting = true;

                        let diff = scrollView.flickableItem.contentY - oldContentY

                        outerFlickable.contentY = outerFlickable.contentY + diff;

                        if (diff > 0) {
                            contentLayout.y = Math.max(root.topInset,  contentLayout.y - diff);
                        } else if (scrollView.flickableItem.contentY < outerFlickable.topEmptyArea + headerItem.height) {
                            contentLayout.y = Math.min(outerFlickable.topEmptyArea,  contentLayout.y + (contentLayout.y - diff)) + root.topInset;
                        }

                        oldContentY = scrollView.flickableItem.contentY;
                        scrollView.userInteracting = false;
                    }
                }
                Item {
                    visible: footerItem.visible
                    implicitHeight: footerItem.height
                }
            }

            // footer item is outside the layout as it should never scroll away

            // Even though we're not actually using any shadows here,
            // we're using a ShadowedRectangle instead of a regular
            // rectangle because it allows fine-grained control over which
            // corners to round, which we need here
            ShadowedRectangle {
                id: footerItem
                width: contentLayout.width - 2
                corners.bottomLeftRadius: Units.smallSpacing
                corners.bottomRightRadius: Units.smallSpacing
                parent: outerFlickable
                x: contentLayout.x + 1
                y: Math.min(parent.height, contentLayout.y + contentLayout.height  -1) - height
                visible: root.footer
                implicitHeight: footerParent.implicitHeight + Units.smallSpacing * 2 + extraMargin
                Theme.colorSet: Theme.Window
                Theme.inherit: false
                color: Theme.backgroundColor

                //Show an extra margin when:
                //* the application is in mobile mode (no toolbarapplicationheader)
                //* the bottom screen controls are visible
                //* the sheet is displayed *under* the controls
                property int extraMargin: (!root.parent ||
                    typeof applicationWindow === "undefined" ||
                    (root.parent === applicationWindow().overlay) ||
                    !applicationWindow().controlsVisible ||
                    (applicationWindow().pageStack && applicationWindow().pageStack.globalToolBar && applicationWindow().pageStack.globalToolBar.actualStyle === ApplicationHeaderStyle.ToolBar) ||
                    (applicationWindow().header && applicationWindow().header.toString().indexOf("ToolBarApplicationHeader") === 0))
                        ? 0 : Units.gridUnit * 3

                z: 2
                Item {
                    id: footerParent
                    implicitHeight: footer ? footer.implicitHeight : 0
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                        margins: Units.smallSpacing
                    }
                }

                Separator {
                    anchors {
                        right: parent.right
                        left: parent.left
                        bottom: parent.top
                        leftMargin: -1
                        rightMargin: -1
                    }
                }
            }
        }
    }
}
