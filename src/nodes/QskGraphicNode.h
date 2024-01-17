/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
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
