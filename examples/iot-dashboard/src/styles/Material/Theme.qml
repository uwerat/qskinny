/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls.Material 2.0
import org.kde.kirigami 2.4 as Kirigami

pragma Singleton


QtObject {
    id: theme
    //NOTE: this is useless per se, but it forces the Material attached property to be created
    Material.elevation:2

    property color textColor: theme.Material.foreground
    onTextColorChanged: theme.Material.foreground = textColor
    property color disabledTextColor: "#9931363b"

    property color highlightColor: theme.Material.accent
    onHighlightColorChanged: theme.Material.accent = highlightColor
    //FIXME: something better?
    property color highlightedTextColor: theme.Material.background
    property color backgroundColor: theme.Material.background
    property color alternateBackgroundColor: Qt.darker(theme.Material.background, 1.05)
    property color activeTextColor: theme.Material.primary
    property color activeBackgroundColor: theme.Material.primary
    property color linkColor: "#2980B9"
    property color linkBackgroundColor: "#2980B9"
    property color visitedLinkColor: "#7F8C8D"
    property color visitedLinkBackgroundColor: "#7F8C8D"
    property color hoverColor: theme.Material.highlightedButtonColor
    property color focusColor: theme.Material.highlightedButtonColor
    property color negativeTextColor: "#DA4453"
    property color negativeBackgroundColor: "#DA4453"
    property color neutralTextColor: "#F67400"
    property color neutralBackgroundColor: "#F67400"
    property color positiveTextColor: "#27AE60"
    property color positiveBackgroundColor: "#27AE60"

    property color buttonTextColor: theme.Material.foreground
    property color buttonBackgroundColor: theme.Material.buttonColor
    property color buttonAlternateBackgroundColor: Qt.darker(theme.Material.buttonColor, 1.05)
    property color buttonHoverColor: theme.Material.highlightedButtonColor
    property color buttonFocusColor: theme.Material.highlightedButtonColor

    property color viewTextColor: theme.Material.foreground
    property color viewBackgroundColor: theme.Material.dialogColor
    property color viewAlternateBackgroundColor: Qt.darker(theme.Material.dialogColor, 1.05)
    property color viewHoverColor: theme.Material.listHighlightColor
    property color viewFocusColor: theme.Material.listHighlightColor

    property color selectionTextColor: theme.Material.primaryHighlightedTextColor
    property color selectionBackgroundColor: theme.Material.textSelectionColor
    property color selectionAlternateBackgroundColor: Qt.darker(theme.Material.textSelectionColor, 1.05)
    property color selectionHoverColor: theme.Material.highlightedButtonColor
    property color selectionFocusColor: theme.Material.highlightedButtonColor

    property color tooltipTextColor: fontMetrics.Material.foreground
    property color tooltipBackgroundColor: fontMetrics.Material.tooltipColor
    property color tooltipAlternateBackgroundColor: Qt.darker(theme.Material.tooltipColor, 1.05)
    property color tooltipHoverColor: fontMetrics.Material.highlightedButtonColor
    property color tooltipFocusColor: fontMetrics.Material.highlightedButtonColor

    property color complementaryTextColor: fontMetrics.Material.foreground
    property color complementaryBackgroundColor: fontMetrics.Material.background
    property color complementaryAlternateBackgroundColor: Qt.lighter(fontMetrics.Material.background, 1.05)
    property color complementaryHoverColor: theme.Material.highlightedButtonColor
    property color complementaryFocusColor: theme.Material.highlightedButtonColor

    property color headerTextColor: fontMetrics.Material.primaryTextColor
    property color headerBackgroundColor: fontMetrics.Material.primaryColor
    property color headerAlternateBackgroundColor: Qt.lighter(fontMetrics.Material.primaryColor, 1.05)
    property color headerHoverColor: theme.Material.highlightedButtonColor
    property color headerFocusColor: theme.Material.highlightedButtonColor

    property font defaultFont: fontMetrics.font

    property list<QtObject> children: [
        TextMetrics {
            id: fontMetrics
            //this is to get a source of dark colors
            Material.theme: Material.Dark
        }
    ]
    //for internal use
    function __propagateColorSet(object, context) {
        //TODO: actually check if it's a dark or light color
        if (context === Kirigami.Theme.Complementary) {
            object.Material.theme = Material.Dark;
        } else {
            object.Material.theme = Material.Light;
        }
    }
    function __propagateTextColor(object, color) {
        object.Material.foreground = color;
    }
    function __propagateBackgroundColor(object, color) {
        object.Material.background = color;
    }
    function __propagatePrimaryColor(object, color) {
        object.Material.primary = color;
    }
    function __propagateAccentColor(object, color) {
        object.Material.accent = color;
    }
}
