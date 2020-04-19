/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef ANCHOR_BOX_H
#define ANCHOR_BOX_H

#include "QskControl.h"

class AnchorBox : public QskControl
{
    Q_OBJECT

    using Inherited = QskControl;

  public:
    AnchorBox( QQuickItem* parent = nullptr );
    ~AnchorBox() override;

    // anchoring to the box
    void addAnchor( QQuickItem*, Qt::AnchorPoint, Qt::AnchorPoint );
    void addAnchors( QQuickItem*, Qt::Corner );
    void addAnchors( QQuickItem*, Qt::Orientations = Qt::Horizontal | Qt::Vertical );

    // anchoring between 2 children
    void addAnchor( QQuickItem*, Qt::AnchorPoint, QQuickItem*, Qt::AnchorPoint );
    void addAnchors( QQuickItem*, Qt::Corner, QQuickItem*, Qt::Corner );
    void addAnchors( QQuickItem*, QQuickItem*,
        Qt::Orientations = Qt::Horizontal | Qt::Vertical );

  protected:
    void geometryChangeEvent( QskGeometryChangeEvent* ) override;
    void updateLayout() override;

    QSizeF layoutSizeHint( Qt::SizeHint, const QSizeF& ) const override;

  private:
    void updateHints();
    void updateGeometries( const QRectF& );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif

