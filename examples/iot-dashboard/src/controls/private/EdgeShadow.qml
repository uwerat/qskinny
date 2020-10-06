/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.4

LinearGradient {
    id: shadow
    /**
     * edge: enumeration
     * This property holds the edge of the shadow that will determine
     * the direction of the gradient.
     * The acceptable values are:
     * Qt.TopEdge: The top edge of the content item.
     * Qt.LeftEdge: The left edge of the content item (default).
     * Qt.RightEdge: The right edge of the content item.
     * Qt.BottomEdge: The bottom edge of the content item.
     */
    property int edge: Qt.LeftEdge

    property int radius: Units.gridUnit
    implicitWidth: radius
    implicitHeight: radius

    start: Qt.point((edge !== Qt.RightEdge ? 0 : width), (edge !== Qt.BottomEdge ? 0 : height))
    end: Qt.point((edge !== Qt.LeftEdge ? 0 : width), (edge !== Qt.TopEdge ? 0 : height))
    gradient: Gradient {
        GradientStop {
            position: 0.0
            color: Qt.rgba(0, 0, 0, 0.25)
        }
        GradientStop {
            position: 0.20
            color: Qt.rgba(0, 0, 0, 0.1)
        }
        GradientStop {
            position: 0.35
            color: Qt.rgba(0, 0, 0, 0.02)
        }
        GradientStop {
            position: 1.0
            color:  "transparent"
        }
    }
}

