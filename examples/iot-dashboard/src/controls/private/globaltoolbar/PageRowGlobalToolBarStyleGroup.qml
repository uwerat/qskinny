/*
 *  SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1
import org.kde.kirigami 2.4 as Kirigami

QtObject {
    id: globalToolBar
    property int style: Kirigami.ApplicationHeaderStyle.None
    readonly property int actualStyle: {
        if (style == Kirigami.ApplicationHeaderStyle.Auto) {
            //Legacy: if ApplicationHeader or ToolbarApplicationHeader are in the header or footer, disable the toolbar here
            if (typeof applicationWindow !== "undefined" && applicationWindow().header && applicationWindow().header.toString().indexOf("ApplicationHeader") !== -1) {
                return Kirigami.ApplicationHeaderStyle.None
            }

            //non legacy logic
            return (Kirigami.Settings.isMobile
                    ? (root.wideMode ? Kirigami.ApplicationHeaderStyle.Titles : Kirigami.ApplicationHeaderStyle.Breadcrumb)
                    : Kirigami.ApplicationHeaderStyle.ToolBar)
        } else {
            //forbid ToolBar on mobile systems
            return style;
        }
    }

    property var showNavigationButtons: (style != Kirigami.ApplicationHeaderStyle.TabBar && (!Kirigami.Settings.isMobile || Qt.platform.os == "ios")) ? (Kirigami.ApplicationHeaderStyle.ShowBackButton | Kirigami.ApplicationHeaderStyle.ShowForwardButton) : Kirigami.ApplicationHeaderStyle.NoNavigationButtons
    property bool separatorVisible: true
    property int toolbarActionAlignment: Qt.AlignRight

    property int minimumHeight: 0
    // FIXME: Figure out the exact standard size of a Toolbar
    property int preferredHeight: (actualStyle == Kirigami.ApplicationHeaderStyle.ToolBar
                    ? Kirigami.Units.iconSizes.medium
                    : Kirigami.Units.gridUnit * 1.8) + Kirigami.Units.smallSpacing * 2
    property int maximumHeight: preferredHeight
}
