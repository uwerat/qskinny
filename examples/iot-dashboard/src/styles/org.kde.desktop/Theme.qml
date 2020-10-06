/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.4

pragma Singleton


QtObject {
    id: theme

    property color textColor: palette.windowText
    property color disabledTextColor: disabledPalette.windowText

    property color highlightColor: palette.highlight
    property color highlightedTextColor: palette.highlightedText
    property color backgroundColor: palette.window
    property color alternateBackgroundColor: Qt.darker(palette.window, 1.05)
    property color activeTextColor: palette.highlight
    property color activeBackgroundColor: palette.highlight
    property color linkColor: "#2980B9"
    property color linkBackgroundColor: "#2980B9"
    property color visitedLinkColor: "#7F8C8D"
    property color visitedLinkBackgroundColor: "#7F8C8D"
    property color hoverColor: palette.highlight
    property color focusColor: palette.highlight
    property color negativeTextColor: "#DA4453"
    property color negativeBackgroundColor: "#DA4453"
    property color neutralTextColor: "#F67400"
    property color neutralBackgroundColor: "#F67400"
    property color positiveTextColor: "#27AE60"
    property color positiveBackgroundColor: "#27AE60"

    property color buttonTextColor: palette.buttonText
    property color buttonBackgroundColor: palette.button
    property color buttonAlternateBackgroundColor: Qt.darker(palette.button, 1.05)
    property color buttonHoverColor: palette.highlight
    property color buttonFocusColor: palette.highlight

    property color viewTextColor: palette.text
    property color viewBackgroundColor: palette.base
    property color viewAlternateBackgroundColor: palette.alternateBase
    property color viewHoverColor: palette.highlight
    property color viewFocusColor: palette.highlight

    property color selectionTextColor: palette.highlightedText
    property color selectionBackgroundColor: palette.highlight
    property color selectionAlternateBackgroundColor: Qt.darker(palette.highlight, 1.05)
    property color selectionHoverColor: palette.highlight
    property color selectionFocusColor: palette.highlight

    property color tooltipTextColor: palette.base
    property color tooltipBackgroundColor: palette.text
    property color tooltipAlternateBackgroundColor: Qt.darker(palette.text, 1.05)
    property color tooltipHoverColor: palette.highlight
    property color tooltipFocusColor: palette.highlight

    property color complementaryTextColor: palette.base
    property color complementaryBackgroundColor: palette.text
    property color complementaryAlternateBackgroundColor: Qt.darker(palette.text, 1.05)
    property color complementaryHoverColor: palette.highlight
    property color complementaryFocusColor: palette.highlight

    property color headerTextColor: palette.text
    property color headerBackgroundColor: palette.base
    property color headerAlternateBackgroundColor: palette.alternateBase
    property color headerHoverColor: palette.highlight
    property color headerFocusColor: palette.highlight

    property font defaultFont: fontMetrics.font

    property list<QtObject> children: [
        TextMetrics {
            id: fontMetrics
        },
        SystemPalette {
            id: palette
            colorGroup: SystemPalette.Active
        },
        SystemPalette {
            id: disabledPalette
            colorGroup: SystemPalette.Disabled
        }
    ]

    function __propagateColorSet(object, context) {}

    function __propagateTextColor(object, color) {}
    function __propagateBackgroundColor(object, color) {}
    function __propagatePrimaryColor(object, color) {}
    function __propagateAccentColor(object, color) {}
}
