/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LAYOUT_ENGINE_H
#define QSK_LAYOUT_ENGINE_H

#include "QskGlobal.h"

QSK_QT_PRIVATE_BEGIN
/*
    QskLayoutEngine.h should be hidden into some cpp file
    as it needs private headers. TODO
 */

#include <private/qgridlayoutengine_p.h>
QSK_QT_PRIVATE_END

class QskLayoutItem;
class QQuickItem;

class QskLayoutEngine : public QGridLayoutEngine
{
  public:
    QskLayoutEngine();
    ~QskLayoutEngine();

    void setGeometries( const QRectF );

    void insertLayoutItem( QskLayoutItem* item, int index );

    QskLayoutItem* layoutItemAt( int index ) const;
    QskLayoutItem* layoutItemAt( int row, int column ) const;
    QskLayoutItem* layoutItemAt( int row, int column, Qt::Orientation ) const;

    int indexAt( int row, int column ) const;

    QskLayoutItem* layoutItemOf( const QQuickItem* ) const;
    int indexOf( const QQuickItem* item ) const;

    QSizeF sizeHint( Qt::SizeHint, const QSizeF& constraint = QSizeF() ) const;

    qreal spacing( Qt::Orientation ) const;
    static qreal defaultSpacing( Qt::Orientation );

#if 1
    QSize requiredCells() const;
    void adjustSpans( int numRows, int numColumns );
#endif
};

inline QskLayoutItem* QskLayoutEngine::layoutItemOf( const QQuickItem* item ) const
{
    return layoutItemAt( indexOf( item ) );
}

#endif
