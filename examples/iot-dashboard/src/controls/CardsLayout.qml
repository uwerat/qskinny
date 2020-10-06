/*
 *  SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.6
import QtQuick.Controls 2.0 as Controls
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.4 as Kirigami

/**
 * A GridLayout optimized for showing one or two columns of cards,
 * depending on the available space.
 * It Should be used when the cards are not instantiated by a model or by a
 * model which has always very few items (In the case of a big model
 * CardsListView or CardsGridview should be used instead).
 * They are presented as a grid of two columns which will remain
 * centered if the application is really wide, or become a single
 * column if there is not enough space for two columns,
 * such as a mobile phone screen.
 * A CardsLayout should always be contained within a ColumnLayout.
 * @inherits GridLayout
 * @since 2.4
 */
GridLayout {
    /**
     * maximumColumns: int
     * The layout will never lay out the items in more columns than maximumColumns
     * Default: 2
     * @since 2.5
     */
    property int maximumColumns: 2

    /**
     * maximumColumnWidth: int
     * The maximum width the columns may have. the cards will never
     * get wider than this size, when the GridLayout is wider than
     * maximumColumnWidth, it will switch from one to two columns.
     * If the default needs to be overridden for some reason,
     * it is advised to express this unit as a multiple
     * of Kirigami.Units.gridUnit
     */
    property int maximumColumnWidth: Kirigami.Units.gridUnit * 20

    /**
     * minimumColumnWidth: int
     * The minimumWidth the columns may have. The layout will try to dispose items
     * in a number of columns that will respect this size constraint.
     * @since 2.5
     */
    property int minimumColumnWidth: Kirigami.Units.gridUnit * 12

    columns: Math.max(1, Math.min(maximumColumns > 0 ? maximumColumns : Infinity,
                                  Math.floor(width/minimumColumnWidth),
                                  Math.ceil(width/maximumColumnWidth)));

    rowSpacing: Kirigami.Units.largeSpacing * columns
    columnSpacing: Kirigami.Units.largeSpacing * columns


    //NOTE: this default width which defaults to 2 columns is just to remove a binding loop on columns
    width: maximumColumnWidth*2 + Kirigami.Units.largeSpacing
    //same computation of columns, but on the parent size
    Layout.preferredWidth: maximumColumnWidth * Math.max(1, Math.min(maximumColumns > 0 ? maximumColumns : Infinity,
                                  Math.floor(parent.width/minimumColumnWidth),
                                  Math.ceil(parent.width/maximumColumnWidth))) + Kirigami.Units.largeSpacing * (columns - 1)

    Layout.maximumWidth: Layout.preferredWidth
    Layout.alignment: Qt.AlignHCenter

    Component.onCompleted: childrenChanged()
    onChildrenChanged: {
        for (var i = 0; i < children.length; ++i) {
            children[i].Layout.fillHeight = true;
        }
    }
}
