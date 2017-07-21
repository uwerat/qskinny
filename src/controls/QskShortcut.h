/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SHORTCUT_H
#define QSK_SHORTCUT_H

#include "QskGlobal.h"
#include <Qt>

class QQuickWindow;
class QKeySequence;
class QObject;

namespace QskShortcut
{
    QSK_EXPORT int addShortcut( const QKeySequence&,
        QObject* receiver, const char* method, bool autoRepeat = true,
        Qt::ConnectionType type = Qt::AutoConnection );

    QSK_EXPORT int addShortcut( QQuickWindow*, const QKeySequence&,
        QObject* receiver, const char* method, bool autoRepeat = true,
        Qt::ConnectionType type = Qt::AutoConnection );

    QSK_EXPORT void setAutoRepeat( int, bool on );
}

#endif
