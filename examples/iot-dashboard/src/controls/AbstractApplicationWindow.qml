/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.5
import QtQuick.Controls 2.0 as QQC2
import "templates/private"
import org.kde.kirigami 2.4
import QtGraphicalEffects 1.0

/**
 * A window that provides some basic features needed for all apps
 * Use this class only if you need a custom content for your application,
 * different from the Page Row behavior recommended by the HIG and provided
 * by ApplicationWindow.
 * It is recommended to use ApplicationWindow instead
 * @see ApplicationWindow
 *
 * It's usually used as a root QML component for the application.
 * It provides support for a central page stack, side drawers and
 * a top ApplicationHeader, as well as basic support for the
 * Android back button
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
 *
 *     contextDrawer: Kirigami.ContextDrawer {
 *         id: contextDrawer
 *     }
 *
 *     pageStack: PageStack {
 *         ...
 *     }
 *  [...]
 * }
 * @endcode
 *
 * @inherit QtQuick.Controls.ApplicationWindow
 */
QQC2.ApplicationWindow {
    id: root

    /**
     * pageStack: StackView
     * Readonly.
     * The stack used to allocate the pages and to manage the transitions
     * between them.
     * Put a container here, such as QQuickControls PageStack
     */
    property Item pageStack
    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    /**
     * Shows a little passive notification at the bottom of the app window
     * lasting for few seconds, with an optional action button.
     *
     * @param message The text message to be shown to the user.
     * @param timeout How long to show the message:
     *            possible values: "short", "long" or the number of milliseconds
     * @param actionText Text in the action button, if any.
     * @param callBack A JavaScript function that will be executed when the
     *            user clicks the button.
     */
    function showPassiveNotification(message, timeout, actionText, callBack) {
        if (!internal.__passiveNotification) {
            var component = Qt.createComponent("templates/private/PassiveNotification.qml");
            internal.__passiveNotification = component.createObject(overlay.parent);
        }

        internal.__passiveNotification.showNotification(message, timeout, actionText, callBack);
    }

   /**
    * Hide the passive notification, if any is shown
    */
    function hidePassiveNotification() {
        if(internal.__passiveNotification) {
           internal.__passiveNotification.hideNotification();
        }
    }


    /**
     * @returns a pointer to this application window
     * can be used anywhere in the application.
     */
    function applicationWindow() {
        return root;
    }

   /**
    * header: ApplicationHeader
    * An item that can be used as a title for the application.
    * Scrolling the main page will make it taller or shorter (trough the point of going away)
    * It's a behavior similar to the typical mobile web browser addressbar
    * the minimum, preferred and maximum heights of the item can be controlled with
    * * Layout.minimumHeight: default is 0, i.e. hidden
    * * Layout.preferredHeight: default is Units.gridUnit * 1.6
    * * Layout.maximumHeight: default is Units.gridUnit * 3
    *
    * To achieve a titlebar that stays completely fixed just set the 3 sizes as the same
    * //FIXME: this should become an actual ApplicationHeader
    */
    //header: undefined

    /**
     * controlsVisible: bool
     * This property controls whether the standard chrome of the app, such
     * as the Action button, the drawer handles and the application
     * header should be visible or not.
     */
    property bool controlsVisible: true

    /**
     * globalDrawer: OverlayDrawer
     * The drawer for global actions, that will be opened by sliding from the
     * left screen edge or by dragging the ActionButton to the right.
     * It is recommended to use the GlobalDrawer class here
     */
    property OverlayDrawer globalDrawer

    /**
     * wideScreen: bool
     * If true the application is considered to be in "widescreen" mode, such as on desktops or horizontal tablets.
     * Different styles can have an own logic for deciding this
     */
    property bool wideScreen: width >= Units.gridUnit * 60

    /**
     * contextDrawer: OverlayDrawer
     * The drawer for context-dependent actions, that will be opened by sliding from the
     * right screen edge or by dragging the ActionButton to the left.
     * It is recommended to use the ContextDrawer class here.
     * The contents of the context drawer should depend from what page is
     * loaded in the main pageStack
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
     * When this page will be the current one, the context drawer will visualize
     * contextualActions defined as property in that page.
     */
    property OverlayDrawer contextDrawer

    /**
     * reachableMode: bool
     * When true the application is in reachable mode for single hand use.
     * the whole content of the application is moved down the screen to be
     * reachable with the thumb. if wideScreen is true, or reachableModeEnabled is false,
     * tis property has no effect.
     */
    property bool reachableMode: false

    /**
     * When true the application will go into reachable mode on pull down
     */
    property bool reachableModeEnabled: true

    color: Theme.backgroundColor

    MouseArea {
        parent: contentItem.parent
        z: -1
        anchors.fill: parent
        onClicked: root.reachableMode = false;
        visible: root.reachableMode && root.reachableModeEnabled
        Rectangle {
            anchors.fill: parent
            color: Qt.rgba(0, 0, 0, 0.3)
            opacity: 0.15
            Icon {
                anchors.horizontalCenter: parent.horizontalCenter
                y: x
                width: Units.iconSizes.large
                height: width
                source: "go-up"
            }
        }
    }

    contentItem.anchors.left: contentItem.parent.left
    contentItem.anchors.right: contentItem.parent.right
    contentItem.anchors.topMargin: root.wideScreen && header && controlsVisible ? header.height : 0
    contentItem.anchors.leftMargin: root.globalDrawer && (root.globalDrawer.modal === false) ? root.globalDrawer.width * root.globalDrawer.position : 0
    contentItem.anchors.rightMargin: root.contextDrawer && root.contextDrawer.modal === false ? root.contextDrawer.width * root.contextDrawer.position : 0

    Binding {
        when: menuBar !== undefined
        target: menuBar
        property: "x"
        value: -contentItem.x
    }
    Binding {
        when: header !== undefined
        target: header
        property: "x"
        value: -contentItem.x
    }
    Binding {
        when: footer !== undefined
        target: footer
        property: "x"
        value: -contentItem.x
    }

    contentItem.transform: Translate {
        Behavior on y {
            NumberAnimation {
                duration: Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }
        y: root.reachableMode && root.reachableModeEnabled && !root.wideScreen ? root.height/2 : 0
        x: root.globalDrawer && root.globalDrawer.modal === true && root.globalDrawer.toString().indexOf("SplitDrawer") === 0 ? root.globalDrawer.contentItem.width * root.globalDrawer.position : 0
    }
    //Don't want overscroll in landscape mode
    onWidthChanged: {
        if (width > height) {
            root.reachableMode = false;
        }
    }
    Binding {
        when: globalDrawer !== undefined && root.visible
        target: globalDrawer
        property: "parent"
        value: overlay
    }
    Binding {
        when: contextDrawer !== undefined && root.visible
        target: contextDrawer
        property: "parent"
        value: overlay
    }
    onPageStackChanged: pageStack.parent = contentItem;

    width: Units.gridUnit * 30
    height: Units.gridUnit * 45
    visible: true


    QtObject {
        id: internal
        property QtObject __passiveNotification
    }

    Shortcut {
        sequence: StandardKey.Quit
        onActivated: root.close()
    }
}
