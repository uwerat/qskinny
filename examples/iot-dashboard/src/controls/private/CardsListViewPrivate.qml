/*
 *  SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.10
import QtQuick.Controls 2.0 as Controls
import org.kde.kirigami 2.4 as Kirigami


ListView {
    id: root

    rightMargin: Kirigami.Units.largeSpacing * 2
    leftMargin: Kirigami.Units.largeSpacing * 2

    property Component _delegateComponent
    delegate: Kirigami.DelegateRecycler {
        width: root.width - root.leftMargin - root.rightMargin
        sourceComponent: root._delegateComponent
    }
}
