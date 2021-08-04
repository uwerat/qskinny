/******************************************************************************
 * QSkinny - Copyright (C) 2021 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#pragma once

#include <QskNamespace.h>

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
        const QVector< QPointF >&, const qreal yMax, Qsk::Position, int lineWidth );

  private:
    QSGFlatColorMaterial m_material;
    QSGGeometry m_geometry;

    QRectF m_rect;
    Type m_type;
    QColor m_color;
    QVector< QPointF > m_dataPoints;
    qreal m_yMax;
    Qsk::Position m_position;
    int m_lineWidth;
};
