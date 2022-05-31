/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PAINTED_NODE_H
#define QSK_PAINTED_NODE_H

#include "QskTextureRenderer.h"
#include <qsgnode.h>

class QSK_EXPORT QskPaintedNode : public QSGNode
{
  public:
    QskPaintedNode();
    ~QskPaintedNode() override;

    void update( QQuickWindow*,
        QskTextureRenderer::RenderMode, const QRectF& );

  protected:
    virtual void paint( QPainter*, const QSizeF& ) = 0;

    // a hash value of '0' always results in repainting
    virtual QskHashValue hash() const = 0;

  private:
    void updateImageNode( QQuickWindow*, const QRectF& );
    void updateImageNodeGL( QQuickWindow*, const QRectF& );

    uint32_t createTexture( QQuickWindow*, const QSize& );

    QskHashValue m_hash;
};

#endif
