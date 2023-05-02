/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_STROKE_NODE_H
#define QSK_STROKE_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QPen;
class QPainterPath;
class QPolygonF;

class QskStrokeNodePrivate;

class QSK_EXPORT QskStrokeNode : public QSGGeometryNode
{
  public:
    QskStrokeNode();

    /*
        We only support monochrome pens ( QPen::color() ) and using a
        QSGFlatColorMaterial is good enough. However QSGVertexColorMaterial
        usually allows better batching. So Flat vs. Colored is a tradeoff
        between memory and performance.
     */
    enum RenderHint
    {
        Flat,
        Colored
    };

    void setRenderHint( RenderHint );
    RenderHint renderHint() const;

    void updateNode( const QPainterPath&, const QTransform&, const QPen& );

    void updateNode( const QPolygonF&, const QTransform&,
        qreal lineWidth, const QColor& );

  private:
    Q_DECLARE_PRIVATE( QskStrokeNode )
};

#endif
