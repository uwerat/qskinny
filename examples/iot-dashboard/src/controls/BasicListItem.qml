/*
 *  SPDX-FileCopyrightText: 2010 Marco Martin <notmart@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0 as QQC2
import org.kde.kirigami 2.12

/**
 * An item delegate for the primitive ListView component.
 *
 * It's intended to make all listviews look coherent.
 * It has a default icon and a label
 *
 */
AbstractListItem {
    id: listItem

    /**
     * string: bool
     * A single text label the list item will contain
     */
    property alias label: listItem.text

    /**
     * A subtitle that goes below the main label
     * Optional; if not defined, the list item will only have a main label
     * @since 5.70
     * @since org.kde.kirigami 2.12
     */
    property alias subtitle: subtitleItem.text

    /**
     * bold: bool
     * Control whether the text (in both primary text and subtitle) should be rendered as bold
     * @since 5.71
     * @since org.kde.kirigami 2.13
     */
    property bool bold: false

    /**
     * icon: var
     * A single icon that will be displayed in the list item.
     * The icon can be a grouped property with name,size,color etc, as QtQuickControls2 icons are defined.
     * The icon can also be either a QIcon, a string name of a fdo compatible name,
     * or any url accepted by the Image element.
     */
    property var icon

    /**
     * iconSize: int
     * The preferred size for the icon
     * @since 2.5
     */
    property alias iconSize: iconItem.size

    /**
     * iconColor: color
     * The color the icon should be colorized to.
     * By default it will be the text color.
     * If the icon shouldn't be colorized in any way, set it to "transparent"
     * @since 2.7
     */
    property alias iconColor: iconItem.color

    /**
     * reserveSpaceForIcon: bool
     * If true, even when there is no icon the space will be reserved for it
     * It's useful in layouts where only some entries have an icon,
     * having the text all horizontally aligned
     */
    property alias reserveSpaceForIcon: iconItem.visible

    /**
     * reserveSpaceForLabel: bool
     * If false, the label will not try to be as wide as possible
     * It's useful in layouts containing entries without text
     */
    property alias reserveSpaceForLabel: labelItem.visible

    default property alias _basicDefault: layout.data

    icon: action ? action.icon.name || action.icon.source : undefined
    contentItem: RowLayout {
        id: layout
        spacing: LayoutMirroring.enabled ? listItem.rightPadding : listItem.leftPadding
        Icon {
            id: iconItem
            source: {
                if (!listItem.icon) {
                    return undefined
                }
                if (listItem.icon.hasOwnProperty) {
                    if (listItem.icon.hasOwnProperty("name") && listItem.icon.name !== "")
                        return listItem.icon.name;
                    if (listItem.icon.hasOwnProperty("source"))
                        return listItem.icon.source;
                }
                return listItem.icon;
            }
            property int size: Units.iconSizes.smallMedium
            Layout.minimumHeight: size
            Layout.maximumHeight: size
            Layout.minimumWidth: size
            selected: (listItem.highlighted || listItem.checked || (listItem.pressed && listItem.supportsMouseEvents))
            opacity: 1
            visible: source != undefined
        }
        ColumnLayout {
            spacing: 0
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            QQC2.Label {
                id: labelItem
                text: listItem.text
                Layout.fillWidth: true
                color: (listItem.highlighted || listItem.checked || (listItem.pressed && listItem.supportsMouseEvents)) ? listItem.activeTextColor : listItem.textColor
                elide: Text.ElideRight
                font.weight: listItem.bold ? Font.Bold : Font.Normal
                opacity: 1
            }
            QQC2.Label {
                id: subtitleItem
                Layout.fillWidth: true
                color: (listItem.highlighted || listItem.checked || (listItem.pressed && listItem.supportsMouseEvents)) ? listItem.activeTextColor : listItem.textColor
                elide: Text.ElideRight
                font: Theme.smallFont
                opacity: listItem.bold ? 0.9 : 0.7
                visible: text.length > 0
            }
        }
    }
}
