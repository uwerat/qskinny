/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRID_LAYOUT_ENGINE_H
#define QSK_GRID_LAYOUT_ENGINE_H

#include "QskGlobal.h"
#include "QskLayoutEngine2D.h"

#include <qnamespace.h>
#include <memory>

class QQuickItem;
class QSizeF;
class QRectF;

class QskGridLayoutEngine : public QskLayoutEngine2D
{
  public:
    QskGridLayoutEngine();
    ~QskGridLayoutEngine() override;

    int count() const override final;

    bool setStretchFactor( int pos, int stretch, Qt::Orientation );
    int stretchFactor( int pos, Qt::Orientation ) const;

    bool setRowSizeHint( int row, Qt::SizeHint, qreal height );
    qreal rowSizeHint( int row, Qt::SizeHint ) const;

    bool setColumnSizeHint( int column, Qt::SizeHint, qreal width );
    qreal columnSizeHint( int column, Qt::SizeHint ) const;

    int insertItem( QQuickItem*, const QRect& grid );
    int insertSpacer( const QSizeF&, const QRect& grid );

    bool removeAt( int index );
    bool clear();

    QQuickItem* itemAt( int index ) const override final;
    QSizeF spacerAt( int index ) const;

    QQuickItem* itemAt( int row, int column ) const;
    int indexAt( int row, int column ) const;

    bool setGridAt( int index, const QRect& );
    QRect gridAt( int index ) const;

    QRect effectiveGridAt( int index ) const;

    void transpose();

  private:
    void layoutItems() override;
    int effectiveCount( Qt::Orientation ) const override;

    void invalidateElementCache() override;

    void setupChain( Qt::Orientation,
        const QskLayoutChain::Segments&, QskLayoutChain& ) const override;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
