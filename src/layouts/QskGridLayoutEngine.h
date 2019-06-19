/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRID_LAYOUT_ENGINE_H
#define QSK_GRID_LAYOUT_ENGINE_H

#include "QskGlobal.h"

#include <qnamespace.h>
#include <qrect.h>
#include <memory>

class QQuickItem;

class QskGridLayoutEngine
{
  public:
    QskGridLayoutEngine();
    ~QskGridLayoutEngine();

    void setGeometries( const QRectF );
    void invalidate();

    void setVisualDirection( Qt::LayoutDirection );
    Qt::LayoutDirection visualDirection() const;

    int itemCount() const;

    int rowCount() const;
    int columnCount() const;

    void insertItem( QQuickItem* item,
        int row, int column, int rowSpan, int columnSpan,
        Qt::Alignment );

    void removeAt( int index );

    int indexAt( int row, int column ) const;
    int indexOf( const QQuickItem* item ) const;

    QQuickItem* itemAt( int index ) const;
    QQuickItem* itemAt( int row, int column ) const;

    int rowOfIndex( int index ) const;
    int rowSpanOfIndex( int index ) const;

    int columnOfIndex( int index ) const;
    int columnSpanOfIndex( int index ) const;

    void setSpacing( Qt::Orientation, qreal spacing );
    qreal spacing( Qt::Orientation ) const;

    void setSpacingAt( Qt::Orientation, int cell, qreal spacing );
    qreal spacingAt( Qt::Orientation, int cell ) const;

    void setStretchFactorAt( Qt::Orientation, int cell, int stretch );
    int stretchFactorAt( Qt::Orientation, int cell );

    void setAlignmentAt( Qt::Orientation, int cell, Qt::Alignment );
    Qt::Alignment alignmentAt( Qt::Orientation, int cell ) const;

    void setAlignmentOf( const QQuickItem*, Qt::Alignment );
    Qt::Alignment alignmentOf( const QQuickItem* ) const;

    void setRetainSizeWhenHiddenOf( const QQuickItem*, bool on );
    bool retainSizeWhenHiddenOf( const QQuickItem* ) const;

    void setRowSizeHint( int row, Qt::SizeHint, qreal height );
    qreal rowSizeHint( int row, Qt::SizeHint ) const;

    void setColumnSizeHint( int column, Qt::SizeHint, qreal width );
    qreal columnSizeHint( int column, Qt::SizeHint ) const;

    QSizeF sizeHint( Qt::SizeHint, const QSizeF& constraint = QSizeF() ) const;

    qreal widthForHeight( qreal height ) const;
    qreal heightForWidth( qreal width ) const;

    static qreal defaultSpacing( Qt::Orientation );

#if 1
    QSize requiredCells() const;
    void adjustSpans( int numRows, int numColumns );
#endif

  private:
    Q_DISABLE_COPY(QskGridLayoutEngine)

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
