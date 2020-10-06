/*
 *  SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.6
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.12 as Kirigami
import "private"

/**
 * This is the standard layout of a Card.
 * It is recommended to use this class when the concept of Cards is needed
 * in the application.
 * This Card has default items as header and footer. The header is an
 * image that can contain an optional title and icon, accessible via the
 * banner grouped property.
 * The footer will show a series of toolbuttons (and eventual overflow menu)
 * representing the actions list accessible with the list property actions.
 * It is possible even tough is discouraged to override the footer:
 * in this case the actions property shouldn't be used.
 *
 * @inherits AbstractCard
 * @since 2.4
 */
Kirigami.AbstractCard {
    id: root

    /**
     * actions: list<Action>
     * if the card should provide clickable actions, put them in this property,
     * they will be put in the footer as a list of ToolButtons plus an optional
     * overflow menu, when not all of them will fit in the available Card width.
     */
    property list<QtObject> actions

    /**
    * actions: hiddenActions<Action>
    * This list of actions is for those you always want in the menu, even if there
    * is enough space.
    * @since 2.6
    */
    property alias hiddenActions: actionsToolBar.hiddenActions

    /**
     * banner: Image
     * Gropuped property to control the banner image present in the header, it
     * has the following sub properties:
     * * url source: the source for the image, it understands any url
     *                    valid for an Image component
     * * string title: the title for the banner, shown as contrasting
     *                 text over the image
     * * Qt.Alignment titleAlignment: the alignment of the title inside the image,
     *                           a combination of flags is supported
     *                           (default: Qt.AlignTop | Qt.AlignLeft)
     * * string titleIcon: the optional icon to put in the banner:
     *                      it can be either a freedesktop-compatible icon name (recommended)
     *                      or any url supported by Image
     * * titleLevel: The Kirigami Heading level for the title, it controls the font size, default 1
     * * wrapMode: if the header should be able to do wrapping
     *
     * It also has the full set of properties a QML Image has, such as sourceSize and fillMode
     */
    readonly property alias banner: bannerImage

    header: BannerImage {
        id: bannerImage
        anchors.leftMargin: -root.leftPadding + root.background.border.width
        anchors.topMargin: -root.topPadding + root.background.border.width
        anchors.rightMargin: root.headerOrientation == Qt.Vertical ? -root.rightPadding + root.background.border.width : 0
        anchors.bottomMargin: root.headerOrientation == Qt.Horizontal ? -root.bottomPadding + root.background.border.width : 0
        //height: Layout.preferredHeight
        implicitWidth: root.headerOrientation == Qt.Horizontal ? sourceSize.width : Layout.preferredWidth
        Layout.preferredHeight: (source != "" ? width / (sourceSize.width / sourceSize.height) : Layout.minimumHeight) + anchors.topMargin + anchors.bottomMargin

        readonly property real widthWithBorder: width + root.background.border.width * 2
        readonly property real heightWithBorder: height + root.background.border.width * 2
        readonly property real radiusFromBackground: root.background.radius - root.background.border.width

        corners.topLeftRadius: radiusFromBackground
        corners.topRightRadius: (root.headerOrientation == Qt.Horizontal && widthWithBorder < root.width) ? 0 : radiusFromBackground
        corners.bottomLeftRadius: (root.headerOrientation != Qt.Horizontal && heightWithBorder < root.height) ? 0 : radiusFromBackground
        corners.bottomRightRadius: heightWithBorder < root.height ? 0 : radiusFromBackground
    }

    onHeaderChanged: {
        if (!header) {
            return;
        }

        header.anchors.leftMargin = Qt.binding(function() {return -root.leftPadding});
        header.anchors.topMargin = Qt.binding(function() {return  -root.topPadding});
        header.anchors.rightMargin = Qt.binding(function() {return root.headerOrientation == Qt.Vertical ? -root.rightPadding : 0});
        header.anchors.bottomMargin = Qt.binding(function() {return root.headerOrientation == Qt.Horizontal ? -root.bottomPadding : 0});
    }

    footer: Kirigami.ActionToolBar {
        id: actionsToolBar
        actions: root.actions
        position: Controls.ToolBar.Footer
        visible: root.footer == actionsToolBar
    }
}
