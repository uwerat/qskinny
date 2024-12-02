/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include "QskGlobal.h"
#include <qsgnode.h>

class QskArcMetrics;
class QskShadowMetrics;

class ArcShadowNodePrivate;

class ArcShadowNode : public QSGGeometryNode
{
  public:
    ArcShadowNode();
    ~ArcShadowNode() override;

    void setShadowData( const QRectF&, qreal spreadRadius, qreal blurRadius,
        qreal startAngle, qreal spanAngle, const QColor& );

  private:
    void setBoundingRectangle( const QRectF& );

    Q_DECLARE_PRIVATE( ArcShadowNode )
};
