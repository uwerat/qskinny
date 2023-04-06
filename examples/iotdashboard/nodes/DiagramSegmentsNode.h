/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QPolygonF>
#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>

class DiagramSegmentsNode : public QSGGeometryNode
{
  public:
    DiagramSegmentsNode();

    void update( const QRectF&, const QColor&,
        const QVector< QVector< QPointF > >&, int xGridLines );

  private:
    QSGFlatColorMaterial m_material;
    QSGGeometry m_geometry;

    QRectF m_rect;
    QColor m_color;
    QVector< QVector< QPointF > > m_dataPoints;
    int m_xGridLines;
};
