/*
 *  SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import org.kde.kirigami 2.12 as Kirigami

Rectangle {
    id: tabRoot
    property bool vertical: false
    signal indexChanged(real xPos, real tabWidth)

    Keys.onPressed: {
        if (event.key == Qt.Key_Enter || event.key == Qt.Key_Return) {
            columnView.currentIndex = index
        }
    }
    activeFocusOnTab: true
    implicitWidth: vertical ? verticalTitleRow.implicitWidth : horizontalTitleRow.implicitWidth
    implicitHeight: vertical ? verticalTitleRow.implicitHeight : horizontalTitleRow.implicitHeight

    onActiveFocusChanged: {
        if (activeFocus) {
            tabRoot.indexChanged(tabRoot.x, tabRoot.width)
        }
    }
    Connections {
        target: columnView
        function onCurrentIndexChanged() {
            if (index == columnView.currentIndex) {
                tabRoot.indexChanged(tabRoot.x, tabRoot.width)
            }
        }
    }

    Accessible.name: modelData.title
    Accessible.description: {
        if (!!modelData.progress) {
            if (index == columnView.currentIndex) {
                return i18nc("Accessibility text for a page tab. Keep the text as concise as possible and don't use a percent sign.", "Current page. Progress: %1 percent.", Math.round(modelData.progress*100))
            } else {
                return i18nc("Accessibility text for a page tab. Keep the text as concise as possible.", "Navigate to %1. Progress: %1 percent.", modelData.title, Math.round(modelData.progress*100))
            }
        } else {
            if (index == columnView.currentIndex) {
                return i18nc("Accessibility text for a page tab. Keep the text as concise as possible.", "Current page.")
            } else if (modelData.needsAttention) {
                return i18nc("Accessibility text for a page tab that's requesting the user's attention. Keep the text as concise as possible.", "Navigate to %1. Demanding attention.", modelData.title)
            } else {
                return i18nc("Accessibility text for a page tab that's requesting the user's attention. Keep the text as concise as possible.", "Navigate to %1.", modelData.title)
            }
        }
    }
    Accessible.role: Accessible.PageTab
    Accessible.focusable: true
    Accessible.onPressAction: columnView.currentIndex = index

    border {
        width: activeFocus ? 2 : 0
        color: Kirigami.Theme.textColor
    }
    color: {
        if (index == columnView.currentIndex) {
            return Kirigami.ColorUtils.adjustColor(Kirigami.Theme.activeTextColor, {"alpha": 0.2*255})
        } else if (modelData.needsAttention) {
            return Kirigami.ColorUtils.adjustColor(Kirigami.Theme.negativeTextColor, {"alpha": 0.2*255})
        } else {
            return "transparent"
        }
    }

    PrivateSwipeHighlight {
        states: [
            State { name: "highlighted"; when: index == columnView.currentIndex },
            State { name: "requestingAttention"; when: modelData.needsAttention }
        ]
    }

    PrivateSwipeProgress {
        anchors.fill: parent
        visible: modelData.progress != undefined
        progress: modelData.progress
    }

    RowLayout {
        id: verticalTitleRow
        anchors.fill: parent
        Accessible.ignored: true
        visible: vertical

        ColumnLayout {
            Layout.margins: Kirigami.Settings.isMobile ? Kirigami.Units.smallSpacing : Kirigami.Units.largeSpacing
            Layout.alignment: Qt.AlignCenter

            Kirigami.Icon {
                visible: !!modelData.icon.name
                source: modelData.icon.name

                Layout.preferredHeight: swipeNavigatorRoot.big
                    ? Kirigami.Units.iconSizes.medium
                    : (Kirigami.Settings.isMobile ? Kirigami.Units.iconSizes.smallMedium : Kirigami.Units.iconSizes.small)
                Layout.preferredWidth: Layout.preferredHeight

                Layout.alignment: (Qt.AlignHCenter | Qt.AlignBottom)
            }
            Kirigami.Heading {
                level: swipeNavigatorRoot.big ? 2 : 5
                text: modelData.title
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
            }
        }
    }

    RowLayout {
        id: horizontalTitleRow
        anchors.fill: parent
        Accessible.ignored: true
        visible: !vertical

        RowLayout {
            Layout.margins: swipeNavigatorRoot.big ? Kirigami.Units.largeSpacing*2 : Kirigami.Units.largeSpacing
            Layout.alignment: Qt.AlignVCenter

            Kirigami.Icon {
                visible: !!modelData.icon.name
                source: modelData.icon.name

                Layout.preferredHeight: swipeNavigatorRoot.big
                    ? Kirigami.Units.iconSizes.medium
                    : (Kirigami.Settings.isMobile ? Kirigami.Units.iconSizes.smallMedium : Kirigami.Units.iconSizes.small)
                Layout.preferredWidth: Layout.preferredHeight

                Layout.alignment: (Qt.AlignLeft | Qt.AlignVCenter)
            }
            Kirigami.Heading {
                level: swipeNavigatorRoot.big ? 1 : 2
                text: modelData.title

                Layout.fillWidth: true
                Layout.alignment: (Qt.AlignLeft | Qt.AlignVCenter)
            }
        }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        Accessible.ignored: true
        onClicked: {
            columnView.currentIndex = index
        }
    }

    Layout.fillHeight: true
    Layout.alignment: Qt.AlignHCenter
}
