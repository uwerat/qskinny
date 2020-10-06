/*
 *  SPDX-FileCopyrightText: 2018 Aleix Pol Gonzalez <aleixpol@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.3
import QtQuick.Controls 2.3 as Controls
import org.kde.kirigami 2.4 as Kirigami

Controls.MenuItem {
    id: menuItem

    visible: action.visible === undefined || action.visible
    height: visible ? implicitHeight : 0
    autoExclusive: action.Controls.ActionGroup && action.Controls.ActionGroup.group && action.Controls.ActionGroup.group.exclusive

    Controls.ToolTip.text: action.tooltip || ""
    Controls.ToolTip.visible: menuItem.hovered && Controls.ToolTip.text.length>0
    Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
}
