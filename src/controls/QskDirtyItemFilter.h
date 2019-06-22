/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_DIRTY_ITEM_FILTER_H
#define QSK_DIRTY_ITEM_FILTER_H

#include "QskGlobal.h"

#include <qobject.h>
#include <qset.h>

class QQuickWindow;
class QQuickItem;

class QskDirtyItemFilter : public QObject
{
    using Inherited = QObject;

  public:
    QskDirtyItemFilter( QObject* parent = nullptr );
    ~QskDirtyItemFilter() override;

    void addWindow( QQuickWindow* window );

    static void filterDirtyList( QQuickWindow*,
        bool ( *isBlocked )( const QQuickItem* ) );

  private:
    void beforeSynchronizing( QQuickWindow* );

    QSet< QObject* > m_windows;
};

#endif
