/*
 *  SPDX-FileCopyrightText: 2017 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.5
import QtQuick.Templates 2.0 as T2
import org.kde.kirigami 2.4 as Kirigami

/**
 * An item that can be used as an header for a ListView.
 * It will play nice with the margin policies of ScrollablePage and can
 * automatically shrink when the list is scrolled, like the behavior
 * of list headers in many mobile applications.
 * @since 2.1
 * @inherit QtQuick.Controls.Control
 */
T2.Control {
    property int minimumHeight: Kirigami.Units.gridUnit * 2 + Kirigami.Units.smallSpacing * 2
    property int maximumHeight: Kirigami.Units.gridUnit * 6

    property ListView view: ListView.view

    width: view.width

    implicitHeight: topPadding + bottomPadding + (view.headerPositioning === ListView.InlineHeader
                                                    ? maximumHeight
                                                    : Math.min(maximumHeight, Math.max(minimumHeight, maximumHeight - Math.max(0, view.contentY))))


    z: 9
    topPadding: applicationWindow() && !applicationWindow().wideScreen && applicationWindow().header ? applicationWindow().header.paintedHeight : 0
    rightPadding: Kirigami.Units.gridUnit

}
