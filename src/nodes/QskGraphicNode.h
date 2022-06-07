/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRAPHIC_NODE_H
#define QSK_GRAPHIC_NODE_H

#include "QskPaintedNode.h"

class QskGraphic;
class QskColorFilter;

class QSK_EXPORT QskGraphicNode : public QskPaintedNode
{
  public:
    QskGraphicNode();
    ~QskGraphicNode() override;

    void setGraphic( QQuickWindow*, const QskGraphic&,
        const QskColorFilter&, const QRectF& );

  private:
    virtual void paint( QPainter*, const QSize&, const void* nodeData ) override;
    virtual QskHashValue hash( const void* nodeData ) const override;
};

#endif
