/*
 *  SPDX-FileCopyrightText: 2010 Marco Martin <notmart@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.4
//NOTE: This must stay at 2.0 until KF6 due to retrocompatibility of the "icon" property
import QtQuick.Templates 2.0 as T2
import QtQuick.Templates 2.4 as QQC2

/**
 * An item delegate for the primitive ListView component.
 *
 * It's intended to make all listviews look coherent.
 *
 * @inherit QtQuick.Templates.ItemDelegate
 */
T2.ItemDelegate {
    id: listItem

    /**
     * supportsMouseEvents: bool
     * Holds if the item emits signals related to mouse interaction.
     *TODO: remove
     * The default value is false.
     */
    property bool supportsMouseEvents: hoverEnabled

    /**
     * containsMouse: bool
     * True when the user hovers the mouse over the list item
     * NOTE: on mobile touch devices this will be true only when pressed is also true
     * TODO: remove?
     */
    property alias containsMouse: listItem.hovered

    /**
     * alternatingBackground: bool
     * If true the background of the list items will be alternating between two
     * colors, helping readability with multiple column views.
     * Use it only when implementing a view which shows data visually in multiple columns
     * @ since 2.7
     */
    property bool alternatingBackground: false

    /**
     * sectionDelegate: bool
     * If true the item will be a delegate for a section, so will look like a
     * "title" for the items under it.
     */
    property bool sectionDelegate: false

    /**
     * separatorVisible: bool
     * True if the separator between items is visible
     * default: true
     */
    property bool separatorVisible: true

    /**
     * textColor: color
     * Color for the text in the item
     * It is advised to leave the default value (Theme.textColor)
     *
     * Note: if custom text elements are inserted in an AbstractListItem,
     * their color property will have to be manually bound with this property
     */
    property color textColor: Theme.textColor

    /**
     * backgroundColor: color
     * Color for the background of the item
     * It is advised to leave the default value (Theme.viewBackgroundColor)
     */
    property color backgroundColor: "transparent"

    /**
     * alternateBackgroundColor: color
     * The background color to use if alternatingBackground is true.
     * It is advised to leave the default.
     * @since 2.7
     */
    property color alternateBackgroundColor: Theme.alternateBackgroundColor

    /**
     * activeTextColor: color
     * Color for the text in the item when pressed or selected
     * It is advised to leave the default value (Theme.highlightedTextColor)
     *
     * Note: if custom text elements are inserted in an AbstractListItem,
     * their color property will have to be manually bound with this property
     */
    property color activeTextColor: Theme.highlightedTextColor

    /**
     * activeBackgroundColor: color
     * Color for the background of the item when pressed or selected
     * It is advised to leave the default value (Theme.highlightColor)
     */
    property color activeBackgroundColor: Theme.highlightColor

    default property alias _default: listItem.contentItem

    // Overrides action property of newer import versions which we can't use
    property QQC2.Action action

    activeFocusOnTab: ListView.view ? false : true

    text: action ? action.text : undefined
    checked: action ? action.checked : false
    checkable: action ? action.checkable : false
    onClicked: {
        if (ListView.view && typeof index !== "undefined") {
            ListView.view.currentIndex = index;
        }
        if (!action) {
            return;
        }

        action.trigger();
        checked = Qt.binding(function() { return action.checked });
    }
    //Theme.inherit: false
   //Theme.colorSet: Theme.View

    padding: Settings.tabletMode ? Units.largeSpacing : Units.smallSpacing

    leftPadding: padding*2
    topPadding: padding

    rightPadding: padding*2
    bottomPadding: padding

    implicitWidth: contentItem ? contentItem.implicitWidth + leftPadding + rightPadding : Units.gridUnit * 12

    implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding

    width: parent && parent.width > 0 ? parent.width : implicitWidth
    Layout.fillWidth: true

    opacity: enabled ? 1 : 0.6

    height: visible ? implicitHeight : 0

    hoverEnabled: true

    QtObject {
        id: internal
        property Flickable view: listItem.ListView.view || (listItem.parent ? listItem.parent.ListView.view : null)
        property bool indicateActiveFocus: listItem.pressed || Settings.tabletMode || listItem.activeFocus || (view ? view.activeFocus : false)
    }

    Accessible.role: Accessible.ListItem
    highlighted: focus && ListView.isCurrentItem && ListView.view && ListView.view.keyNavigationEnabled
}
