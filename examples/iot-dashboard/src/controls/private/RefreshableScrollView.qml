/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.0 as QQC2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.4
import "../templates/private" as P

P.ScrollView {
    id: root

    /**
     * type: bool
     * If true the list is asking for refresh and will show a loading spinner.
     * it will automatically be set to true when the user pulls down enough the list.
     * This signals the application logic to start its refresh procedure.
     * The application itself will have to set back this property to false when done.
     */
    property bool refreshing: false

    /**
     * type: bool
     * If true the list supports the "pull down to refresh" behavior.
     */
    property bool supportsRefreshing: false

    /**
     * leftPadding: int
     * default contents padding at left
     */
    property int leftPadding: Units.gridUnit

    /**
     * topPadding: int
     * default contents padding at top
     */
    property int topPadding: Units.gridUnit

    /**
     * rightPadding: int
     * default contents padding at right
     */
    property int rightPadding: Units.gridUnit

    /**
     * bottomPadding: int
     * default contents padding at bottom
     */
    property int bottomPadding: Units.gridUnit

    /**
     * Set when this scrollview manages a whole page
     */
    property Page page

    property Item _swipeFilter

    onRefreshingChanged: flickableItem.topMargin = topPadding + (refreshing ? busyIndicatorFrame.height : 0);
    children: [
        Item {
            id: busyIndicatorFrame
            z: 99
            y: root.flickableItem.verticalLayoutDirection === ListView.BottomToTop
                ? -root.flickableItem.contentY+root.flickableItem.originY+height
                : -root.flickableItem.contentY+root.flickableItem.originY-height
            width: root.flickableItem.width
            height: busyIndicator.height + Units.gridUnit * 2
            QQC2.BusyIndicator {
                id: busyIndicator
                anchors.centerIn: parent
                running: root.refreshing
                visible: root.refreshing
                //Android busywidget QQC seems to be broken at custom sizes
            }
            Rectangle {
                id: spinnerProgress
                anchors {
                    fill: busyIndicator
                    margins: Math.ceil(Units.smallSpacing)
                }
                radius: width
                visible: supportsRefreshing && !refreshing && progress > 0
                color: "transparent"
                opacity: 0.8
                border.color: Theme.backgroundColor
                border.width: Math.ceil(Units.smallSpacing)
                property real progress: supportsRefreshing && !refreshing ? (parent.y/busyIndicatorFrame.height) : 0
            }
            ConicalGradient {
                source: spinnerProgress
                visible: spinnerProgress.visible
                anchors.fill: spinnerProgress
                gradient: Gradient {
                    GradientStop { position: 0.00; color: Theme.highlightColor }
                    GradientStop { position: spinnerProgress.progress; color: Theme.highlightColor }
                    GradientStop { position: spinnerProgress.progress + 0.01; color: "transparent" }
                    GradientStop { position: 1.00; color: "transparent" }
                }
            }

            onYChanged: {
                //it's overshooting enough and not reachable: start countdown for reachability

                if (y > root.topPadding + Units.gridUnit && (typeof(applicationWindow) == "undefined" || !applicationWindow().reachableMode)) {
                    overshootResetTimer.running = true;
                //not reachable and not overshooting enough, stop reachability countdown
                } else if (typeof(applicationWindow) == "undefined" || !applicationWindow().reachableMode) {
                    //it's important it doesn't restart
                    overshootResetTimer.running = false;
                }

                if (!supportsRefreshing) {
                    return;
                }

                if (!root.refreshing && y > busyIndicatorFrame.height/2 + topPadding) {
                    refreshTriggerTimer.running = true;
                } else {
                    refreshTriggerTimer.running = false;
                }
            }
            Timer {
                id: refreshTriggerTimer
                interval: 500
                onTriggered: {
                    if (!root.refreshing && parent.y > busyIndicatorFrame.height/2 + topPadding) {
                        root.refreshing = true;
                    }
                }
            }
            Connections {
                enabled: typeof applicationWindow !== "undefined"
                target: typeof applicationWindow !== "undefined" ? applicationWindow() : null
                onReachableModeChanged: {
                    overshootResetTimer.running = applicationWindow().reachableMode;
                }
            }
            Timer {
                id: overshootResetTimer
                interval: (typeof applicationWindow !== "undefined"  && applicationWindow().reachableMode) ? 8000 : 2000
                onTriggered: {
                    //put it there because widescreen may have changed since timer start
                    if (!Settings.isMobile || (typeof applicationWindow !== "undefined"  && applicationWindow().wideScreen) || root.flickableItem.verticalLayoutDirection === ListView.BottomToTop) {
                        return;
                    }
                    applicationWindow().reachableMode = !applicationWindow().reachableMode;
                }
            }

            Binding {
                target: root.flickableItem
                property: "flickableDirection"
                value: Flickable.VerticalFlick
            }

            Binding {
                target: root.flickableItem
                property: "bottomMargin"
                value: root.page.bottomPadding
            }

            Binding {
                target: root.contentItem
                property: "width"
                value: root.flickableItem.width
            }
        }
    ]

    Component.onCompleted: leftPaddingChanged()
    onRightPaddingChanged: leftPaddingChanged()
    onLeftPaddingChanged: {
        //for gridviews do apply margins
        if (root.contentItem == root.flickableItem) {
            if (typeof root.flickableItem.cellWidth != "undefined") {
                flickableItem.anchors.leftMargin = leftPadding;
                flickableItem.anchors.rightMargin = rightPadding;
            } else {
                flickableItem.anchors.leftMargin = 0;
                flickableItem.anchors.rightMargin = 0;
            }
            flickableItem.anchors.topMargin = 0;
            flickableItem.anchors.bottomMargin = 0;
        } else {
            flickableItem.anchors.leftMargin = leftPadding;
            flickableItem.anchors.topMargin = topPadding;
            flickableItem.anchors.rightMargin = rightPadding;
            flickableItem.anchors.bottomMargin = bottomPadding;
        }
    }
}
