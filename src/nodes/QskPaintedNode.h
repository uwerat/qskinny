/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PAINTED_NODE_H
#define QSK_PAINTED_NODE_H

#include "QskTextureNode.h"
#include "QskTextureRenderer.h"

class QSK_EXPORT QskPaintedNode : public QskTextureNode
{
  public:
    QskPaintedNode();
    ~QskPaintedNode() override;

    void update( QQuickWindow*,
        QskTextureRenderer::RenderMode, const QRect& );

  protected:
    virtual void paint( QPainter*, const QSizeF& ) = 0;

    // a hash value of '0' always results in repainting
    virtual uint hash() = 0;

  private:
    class PaintHelper;

    void setTexture( QQuickWindow*,
        const QRectF&, uint id, Qt::Orientations ) = delete;

    uint m_hash;
};

#endif
