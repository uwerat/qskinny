/*
 *  SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.6
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.12 as Kirigami

/**
 * This Component is used as the header of GlobalDrawer and as the header
 * of Card, It can be accessed there as a grouped property but can never
 * be instantiated directly
 */
Kirigami.ShadowedImage {
    id: root

    /*
     * FIXME: compatibility
     */
    property alias imageSource: root.source
    property alias iconSource: root.titleIcon

    /**
     * title: string
     * A title to be displayed on top of the image
     */
    property alias title: heading.text

    /**
     * icon: var
     * An icon to be displayed alongside the title.
     * It can be a QIcon, a fdo-compatible icon name, or any url understood by Image
     */
    property alias titleIcon: headingIcon.source

    /**
     * titleAlignment: Qt.Alignment
     */
    property int titleAlignment: Qt.AlignTop | Qt.AlignLeft

    /**
     * titleLevel: a Kirigami Heading level, default 1
     */
    property alias titleLevel: heading.level

    /**
     * wrapMode: if the header should be able to do wrapping
     */
    property alias titleWrapMode: heading.wrapMode

    property int leftPadding: headingIcon.valid ? Kirigami.Units.smallSpacing * 2 : Kirigami.Units.largeSpacing
    property int topPadding: headingIcon.valid ? Kirigami.Units.smallSpacing * 2 : Kirigami.Units.largeSpacing
    property int rightPadding: headingIcon.valid ? Kirigami.Units.smallSpacing * 2 : Kirigami.Units.largeSpacing
    property int bottomPadding: headingIcon.valid ? Kirigami.Units.smallSpacing * 2 : Kirigami.Units.largeSpacing

    Layout.fillWidth: true

    Layout.preferredWidth: titleLayout.implicitWidth || sourceSize.width
    Layout.preferredHeight: titleLayout.completed && source != "" ? width/(sourceSize.width / sourceSize.height) : Layout.minimumHeight
    Layout.minimumHeight: titleLayout.implicitHeight > 0 ? titleLayout.implicitHeight + Kirigami.Units.smallSpacing * 2 : 0
    property int implicitWidth: Layout.preferredWidth

    readonly property bool empty: bannerImage.title !== undefined && bannerImage.title.length === 0 &&
                                  bannerImage.source !== undefined && bannerImage.source.length === 0 &&
                                  bannerImage.titleIcon !== undefined &&bannerImage.titleIcon.length === 0

    fillMode: Image.PreserveAspectCrop
    asynchronous: true

    color: "transparent"

    Component.onCompleted: {
        titleLayout.completed = true;
    }

    Kirigami.ShadowedRectangle {
        anchors {
            left: parent.left
            right: parent.right
            top: (root.titleAlignment & Qt.AlignTop) ? parent.top : undefined
            bottom: (root.titleAlignment & Qt.AlignBottom) ? parent.bottom : undefined
        }
        height: Math.min(parent.height, titleLayout.height * 1.5)

        opacity: 0.5
        color: "black"

        visible: root.source != "" && root.title != "" && ((root.titleAlignment & Qt.AlignTop) || (root.titleAlignment & Qt.AlignBottom))

        corners.topLeftRadius: root.titleAlignment & Qt.AlignTop ? root.corners.topLeftRadius : 0
        corners.topRightRadius: root.titleAlignment & Qt.AlignTop ? root.corners.topRightRadius : 0
        corners.bottomLeftRadius: root.titleAlignment & Qt.AlignBottom ? root.corners.bottomLeftRadius : 0
        corners.bottomRightRadius: root.titleAlignment & Qt.AlignBottom ? root.corners.bottomRightRadius : 0
    }

    RowLayout {
        id: titleLayout
        property bool completed: false
        anchors {
            left: root.titleAlignment & Qt.AlignLeft ? parent.left : undefined
            top: root.titleAlignment & Qt.AlignTop ? parent.top : undefined
            right: root.titleAlignment & Qt.AlignRight ? parent.right : undefined
            bottom: root.titleAlignment & Qt.AlignBottom ? parent.bottom : undefined
            horizontalCenter: root.titleAlignment & Qt.AlignHCenter ? parent.horizontalCenter : undefined
            verticalCenter: root.titleAlignment & Qt.AlignVCenter ? parent.verticalCenter : undefined

            leftMargin: root.leftPadding
            topMargin: root.topPadding
            rightMargin: root.rightPadding
            bottomMargin: root.bottomPadding
        }
        width: Math.min(implicitWidth, parent.width -root.leftPadding - root.rightPadding)
        height: Math.min(implicitHeight, parent.height - root.topPadding - root.bottomPadding)
        Kirigami.Icon {
            id: headingIcon
            Layout.minimumWidth: Kirigami.Units.iconSizes.large
            Layout.minimumHeight: width
            visible: valid
            isMask: false
        }
        Kirigami.Heading {
            id: heading
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: text.length > 0
            level: 1
            color: source != "" ? "white" : Kirigami.Theme.textColor
            wrapMode: Text.NoWrap
            elide: Text.ElideRight
        }
    }
}
