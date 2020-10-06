/*
 *  SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.5
import org.kde.kirigami 2.12 as Kirigami

/**
 * An org::kde::kirigami::ApplicationWindow with a preconfigured PageRouter.
 *
 * In order to call functions on the PageRouter, use @link PageRouterAttached  the attached Kirigami.PageRouter object @endlink.
 */

Kirigami.ApplicationWindow {
    id: __kirigamiApplicationWindow

    /**
     * @see PageRouter::routes
     */
    default property alias routes: __kirigamiPageRouter.routes

    /**
     * @see PageRouter::initialRoute
     */
    property alias initialRoute: __kirigamiPageRouter.initialRoute

    /**
     * The PageRouter of this window.
     */
    property alias router: __kirigamiPageRouter

    Kirigami.PageRouter {
        id: __kirigamiPageRouter
        pageStack: __kirigamiApplicationWindow.pageStack.columnView
    }
}