/*
 *  SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.5
import QtQuick.Controls 2.0 as Controls
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.4 as Kirigami



Flickable {
    id: root

    property Kirigami.PageRow pageRow: parent.pageRow

    contentHeight: height
    contentWidth: mainLayout.width
    clip: true
    boundsBehavior: Flickable.StopAtBounds

    Connections {
        target: pageRow
        onCurrentIndexChanged: {
            var currentItem = mainLayout.children[pageRow.currentIndex];
            listScrollAnim.running = false
            listScrollAnim.from = root.contentX;
            listScrollAnim.to = Math.max( 0,
                    Math.min(currentItem.x + currentItem.width/2 - root.width/2,
                    root.contentWidth - root.width))
            listScrollAnim.running = true;
        }
    }

    RowLayout {
        id: mainLayout
        height: parent.height
        spacing: 0
        Repeater {
            id: mainRepeater
            model: pageRow.depth
            delegate: MouseArea {
                Layout.preferredWidth: delegateLayout.implicitWidth
                Layout.fillHeight: true
                onClicked: pageRow.currentIndex = modelData;
                hoverEnabled: !Kirigami.Settings.tabletMode
                Rectangle {
                    color: Kirigami.Theme.highlightColor
                    anchors.fill: parent
                    radius: 3
                    opacity: mainRepeater.count > 1 && parent.containsMouse ? 0.1 : 0
                }
                RowLayout {
                    id: delegateLayout
                    anchors.fill: parent
                    readonly property Kirigami.Page page: pageRow.get(modelData)
                    spacing: 0

                    Kirigami.Icon {
                        visible: modelData > 0
                        Layout.alignment: Qt.AlignVCenter
                        Layout.preferredHeight: Kirigami.Units.iconSizes.small
                        Layout.preferredWidth: Layout.preferredHeight
                        isMask: true
                        color: Kirigami.Theme.textColor
                        source: LayoutMirroring.enabled ? "go-next-symbolic-rtl" : "go-next-symbolic"
                    }
                    Kirigami.Heading {
                        Layout.leftMargin: Kirigami.Units.largeSpacing
                        color: Kirigami.Theme.textColor
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.NoWrap
                        text: delegateLayout.page ? delegateLayout.page.title : ""
                        opacity: modelData == pageRow.currentIndex ? 1 : 0.4
                        rightPadding: Kirigami.Units.largeSpacing
                    }
                }
            }
        }
    }

    NumberAnimation {
        id: listScrollAnim
        target: root
        property: "contentX"
        duration: Kirigami.Units.longDuration
        easing.type: Easing.InOutQuad
    }
}


