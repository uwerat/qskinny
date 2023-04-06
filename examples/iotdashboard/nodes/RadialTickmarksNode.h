/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskIntervalF.h>

#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>

class QskArcMetrics;
class QskScaleTickmarks;

class RadialTickmarksNode : public QSGGeometryNode
{
  public:
    RadialTickmarksNode();
    ~RadialTickmarksNode() override;

    void update( const QColor&, const QRectF&,
        const QskArcMetrics&, const QskScaleTickmarks&, int lineWidth );

  private:
    QSGGeometry m_geometry;
    QSGFlatColorMaterial m_material;

    QRectF m_rect;
    int m_lineWidth = 0;

    QskHashValue m_hash = 0;
};
