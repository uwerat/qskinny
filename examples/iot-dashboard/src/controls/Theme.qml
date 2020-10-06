/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.4

pragma Singleton

/**
 * A set of named colors for the application
 *
 * Since frameworks 5.38 Kirigami.Theme has been made an attached
 * property - the recommended way to interact with the Kirigami.Theme
 * object is documented in its replacement found at
 * Kirigami::PlatformTheme.
 *
 * Attempting to access the depreciated colors declared here will
 * generate warnings. Use the attached properties color sets instead.
 * For example, instead of using:
 *
 * @code
 * color: Kirigami.Theme.buttonTextColor
 * @endcode
 *
 * Use the button color set instead:
 *
 * @code
 * Kirigami.Theme.colorSet: Kirigami.Theme.Button
 * color: Kirigami.Theme.textColor
 * @endcode
 *
 * For more color sets, see Kirigami::PlatformTheme::ColorSet.
 * @inherit QtQuick.QtObject
 */
QtObject {
    id: theme

    property color textColor: "#31363b"
    property color disabledTextColor: "#9931363b"

    property color highlightColor: "#2196F3"
    property color highlightedTextColor: "#eff0fa"
    property color backgroundColor: "#eff0f1"
    property color alternateBackgroundColor: "#bdc3c7"

    property color activeTextColor: "#0176D3"
    property color activeBackgroundColor: "#0176D3"
    property color linkColor: "#2196F3"
    property color linkBackgroundColor: "#2196F3"
    property color visitedLinkColor: "#2196F3"
    property color visitedLinkBackgroundColor: "#2196F3"
    property color negativeTextColor: "#DA4453"
    property color negativeBackgroundColor: "#DA4453"
    property color neutralTextColor: "#F67400"
    property color neutralBackgroundColor: "#F67400"
    property color positiveTextColor: "#27AE60"
    property color positiveBackgroundColor: "#27AE60"

    property color buttonTextColor: "#31363b"
    property color buttonBackgroundColor: "#eff0f1"
    property color buttonAlternateBackgroundColor: "#bdc3c7"
    property color buttonHoverColor: "#2196F3"
    property color buttonFocusColor: "#2196F3"

    property color viewTextColor: "#31363b"
    property color viewBackgroundColor: "#fcfcfc"
    property color viewAlternateBackgroundColor: "#eff0f1"
    property color viewHoverColor: "#2196F3"
    property color viewFocusColor: "#2196F3"

    property color selectionTextColor: "#eff0fa"
    property color selectionBackgroundColor: "#2196F3"
    property color selectionAlternateBackgroundColor: "#1d99f3"
    property color selectionHoverColor: "#2196F3"
    property color selectionFocusColor: "#2196F3"

    property color tooltipTextColor: "#eff0f1"
    property color tooltipBackgroundColor: "#31363b"
    property color tooltipAlternateBackgroundColor: "#4d4d4d"
    property color tooltipHoverColor: "#2196F3"
    property color tooltipFocusColor: "#2196F3"

    property color complementaryTextColor: "#eff0f1"
    property color complementaryBackgroundColor: "#31363b"
    property color complementaryAlternateBackgroundColor: "#3b4045"
    property color complementaryHoverColor: "#2196F3"
    property color complementaryFocusColor: "#2196F3"

    property color headerTextColor: "#232629"
    property color headerBackgroundColor: "#e3e5e7"
    property color headerAlternateBackgroundColor: "#eff0f1"
    property color headerHoverColor: "#2196F3"
    property color headerFocusColor: "#93cee9"
    
    property font defaultFont: fontMetrics.font
    property font smallFont: {
        let font = fontMetrics.font
        if (!!font.pixelSize) {
            font.pixelSize =- 2
        } else {
            font.pointSize =- 2
        }
        return font
    }

    property list<QtObject> children: [
        TextMetrics {
            id: fontMetrics
        }
    ]
    function __propagateColorSet(object, context) {}
    function __propagateTextColor(object, color) {}
    function __propagateBackgroundColor(object, color) {}
    function __propagatePrimaryColor(object, color) {}
    function __propagateAccentColor(object, color) {}
}
