/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskPlatform.h"

#include <qguiapplication.h>
#include <qquickwindow.h>
#include <qscreen.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qguiapplication_p.h>
#include <private/qhighdpiscaling_p.h>
QSK_QT_PRIVATE_END

#include <qpa/qplatformintegration.h>
#include <qpa/qplatformscreen.h>

qreal qskGlobalScaleFactor()
{
    // The value of QT_SCALE_FACTOR
    const QScreen* noScreen = nullptr;
    return QHighDpiScaling::factor( noScreen );
}

const QPlatformIntegration* qskPlatformIntegration()
{
    return QGuiApplicationPrivate::platformIntegration();
}

const QPlatformTheme* qskPlatformTheme()
{
    return QGuiApplicationPrivate::platformTheme();
}

bool qskMaybeDesktopPlatform()
{
#if QT_CONFIG(cursor)
    if ( const auto platform = QGuiApplicationPrivate::platformIntegration() )
        return platform->hasCapability( QPlatformIntegration::MultipleWindows );
#endif

    return false;
}

QRect qskPlatformScreenGeometry( const QScreen* screen )
{
    if ( screen == nullptr )
        return QRect();

    return screen->handle()->geometry();
}

static inline qreal qskWindowDpi( const QWindow* window )
{
    QScreen* screen = nullptr;
    if ( window )
        screen = window->screen();

    if ( screen == nullptr )
        screen = QGuiApplication::primaryScreen();

    return QHighDpiScaling::logicalDpi( screen ).first;
}

qreal qskInchesToPixels( const QQuickWindow* window, qreal inches )
{
    return qskWindowDpi( window ) * inches;
}

qreal qskMMToPixels( const QQuickWindow* window, qreal mm )
{
    return qskWindowDpi( window ) * mm / 25.4;
}
