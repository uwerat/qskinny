/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_CLIP_NODE_H
#define QSK_BOX_CLIP_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QskBoxShapeMetrics;
class QskBoxBorderMetrics;

class QSK_EXPORT QskBoxClipNode : public QSGClipNode
{
  public:
    QskBoxClipNode();
    ~QskBoxClipNode() override;

    void setBox( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics& );

  private:
    uint m_hash;
    QRectF m_rect;

    QSGGeometry m_geometry;
};

#endif
