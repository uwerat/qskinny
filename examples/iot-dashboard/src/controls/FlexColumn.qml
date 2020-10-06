/*
 *  SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import org.kde.kirigami 2.13 as Kirigami

/**
 * FlexColumn is a column that grows in width to a fixed cap.
 */
ColumnLayout {
    id: __outer

    default property alias columnChildren: __inner.children

    /**
     * padding: real
     *
     * The column's offset from the cross axis.
     * Note that padding is applied on both sides
     * when the column is aligned to a centered cross axis.
     */
    property real padding: Kirigami.Units.largeSpacing

    /**
     * maximumWidth: real
     *
     * How large the column can grow.
     */
    property real maximumWidth: Kirigami.Units.gridUnit * 50

    /**
     * crossAxis: Qt.Alignment
     *
     * Where the column should align when it hits its maximum width.
     */
    property int alignment: Qt.AlignHCenter | Qt.AlignTop

    Layout.fillWidth: true
    Layout.fillHeight: true

    enum CrossAxis {
        Left,
        Center,
        Right
    }

    ColumnLayout {
        id: __inner
        Layout.maximumWidth: __outer.maximumWidth
        Layout.leftMargin: __outer.alignment & Qt.AlignLeft || __outer.alignment & Qt.AlignHCenter ? __outer.padding : 0
        Layout.rightMargin: __outer.alignment & Qt.AlignRight || __outer.alignment & Qt.AlignHCenter ? __outer.padding : 0
        Layout.alignment: __outer.alignment
    }
}