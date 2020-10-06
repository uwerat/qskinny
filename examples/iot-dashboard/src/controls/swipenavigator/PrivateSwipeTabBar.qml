/*
 *  SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.12

import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import org.kde.kirigami 2.12 as Kirigami

ScrollView {
    id: view
    implicitWidth: bar.implicitWidth
    ScrollBar.horizontal.visible: false

    Item {
        height: view.height
        implicitHeight: bar.implicitHeight
        implicitWidth: bar.implicitWidth
        width: Math.max(view.width, bar.implicitWidth)

        RowLayout {
            id: bar
            spacing: 0
            signal indexChanged(real xPos, real tabWidth)

            anchors.centerIn: parent
            width: Kirigami.Settings.isMobile && swipeNavigatorRoot.height > swipeNavigatorRoot.width ? parent.width : implicitWidth
            property real targetDestination
            NumberAnimation {
                id: scrollAni
                target: view.ScrollBar.horizontal
                property: "position"
                to: bar.targetDestination
                duration: Kirigami.Units.longDuration
                easing.type: Easing.OutExpo
            }
            onIndexChanged: {
                if (xPos > (bar.width)/2) {
                    bar.targetDestination = (1-view.ScrollBar.horizontal.size) * ((xPos+tabWidth) / bar.width)
                    scrollAni.restart()
                } else {
                    bar.targetDestination = (1-view.ScrollBar.horizontal.size) * ((xPos) / bar.width)
                    scrollAni.restart()
                }
            }

            property Item layouter: Item {
                Row {
                    id: expandedLayouter
                    Repeater {
                        model: swipeNavigatorRoot.pages
                        delegate: PrivateSwipeTab { vertical: false }
                    }
                }
            }

            Repeater {
                model: swipeNavigatorRoot.pages
                delegate: PrivateSwipeTab {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    vertical: Kirigami.Settings.isMobile
                        ? (swipeNavigatorRoot.width < swipeNavigatorRoot.height ? true : expandedLayouter.width > swipeNavigatorRoot.width)
                        : expandedLayouter.width > swipeNavigatorRoot.width
                    onIndexChanged: bar.indexChanged(xPos, tabWidth)
                }
            }
        }
    }
}
