/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRAPHIC_NODE_H
#define QSK_GRAPHIC_NODE_H

#include "QskTextureRenderer.h"
#include "QskTextureNode.h"

class QskGraphic;
class QskColorFilter;
class QQuickWindow;

class QSK_EXPORT QskGraphicNode : public QskTextureNode
{
  public:
    QskGraphicNode();
    ~QskGraphicNode() override;

    void setGraphic( QQuickWindow*,
        const QskGraphic&, const QskColorFilter&,
        QskTextureRenderer::RenderMode, const QRectF&,
        Qt::Orientations mirrored = Qt::Orientations() );

  private:
    void setTexture( QQuickWindow*,
        const QRectF&, uint id, Qt::Orientations ) = delete;

    uint m_hash;
};

#endif
