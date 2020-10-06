/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.4

RadialGradient {
    id: shadow
    /**
     * corner: enumeration
     * This property holds the corner of the shadow that will determine
     * the direction of the gradient.
     * The acceptable values are:
     * Qt.TopLeftCorner, TopRightCorner, BottomLeftCorner, BottomRightCorner
     */
    property int corner: Qt.TopRightCorner

    readonly property real margin: -Math.floor(radius/3)

    property int radius: Units.gridUnit

    width: radius - margin
    height: radius - margin

    horizontalRadius: width
    verticalRadius: height
    horizontalOffset: {
        switch (corner) {
            case Qt.TopLeftCorner:
            case Qt.BottomLeftCorner:
                return -width/2;
            default:
                return width/2;
        }
    }
    verticalOffset: {
        switch (corner) {
            case Qt.TopLeftCorner:
            case Qt.TopRightCorner:
                return -width/2;
            default:
                return width/2;
        }
    }

    gradient: Gradient {
        GradientStop {
            position: 0.0
            color: Qt.rgba(0, 0, 0, 0.25)
        }
        GradientStop {
            position: 1 - radius/(radius - margin)
            color: Qt.rgba(0, 0, 0, 0.25)
        }
        GradientStop {
            position: 1 - radius/(radius - margin) + radius/(radius - margin) * 0.2
            color: Qt.rgba(0, 0, 0, 0.1)
        }
        GradientStop {
            position: 1 - radius/(radius - margin) + radius/(radius - margin) * 0.35
            color: Qt.rgba(0, 0, 0, 0.02)
        }
        GradientStop {
            position: 1.0
            color:  "transparent"
        }
    }
}

