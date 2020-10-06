/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0 as Controls
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.7

import "../templates/private"

Item {
    id: root

    anchors {
        left: parent.left
        right: parent.right
        bottom: parent.bottom
        bottomMargin: root.page.footer ? root.page.footer.height : 0
    }
    //smallSpacing for the shadow
    implicitHeight: button.height + Units.smallSpacing
    clip: true

    readonly property Page page: root.parent.page
    //either Action or QAction should work here

    function isActionAvailable(action) { return action && (action.hasOwnProperty("visible") ? action.visible === undefined || action.visible : !action.hasOwnProperty("visible")); }

    readonly property QtObject action: root.page && isActionAvailable(root.page.mainAction) ? root.page.mainAction : null
    readonly property QtObject leftAction: root.page && isActionAvailable(root.page.leftAction) ? root.page.leftAction : null
    readonly property QtObject rightAction: root.page && isActionAvailable(root.page.rightAction) ? root.page.rightAction : null

    readonly property bool hasApplicationWindow: typeof applicationWindow !== "undefined" && applicationWindow
    readonly property bool hasGlobalDrawer: typeof globalDrawer !== "undefined" && globalDrawer
    readonly property bool hasContextDrawer: typeof contextDrawer !== "undefined" && contextDrawer

    transform: Translate {
        id: translateTransform
        y: mouseArea.internalVisibility ? 0 : button.height
        Behavior on y {
            NumberAnimation {
                duration: Units.longDuration
                easing.type: mouseArea.internalVisibility == true ? Easing.InQuad : Easing.OutQuad
            }
        }
    }

    onWidthChanged: button.x = root.width/2 - button.width/2
    Item {
        id: button
        x: root.width/2 - button.width/2

        anchors.bottom: edgeMouseArea.bottom

        implicitWidth: implicitHeight + Units.iconSizes.smallMedium*2 + Units.gridUnit
        implicitHeight: Units.iconSizes.medium + Units.largeSpacing * 2


        onXChanged: {
            if (mouseArea.pressed || edgeMouseArea.pressed || fakeContextMenuButton.pressed) {
                if (root.hasGlobalDrawer && globalDrawer.enabled && globalDrawer.modal) {
                    globalDrawer.peeking = true;
                    globalDrawer.visible = true;
                    globalDrawer.position = Math.min(1, Math.max(0, (x - root.width/2 + button.width/2)/globalDrawer.contentItem.width + mouseArea.drawerShowAdjust));
                }
                if (root.hasContextDrawer && contextDrawer.enabled && contextDrawer.modal) {
                    contextDrawer.peeking = true;
                    contextDrawer.visible = true;
                    contextDrawer.position = Math.min(1, Math.max(0, (root.width/2 - button.width/2 - x)/contextDrawer.contentItem.width + mouseArea.drawerShowAdjust));
                }
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent

            visible: action != null || leftAction != null || rightAction != null
            property bool internalVisibility: (!root.hasApplicationWindow || (applicationWindow().controlsVisible && applicationWindow().height > root.height*2)) && (root.action === null || root.action.visible === undefined || root.action.visible)
            preventStealing: true

            drag {
                target: button
                //filterChildren: true
                axis: Drag.XAxis
                minimumX: root.hasContextDrawer && contextDrawer.enabled && contextDrawer.modal ? 0 : root.width/2 - button.width/2
                maximumX: root.hasGlobalDrawer && globalDrawer.enabled && globalDrawer.modal ? root.width : root.width/2 - button.width/2
            }

            property var downTimestamp;
            property int startX
            property int startMouseY
            property real drawerShowAdjust

            readonly property int currentThird: (3*mouseX)/width
            readonly property QtObject actionUnderMouse: {
                switch(currentThird) {
                    case 0: return leftAction;
                    case 1: return action;
                    case 2: return rightAction;
                    default: return null
                }
            }

            hoverEnabled: true

            Controls.ToolTip.visible: containsMouse && !Settings.tabletMode && actionUnderMouse
            Controls.ToolTip.text: actionUnderMouse ? actionUnderMouse.text : ""
            Controls.ToolTip.delay: Units.toolTipDelay

            onPressed: {
                //search if we have a page to set to current
                if (root.hasApplicationWindow && applicationWindow().pageStack.currentIndex !== undefined && root.page.ColumnView.level !== undefined) {
                    //search the button parent's parent, that is the page parent
                    //this will make the context drawer open for the proper page
                    applicationWindow().pageStack.currentIndex = root.page.ColumnView.level;
                }
                downTimestamp = (new Date()).getTime();
                startX = button.x + button.width/2;
                startMouseY = mouse.y;
                drawerShowAdjust = 0;
            }
            onReleased: {
                if (root.hasGlobalDrawer) globalDrawer.peeking = false;
                if (root.hasContextDrawer) contextDrawer.peeking = false;
                //pixel/second
                var x = button.x + button.width/2;
                var speed = ((x - startX) / ((new Date()).getTime() - downTimestamp) * 1000);
                drawerShowAdjust = 0;

                //project where it would be a full second in the future
                if (root.hasContextDrawer && root.hasGlobalDrawer && globalDrawer.modal && x + speed > Math.min(root.width/4*3, root.width/2 + globalDrawer.contentItem.width/2)) {
                    globalDrawer.open();
                    contextDrawer.close();
                } else if (root.hasContextDrawer && x + speed < Math.max(root.width/4, root.width/2 - contextDrawer.contentItem.width/2)) {
                    if (root.hasContextDrawer && contextDrawer.modal) {
                        contextDrawer.open();
                    }
                    if (root.hasGlobalDrawer && globalDrawer.modal) {
                        globalDrawer.close();
                    }
                } else {
                    if (root.hasGlobalDrawer && globalDrawer.modal) {
                        globalDrawer.close();
                    }
                    if (root.hasContextDrawer && contextDrawer.modal) {
                        contextDrawer.close();
                    }
                }
                //Don't rely on native onClicked, but fake it here:
                //Qt.startDragDistance is not adapted to devices dpi in case
                //of Android, so consider the button "clicked" when:
                //*the button has been dragged less than a gridunit
                //*the finger is still on the button
                if (Math.abs((button.x + button.width/2) - startX) < Units.gridUnit &&
                    mouse.y > 0) {

                    //if an action has been assigned, trigger it
                    if (actionUnderMouse && actionUnderMouse.trigger) {
                        actionUnderMouse.trigger();
                    }

                    if (actionUnderMouse && actionUnderMouse.hasOwnProperty("children") && actionUnderMouse.children.length > 0) {
                        var subMenuUnderMouse;
                        switch (actionUnderMouse) {
                        case leftAction:
                            subMenuUnderMouse = leftActionSubMenu;
                            break;
                        case mainAction:
                            subMenuUnderMouse = mainActionSubMenu;
                            break
                        case rightAction:
                            subMenuUnderMouse = rightActionSubMenu;
                            break;
                        }
                        if (subMenuUnderMouse && !subMenuUnderMouse.visible) {
                            subMenuUnderMouse.visible = true;
                        }
                    }
                }
            }

            onPositionChanged: {
                drawerShowAdjust = Math.min(0.3, Math.max(0, (startMouseY - mouse.y)/(Units.gridUnit*15)));
                button.xChanged();
            }
            onPressAndHold: {
                if (!actionUnderMouse) {
                    return;
                }

                //if an action has been assigned, show a message like a tooltip
                if (actionUnderMouse && actionUnderMouse.text && Settings.tabletMode) {
                    Controls.ToolTip.show(actionUnderMouse.text, 3000)
                }
            }
            Connections {
                target: root.hasGlobalDrawer ? globalDrawer : null
                onPositionChanged: {
                    if ( globalDrawer && globalDrawer.modal && !mouseArea.pressed && !edgeMouseArea.pressed && !fakeContextMenuButton.pressed) {
                        button.x = globalDrawer.contentItem.width * globalDrawer.position + root.width/2 - button.width/2;
                    }
                }
            }
            Connections {
                target: root.hasContextDrawer ? contextDrawer : null
                onPositionChanged: {
                    if (contextDrawer && contextDrawer.modal && !mouseArea.pressed && !edgeMouseArea.pressed && !fakeContextMenuButton.pressed) {
                        button.x = root.width/2 - button.width/2 - contextDrawer.contentItem.width * contextDrawer.position;
                    }
                }
            }

            Item {
                id: background
                anchors {
                    fill: parent
                }

                Rectangle {
                    id: buttonGraphics
                    radius: width/2
                    anchors.centerIn: parent
                    height: parent.height - Units.smallSpacing*2
                    width: height
                    enabled: root.action && root.action.enabled
                    visible: root.action
                    readonly property bool pressed: root.action && root.action.enabled && ((root.action == mouseArea.actionUnderMouse && mouseArea.pressed) || root.action.checked)
                    property color baseColor: root.action && root.action.icon && root.action.icon.color && root.action.icon.color != undefined && root.action.icon.color.a > 0 ? root.action.icon.color : Theme.highlightColor
                    color: pressed ? Qt.darker(baseColor, 1.3) : baseColor

                    ActionsMenu {
                        id: mainActionSubMenu
                        y: -height
                        x: -width/2 + parent.width/2
                        actions: root.action && root.action.hasOwnProperty("children") ? root.action.children : ""
                        submenuComponent: Component {
                            ActionsMenu {}
                        }
                    }
                    Icon {
                        id: icon
                        anchors.centerIn: parent
                        width: Units.iconSizes.smallMedium
                        height: width
                        source: root.action && root.action.icon.name ? root.action.icon.name : ""
                        selected: true
                        color: root.action && root.action.color && root.action.color.a > 0 ? root.action.color : (selected ? Theme.highlightedTextColor : Theme.textColor)
                    }
                    Behavior on color {
                        ColorAnimation {
                            duration: Units.longDuration
                            easing.type: Easing.InOutQuad
                        }
                    }
                    Behavior on x {
                        NumberAnimation {
                            duration: Units.longDuration
                            easing.type: Easing.InOutQuad
                        }
                    }
                }
                //left button
                Rectangle {
                    id: leftButtonGraphics
                    z: -1
                    anchors {
                        left: parent.left
                        bottom: parent.bottom
                        bottomMargin: Units.smallSpacing
                    }
                    enabled: root.leftAction && root.leftAction.enabled
                    radius: Units.devicePixelRatio*2
                    height: Units.iconSizes.smallMedium + Units.smallSpacing * 2
                    width: height + (root.action ? Units.gridUnit*2 : 0)
                    visible: root.leftAction

                    readonly property bool pressed: root.leftAction && root.leftAction.enabled && ((mouseArea.actionUnderMouse == root.leftAction && mouseArea.pressed) || root.leftAction.checked)
                    property color baseColor: root.leftAction && root.leftAction.icon && root.leftAction.icon.color && root.leftAction.icon.color != undefined && root.leftAction.icon.color.a > 0 ? root.leftAction.icon.color : Theme.highlightColor
                    color: pressed ? baseColor : Theme.backgroundColor
                    Behavior on color {
                        ColorAnimation {
                            duration: Units.longDuration
                            easing.type: Easing.InOutQuad
                        }
                    }
                    ActionsMenu {
                        id: leftActionSubMenu
                        y: -height
                        x: -width/2 + parent.width/2
                        actions: root.leftAction && root.leftAction.hasOwnProperty("children") ? root.leftAction.children : ""
                        submenuComponent: Component {
                            ActionsMenu {}
                        }
                    }
                    Icon {
                        source: root.leftAction && root.leftAction.icon.name ? root.leftAction.icon.name : ""
                        width: Units.iconSizes.smallMedium
                        height: width
                        selected: leftButtonGraphics.pressed
                        color: root.action && root.action.color && root.action.color.a > 0 ? root.action.color : (selected ? Theme.highlightedTextColor : Theme.textColor)
                        anchors {
                            left: parent.left
                            verticalCenter: parent.verticalCenter
                            margins: Units.smallSpacing * 2
                        }
                    }
                }
                //right button
                Rectangle {
                    id: rightButtonGraphics
                    z: -1
                    anchors {
                        right: parent.right
                        //verticalCenter: parent.verticalCenter
                        bottom: parent.bottom
                        bottomMargin: Units.smallSpacing
                    }
                    enabled: root.rightAction && root.rightAction.enabled
                    radius: Units.devicePixelRatio*2
                    height: Units.iconSizes.smallMedium + Units.smallSpacing * 2
                    width: height + (root.action ? Units.gridUnit*2 : 0)
                    visible: root.rightAction
                    readonly property bool pressed: root.rightAction && root.rightAction.enabled && ((mouseArea.actionUnderMouse == root.rightAction && mouseArea.pressed) || root.rightAction.checked)
                    property color baseColor: root.rightAction && root.rightAction.icon && root.rightAction.icon.color && root.rightAction.icon.color != undefined && root.rightAction.icon.color.a > 0 ? root.rightAction.icon.color : Theme.highlightColor
                    color: pressed ? baseColor : Theme.backgroundColor
                    Behavior on color {
                        ColorAnimation {
                            duration: Units.longDuration
                            easing.type: Easing.InOutQuad
                        }
                    }
                    ActionsMenu {
                        id: rightActionSubMenu
                        y: -height
                        x: -width/2 + parent.width/2
                        actions: root.rightAction && root.rightAction.hasOwnProperty("children") ? root.rightAction.children : ""
                        submenuComponent: Component {
                            ActionsMenu {}
                        }
                    }
                    Icon {
                        source: root.rightAction && root.rightAction.icon.name ? root.rightAction.icon.name : ""
                        width: Units.iconSizes.smallMedium
                        height: width
                        selected: rightButtonGraphics.pressed
                        color: root.action && root.action.color && root.action.color.a > 0 ? root.action.color : (selected ? Theme.highlightedTextColor : Theme.textColor)
                        anchors {
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                            margins: Units.smallSpacing * 2
                        }
                    }
                }
            }

            DropShadow {
                anchors.fill: background
                horizontalOffset: 0
                verticalOffset: Units.devicePixelRatio
                radius: Units.gridUnit /2
                samples: 16
                color: Qt.rgba(0, 0, 0, mouseArea.pressed ? 0.6 : 0.4)
                source: background
            }
        }
    }

    MouseArea {
        id: fakeContextMenuButton
        anchors {
            right: edgeMouseArea.right
            bottom: parent.bottom
            margins: Units.smallSpacing
        }
        drag {
            target: button
            axis: Drag.XAxis
            minimumX: root.hasContextDrawer && contextDrawer.enabled && contextDrawer.modal ? 0 : root.width/2 - button.width/2
            maximumX: root.hasGlobalDrawer && globalDrawer.enabled && globalDrawer.modal ? root.width : root.width/2 - button.width/2
        }
        visible: root.page.actions && root.page.actions.contextualActions.length > 0 && (applicationWindow === undefined || applicationWindow().wideScreen)
            //using internal pagerow api
            && (root.page && root.page.parent ? root.page.ColumnView.level < applicationWindow().pageStack.depth-1 : false)

        width: Units.iconSizes.smallMedium + Units.smallSpacing*2
        height: width


        DropShadow {
            anchors.fill: handleGraphics
            horizontalOffset: 0
            verticalOffset: Units.devicePixelRatio
            radius: Units.gridUnit /2
            samples: 16
            color: Qt.rgba(0, 0, 0, fakeContextMenuButton.pressed ? 0.6 : 0.4)
            source: handleGraphics
        }
        Rectangle {
            id: handleGraphics
            anchors.fill: parent
            color: fakeContextMenuButton.pressed ? Theme.highlightColor : Theme.backgroundColor
            radius: Units.devicePixelRatio
            Icon {
                anchors.centerIn: parent
                width: Units.iconSizes.smallMedium
                selected: fakeContextMenuButton.pressed
                height: width
                source: "overflow-menu"
            }
            Behavior on color {
                ColorAnimation {
                    duration: Units.longDuration
                    easing.type: Easing.InOutQuad
                }
            }
        }

        onPressed: {
            mouseArea.onPressed(mouse)
        }
        onReleased: {
            if (globalDrawer) {
                globalDrawer.peeking = false;
            }
            if (contextDrawer) {
                contextDrawer.peeking = false;
            }
            var pos = root.mapFromItem(fakeContextMenuButton, mouse.x, mouse.y);
            if (contextDrawer) {
                if (pos.x < root.width/2) {
                    contextDrawer.open();
                } else if (contextDrawer.drawerOpen && mouse.x > 0 && mouse.x < width) {
                    contextDrawer.close();
                }
            }
            if (globalDrawer) {
                if (globalDrawer.position > 0.5) {
                    globalDrawer.open();
                } else {
                    globalDrawer.close();
                }
            }
            if (containsMouse && (!globalDrawer || !globalDrawer.drawerOpen || !globalDrawer.modal) &&
                (!contextDrawer || !contextDrawer.drawerOpen || !contextDrawer.modal)) {
                contextMenu.visible = !contextMenu.visible;
            }
        }
        ActionsMenu {
            id: contextMenu
            x: parent.width - width
            y: -height
            actions: root.page.actions.contextualActions
            submenuComponent: Component {
                ActionsMenu {}
            }
        }
    }

    MouseArea {
        id: edgeMouseArea
        z:99
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        drag {
            target: button
            //filterChildren: true
            axis: Drag.XAxis
            minimumX: root.hasContextDrawer && contextDrawer.enabled && contextDrawer.modal ? 0 : root.width/2 - button.width/2
            maximumX: root.hasGlobalDrawer && globalDrawer.enabled && globalDrawer.modal ? root.width : root.width/2 - button.width/2
        }
        height: Units.smallSpacing * 3

        onPressed: mouseArea.onPressed(mouse)
        onPositionChanged: mouseArea.positionChanged(mouse)
        onReleased: mouseArea.released(mouse)
    }
}
