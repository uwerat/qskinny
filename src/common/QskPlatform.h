/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PLATFORM_H
#define QSK_PLATFORM_H

#include "QskGlobal.h"

class QScreen;
class QPlatformIntegration;
class QRect;

QSK_EXPORT qreal qskDpiScaled( qreal value );
QSK_EXPORT qreal qskGlobalScaleFactor();

QSK_EXPORT bool qskMaybeDesktopPlatform();
QSK_EXPORT const QPlatformIntegration* qskPlatformIntegration();
QSK_EXPORT QRect qskPlatformScreenGeometry( const QScreen* );

#endif
