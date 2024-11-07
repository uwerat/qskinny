/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_BOX_CLIP_NODE_H
#define QSK_BOX_CLIP_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QskBoxShapeMetrics;
class QskBoxBorderMetrics;

class QQuickWindow;

class QSK_EXPORT QskBoxClipNode : public QSGClipNode
{
  public:
    QskBoxClipNode();
    ~QskBoxClipNode() override;

    void setBox( const QQuickWindow*, const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics& );

  private:
    QskHashValue m_hash;
    QRectF m_rect;

    QSGGeometry m_geometry;
};

#endif
