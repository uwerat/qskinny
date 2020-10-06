/*
 *  SPDX-FileCopyrightText: 2012 Marco Martin <mart@kde.org>
 *  SPDX-FileCopyrightText: 2016 Aleix Pol Gonzalez <aleixpol@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.12

/**
 * A visual separator
 *
 * Useful for splitting one set of items from another.
 *
 * @inherit QtQuick.Rectangle
 */

Rectangle {
    id: root
    implicitHeight: Math.floor(Units.devicePixelRatio)
    implicitWidth: Math.floor(Units.devicePixelRatio)

    enum Weight {
        Light,
        Normal
    }

    /**
     * weight: int
     * The visual weight of the separator.
     * 
     * Weight values:
     * * Separator.Weight.Light
     * * Separator.Weight.Normal
     * 
     * default: Separator.Weight.Normal
     * 
     * @since 5.72
     * @since org.kde.kirigami 2.12
     */
    property int weight: Separator.Weight.Normal

    /* TODO: If we get a separator color role, change this to
     * mix weights lower than Normal with the background color
     * and mix weights higher than Normal with the text color.
     */
    color: ColorUtils.linearInterpolation(Theme.backgroundColor, Theme.textColor, weight == Separator.Weight.Light ? 0.1 : 0.2);
}

