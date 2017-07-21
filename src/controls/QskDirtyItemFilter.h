/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_DIRTY_ITEM_FILTER_H
#define QSK_DIRTY_ITEM_FILTER_H 1

#include "QskGlobal.h"

#include <QObject>
#include <QSet>

class QQuickWindow;
class QQuickItem;

class QSK_EXPORT QskDirtyItemFilter : public QObject
{
    using Inherited = QObject;

public:
    QskDirtyItemFilter( QObject* parent = nullptr );
    virtual ~QskDirtyItemFilter();

    void addWindow( QQuickWindow* window );

    static void filterDirtyList( QQuickWindow*,
        bool ( *isBlocked )( const QQuickItem* ) );

private:
    void beforeSynchronizing( QQuickWindow* );
    void resetBlockedDirty();
    void cleanUp( QObject* );

    QSet< QObject* > m_windows;
};

#endif
