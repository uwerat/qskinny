/*
 *  SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.10
import QtQuick.Controls 2.0 as Controls
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.4 as Kirigami
import "private"
/**
 * CardsListView is a ListView which can have AbstractCard as its delegate: it will
 * automatically assign the proper spacings and margins around the cards adhering
 * to the design guidelines.
 * CardsListView should be used only with cards which can look good at any
 * horizontal size, so It is recommended to use directly AbstractCard with an
 * appropriate layout inside, because they are stretching for the whole list width.
 * Therefore is discouraged to use it with the Card type, unless it has
 * Horizontal as headerOrientation.
 * The choice between using this view with AbstractCard or a normal ListView
 * with AbstractListItem/BasicListItem is purely a choice based on aesthetics alone.
 * It is discouraged to tweak the properties of this ListView.
 * @inherits ListView
 * @since 2.4
 */
CardsListViewPrivate {
    id: root
    spacing: Kirigami.Units.largeSpacing * 2
    topMargin: headerPositioning != ListView.InlineHeader ? spacing : 0

    property alias delegate: root._delegateComponent
    headerPositioning: ListView.OverlayHeader
}
