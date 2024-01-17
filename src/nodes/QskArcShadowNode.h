/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_ARC_SHADOW_NODE_H
#define QSK_ARC_SHADOW_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

class QskArcMetrics;
class QskShadowMetrics;

class QskArcShadowNodePrivate;

class QskArcShadowNode : public QSGGeometryNode
{
  public:
    QskArcShadowNode();
    ~QskArcShadowNode() override;

    void setShadowData( const QRectF&, qreal spreadRadius, qreal blurRadius,
        qreal startAngle, qreal spanAngle, const QColor& );

  private:
    void setBoundingRectangle( const QRectF& );

    Q_DECLARE_PRIVATE( QskArcShadowNode )
};

#endif
