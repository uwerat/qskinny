/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LAYOUT_ENGINE_2D_H
#define QSK_LAYOUT_ENGINE_2D_H

#include "QskGlobal.h"
#include "QskLayoutChain.h"
#include "QskSizePolicy.h"

#include <qnamespace.h>
#include <memory>

class QQuickItem;
class QskLayoutHint;

class QskLayoutEngine2D
{
  public:
    QskLayoutEngine2D();
    virtual ~QskLayoutEngine2D();

    virtual int count() const = 0;

    virtual QQuickItem* itemAt( int index ) const = 0;

    int indexOf( const QQuickItem* ) const;

    int rowCount() const;
    int columnCount() const;

    bool setVisualDirection( Qt::LayoutDirection );
    Qt::LayoutDirection visualDirection() const;

    bool setDefaultAlignment( Qt::Alignment );
    Qt::Alignment defaultAlignment() const;

    bool setExtraSpacingAt( Qt::Edges edges );
    Qt::Edges extraSpacingAt() const;

    bool setSpacing( qreal spacing, Qt::Orientations );
    qreal spacing( Qt::Orientation ) const;

    qreal defaultSpacing( Qt::Orientation ) const;

    void invalidate();

    qreal widthForHeight( qreal height ) const;
    qreal heightForWidth( qreal width ) const;

    QSizeF sizeHint( Qt::SizeHint, const QSizeF& constraint ) const;

    void setGeometries( const QRectF& );

  protected:

    void layoutItem( QQuickItem*, const QRect& grid ) const;
    QskLayoutHint layoutHint( const QQuickItem*,
        Qt::Orientation, qreal constraint ) const;

    enum
    {
        ElementCache = 1 << 0,
        LayoutCache  = 1 << 1
    };

    void invalidate( int what );
    bool requiresAdjustment( const QQuickItem* ) const;

  private:
    Q_DISABLE_COPY( QskLayoutEngine2D )

    void updateSegments( const QSizeF& ) const;

    virtual void layoutItems() = 0;
    virtual int effectiveCount( Qt::Orientation ) const = 0;

    virtual void invalidateElementCache() = 0;
    QskSizePolicy::ConstraintType constraintType() const;

    void setupChain( Qt::Orientation ) const;
    void setupChain( Qt::Orientation, const QskLayoutChain::Segments& ) const;

    virtual void setupChain( Qt::Orientation,
        const QskLayoutChain::Segments&, QskLayoutChain& ) const = 0;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

inline void QskLayoutEngine2D::invalidate()
{
    invalidate( ElementCache | LayoutCache );
}

inline int QskLayoutEngine2D::rowCount() const
{
    return effectiveCount( Qt::Vertical );
}

inline int QskLayoutEngine2D::columnCount() const
{
    return effectiveCount( Qt::Horizontal );
}

#endif
