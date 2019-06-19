/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LINEAR_LAYOUT_ENGINE_H
#define QSK_LINEAR_LAYOUT_ENGINE_H

#include <qnamespace.h>
#include <qrect.h>
#include <memory>

class QQuickItem;

class QskLinearLayoutEngine
{
  public:
    QskLinearLayoutEngine( Qt::Orientation, uint dimension );
    ~QskLinearLayoutEngine();

    Qt::Orientation orientation() const;
    void setOrientation( Qt::Orientation );

    void setDimension( uint dimension );
    uint dimension() const;

    void setDefaultAlignment( Qt::Alignment );
    Qt::Alignment defaultAlignment() const;

    void setExtraSpacingAt( Qt::Edges );
    Qt::Edges extraSpacingAt() const;

    int count() const;

    int rowCount() const;
    int columnCount() const;

    void setSpacing( qreal spacing, Qt::Orientations );
    qreal spacing( Qt::Orientation ) const;
    qreal defaultSpacing( Qt::Orientation ) const;

    void insertItem( QQuickItem*, int index );
    void addItem( QQuickItem* );

    void insertSpacerAt( int index, qreal spacing );
    void addSpacer( qreal spacing );

    void removeAt( int index );

    QQuickItem* itemAt( int index ) const;
    int spacerAt( int index ) const;

    void setRetainSizeWhenHiddenAt( int index, bool on );
    bool retainSizeWhenHiddenAt( int index ) const;

    void setAlignmentAt( int index, Qt::Alignment );
    Qt::Alignment alignmentAt( int index ) const;

    void setStretchFactorAt( int index, int stretchFactor );
    int stretchFactorAt( int index ) const;

    void setGeometries( const QRectF& );

    qreal widthForHeight( qreal height ) const;
    qreal heightForWidth( qreal width ) const;

    QSizeF sizeHint( Qt::SizeHint, const QSizeF& contraint ) const;

    void setVisualDirection( Qt::LayoutDirection );
    Qt::LayoutDirection visualDirection() const;

    enum
    {
        EntryCache  = 1 << 0,
        CellCache   = 1 << 1,
        LayoutCache = 1 << 2
    };

    void invalidate( int what = EntryCache | CellCache | LayoutCache );

  private:
    void updateCellGeometries( const QSizeF& );

  private:
    Q_DISABLE_COPY(QskLinearLayoutEngine)

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
