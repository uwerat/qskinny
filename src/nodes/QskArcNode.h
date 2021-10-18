/**********************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_ARC_NODE_H
#define QSK_ARC_NODE_H

#include "QskArcBorderMetrics.h"
#include "QskArcMetrics.h"
#include "QskGradient.h"
#include "QskPaintedNode.h"

class QSK_EXPORT QskArcNode : public QskPaintedNode
{
  public:
    QskArcNode();
    ~QskArcNode() override;

    void setArcData( const QRectF&, const QskArcMetrics&,
        const QskArcBorderMetrics&, const QskGradient&, QQuickWindow* );

    void paint( QPainter* painter, const QSizeF& size ) override;
    uint hash() const override;

  private:
    QskArcMetrics m_metrics;
    QskArcBorderMetrics m_borderMetrics;
    QskGradient m_gradient;
};

#endif
