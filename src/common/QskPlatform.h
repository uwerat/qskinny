/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_PLATFORM_H
#define QSK_PLATFORM_H

#include "QskGlobal.h"

class QScreen;
class QPlatformIntegration;
class QPlatformTheme;
class QRect;
class QQuickWindow;

QSK_EXPORT qreal qskGlobalScaleFactor();

QSK_EXPORT bool qskMaybeDesktopPlatform();
QSK_EXPORT QRect qskPlatformScreenGeometry( const QScreen* );

QSK_EXPORT const QPlatformIntegration* qskPlatformIntegration();
QSK_EXPORT const QPlatformTheme* qskPlatformTheme();

QSK_EXPORT qreal qskInchesToPixels( const QQuickWindow*, qreal mm );
QSK_EXPORT qreal qskMMToPixels( const QQuickWindow*, qreal mm );

#endif
