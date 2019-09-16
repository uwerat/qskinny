/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LINEAR_LAYOUT_ENGINE_H
#define QSK_LINEAR_LAYOUT_ENGINE_H

#include "QskGlobal.h"
#include "QskLayoutEngine2D.h"

#include <qnamespace.h>
#include <memory>

class QQuickItem;
class QSizeF;
class QRectF;

class QskLinearLayoutEngine : public QskLayoutEngine2D
{
  public:
    QskLinearLayoutEngine( Qt::Orientation, uint dimension );
    ~QskLinearLayoutEngine() override;

    Qt::Orientation orientation() const;
    bool setOrientation( Qt::Orientation );

    bool setDimension( uint dimension );
    uint dimension() const;

    int count() const override final;

    int insertItem( QQuickItem*, int index );
    int addItem( QQuickItem* );

    int insertSpacerAt( int index, qreal spacing );
    int addSpacer( qreal spacing );

    bool removeAt( int index );
    bool clear();

    QQuickItem* itemAt( int index ) const override final;
    qreal spacerAt( int index ) const;

    bool setStretchFactorAt( int index, int stretchFactor );
    int stretchFactorAt( int index ) const;

  private:
    Q_DISABLE_COPY(QskLinearLayoutEngine)

    void layoutItems() override;

    int effectiveCount() const;
    int effectiveCount( Qt::Orientation ) const override;

    void invalidateElementCache() override;

    virtual void setupChain( Qt::Orientation,
        const QskLayoutChain::Segments&, QskLayoutChain& ) const override;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

inline int QskLinearLayoutEngine::addItem( QQuickItem* item )
{
    return insertItem( item, -1 );
}

inline int QskLinearLayoutEngine::addSpacer( qreal spacing )
{
    return insertSpacerAt( -1, spacing );
}

#endif
