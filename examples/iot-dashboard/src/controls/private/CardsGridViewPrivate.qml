/*
 *  SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.10
import QtQuick.Controls 2.0 as Controls
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.4 as Kirigami


GridView {
    id: root

    property Component _delegateComponent
    delegate: Kirigami.DelegateRecycler {
        width: Math.min(root.cellWidth, root.maximumColumnWidth) - Kirigami.Units.largeSpacing * 2

        //in grid views align the cells in the middle
        anchors.left: parent.left
        anchors.leftMargin: (width + Kirigami.Units.largeSpacing*2) * (index % root.columns ) + root.width/2 - (root.columns*(width + Kirigami.Units.largeSpacing*2))/2

        sourceComponent: root._delegateComponent
    }
}
