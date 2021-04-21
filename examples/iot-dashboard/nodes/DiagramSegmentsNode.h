/******************************************************************************
 * QSkinny - Copyright (C) 2021 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef DIAGRAMSEGMENTSNODE_H
#define DIAGRAMSEGMENTSNODE_H

#include <QPolygonF>
#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>

class DiagramSegmentsNode : public QSGGeometryNode
{
    public:
        DiagramSegmentsNode();

        void update( const QRectF& rect, const QColor& color, const QVector< QVector<QPointF> >& dataPoints, int xGridLines );

    private:
        QSGFlatColorMaterial m_material;
        QSGGeometry m_geometry;

        QRectF m_rect;
        QColor m_color;
        QVector< QVector<QPointF> > m_dataPoints;
        int m_xGridLines;
};

#endif
