/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QPolygonF>
#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>

class DiagramDataNode : public QSGGeometryNode
{
  public:
    enum Type
    {
        Line,
        Area,
    };

    DiagramDataNode();

    void update( const QRectF&, Type, const QColor&,
        const QVector< QPointF >&, const qreal yMax, bool inverted, int lineWidth );

  private:
    QSGFlatColorMaterial m_material;
    QSGGeometry m_geometry;

    QRectF m_rect;
    Type m_type;
    QColor m_color;
    QVector< QPointF > m_dataPoints;
    qreal m_yMax;
    bool m_inverted;
    int m_lineWidth;
};
