/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_CLIP_NODE_H
#define QSK_CLIP_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QskBoxShapeMetrics;
class QskBoxBorderMetrics;

class QQuickWindow;

class QSK_EXPORT QskClipNode : public QSGClipNode
{
    using Inherited = QSGClipNode;

  public:
    QskClipNode();
    ~QskClipNode() override;

    void setRect( const QRectF& );
    void setRegion( const QRectF&, const QRectF& excludedRect );

    void setBox( const QQuickWindow*, const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics& );

    QRectF boundingRectangle() const;

  private:
    void reset();

    void setIsRectangular( bool ) = delete;
    void setClipRect( const QRectF& ) = delete;
    QRectF clipRect() const = delete;

    QskHashValue m_hash;
    QSGGeometry m_geometry;
};

inline QRectF QskClipNode::boundingRectangle() const
{
    return Inherited::clipRect();
}

#endif
