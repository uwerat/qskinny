/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_ARC_NODE_H
#define QSK_ARC_NODE_H

#include "QskPaintedNode.h"

class QskArcMetrics;
class QskGradient;

// should be a QSGGeometryNode, TODO ..

class QSK_EXPORT QskArcNode : public QskPaintedNode
{
  public:
    QskArcNode();
    ~QskArcNode() override;

    void setArcData( const QRectF&, const QskArcMetrics&,
        const QskGradient&, QQuickWindow* );

  protected:
    void paint( QPainter*, const QSize&, const void* nodeData ) override;
    QskHashValue hash( const void* nodeData ) const override;
};

#endif
