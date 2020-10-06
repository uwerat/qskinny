/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1
import QtGraphicalEffects 1.0
import QtQuick.Templates 2.0 as T2
import org.kde.kirigami 2.5

import "private"
import "templates" as T

/**
 * Overlay Drawers are used to expose additional UI elements needed for
 * small secondary tasks for which the main UI elements are not needed.
 * For example in Okular Mobile, an Overlay Drawer is used to display
 * thumbnails of all pages within a document along with a search field.
 * This is used for the distinct task of navigating to another page.
 */
T.OverlayDrawer {
    id: root

//BEGIN Properties
    focus: false
    modal: true
    drawerOpen: !modal
    closePolicy: modal ? T2.Popup.CloseOnEscape | T2.Popup.CloseOnReleaseOutside : T2.Popup.NoAutoClose
    handleVisible: interactive && (modal || !drawerOpen) && (typeof(applicationWindow)===typeof(Function) && applicationWindow() ? applicationWindow().controlsVisible : true)

    onPositionChanged: {
        if (!modal && !root.peeking && !root.animating) {
            position = 1;
        }
    }

    background: Rectangle {
        color: Theme.backgroundColor

        Item {
            parent: root.handle
            anchors.fill: parent

            DropShadow {
                anchors.fill: handleGraphics
                visible: !parent.parent.handleAnchor || !parent.parent.handleAnchor.visible || root.handle.pressed || (root.modal && root.position > 0)
                horizontalOffset: 0
                verticalOffset: Units.devicePixelRatio
                radius: Units.gridUnit /2
                samples: 16
                color: Qt.rgba(0, 0, 0, root.handle.pressed ? 0.6 : 0.4)
                source: handleGraphics
            }
            Rectangle {
                id: handleGraphics
                anchors.centerIn: parent

                Theme.colorSet: parent.parent.handleAnchor && parent.parent.handleAnchor.visible ? parent.parent.handleAnchor.Theme.colorSet : Theme.Button

                Theme.backgroundColor: parent.parent.handleAnchor && parent.parent.handleAnchor.visible ? parent.parent.handleAnchor.Theme.backgroundColor : undefined

                Theme.textColor: parent.parent.handleAnchor && parent.parent.handleAnchor.visible ? parent.parent.handleAnchor.Theme.textColor : undefined

                Theme.inherit: false
                color: root.handle.pressed ? Theme.highlightColor : Theme.backgroundColor

                visible: !parent.parent.handleAnchor || !parent.parent.handleAnchor.visible

                width: Units.iconSizes.smallMedium + Units.smallSpacing * 2
                height: width
                radius: Units.devicePixelRatio * 2
                Behavior on color {
                    ColorAnimation {
                        duration: Units.longDuration
                        easing.type: Easing.InOutQuad
                    }
                }
            }
            Loader {
                anchors.centerIn: handleGraphics
                width: height
                height: Units.iconSizes.smallMedium

                Theme.colorSet: handleGraphics.Theme.colorSet
                Theme.backgroundColor: handleGraphics.Theme.backgroundColor
                Theme.textColor: handleGraphics.Theme.textColor

                source: {
                    var edge = root.edge;
                    if (Qt.application.layoutDirection == Qt.RightToLeft) {
                        if (edge === Qt.LeftEdge) {
                            edge = Qt.RightEdge;
                        } else {
                            edge = Qt.LeftEdge;
                        }
                    }

                    if (root.handleClosedIcon.source && root.handleOpenIcon.source) {
                        return Qt.resolvedUrl("templates/private/GenericDrawerIcon.qml");
                    } else if (edge == Qt.LeftEdge ) {
                        return Qt.resolvedUrl("templates/private/MenuIcon.qml");
                    } else if(edge == Qt.RightEdge && root.hasOwnProperty("actions")) {
                        return Qt.resolvedUrl("templates/private/ContextIcon.qml");
                    }else {
                        return "";
                    }
                }
                onItemChanged: {
                    if(item) {
                        item.drawer = Qt.binding(function(){return root});
                        item.color = Qt.binding(function(){return root.handle.pressed ? Theme.highlightedTextColor : Theme.textColor});
                    }
                }
            }
        }


        Separator {
            anchors {
                right: root.edge == Qt.RightEdge ? parent.left : (root.edge == Qt.LeftEdge ? undefined : parent.right)
                left: root.edge == Qt.LeftEdge ? parent.right : (root.edge == Qt.RightEdge ? undefined : parent.left)
                top: root.edge == Qt.TopEdge ? parent.bottom : (root.edge == Qt.BottomEdge ? undefined : parent.top)
                bottom: root.edge == Qt.BottomEdge ? parent.top : (root.edge == Qt.TopEdge ? undefined : parent.bottom)
            }
            visible: !root.modal
        }
        EdgeShadow {
            z: -2
            visible: root.modal
            edge: root.edge
            anchors {
                right: root.edge == Qt.RightEdge ? parent.left : (root.edge == Qt.LeftEdge ? undefined : parent.right)
                left: root.edge == Qt.LeftEdge ? parent.right : (root.edge == Qt.RightEdge ? undefined : parent.left)
                top: root.edge == Qt.TopEdge ? parent.bottom : (root.edge == Qt.BottomEdge ? undefined : parent.top)
                bottom: root.edge == Qt.BottomEdge ? parent.top : (root.edge == Qt.TopEdge ? undefined : parent.bottom)
            }

            opacity: root.position == 0 ? 0 : 1

            Behavior on opacity {
                NumberAnimation {
                    duration: Units.longDuration
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }
}
