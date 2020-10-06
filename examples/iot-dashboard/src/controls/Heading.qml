/*
 *  SPDX-FileCopyrightText: 2012 by Sebastian Kügler <sebas@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 2.0 as QQC2
import org.kde.kirigami 2.4

/**
 * A heading label used for subsections of texts.
 *
 * The characteristics of the text will be automatically set according to the
 * plasma Theme. Use this components for section titles or headings in your UI,
 * for example page or section titles.
 *
 * Example usage:
 *
 * @code
 * import org.kde.kirigami 2.4 as Kirigami
 * [...]
 * Column {
 *     Kirigami.Heading {
 *         text: "Apples in the sunlight"
 *         level: 2
 *     }
 *   [...]
 * }
 * @endcode
 *
 * The most important property is "text", which applies to the text property of
 * Label. See PlasmaComponents Label and primitive QML Text element API for
 * additional properties, methods and signals.
 * @inherits QtQuick.Controls.Label
 */
QQC2.Label {
    id: heading

    /**
     * level: int
     * The level determines how big the section header is display, values
     * between 1 (big) and 5 (small) are accepted
     */
    property int level: 1

    /**
     * step: int
     * adjust the point size in between a level and another.
     * DEPRECATED
     */
    property int step: 0

    font.pointSize: headerPointSize(level)

    function headerPointSize(l) {
        var n = Theme.defaultFont.pointSize;
        var s;
        switch (l) {
        case 1:
            return Math.round(n * 1.50) + step;
        case 2:
            return Math.round(n * 1.30) + step;
        case 3:
            return Math.round(n * 1.20) + step;
        case 4:
            return Math.round(n * 1.10) + step;
        default:
            return n + step;
        }
    }
}
