/*
 *  SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1
import org.kde.kirigami 2.12

Item {
    property alias color: shadowRectangle.color
    property alias radius: shadowRectangle.radius
    property alias shadow: shadowRectangle.shadow
    property alias border: shadowRectangle.border
    property alias corners: shadowRectangle.corners

    property alias source: image.source
    property alias asynchronous: image.asynchronous
    property alias fillMode: image.fillMode
    property alias sourceSize: image.sourceSize

    Image {
        id: image
        anchors.fill: parent
        visible: shadowRectangle.softwareRendering
    }

    ShadowedTexture {
        id: shadowRectangle
        anchors.fill: parent

        source: image.status == Image.Ready ? image : null
    }
}
