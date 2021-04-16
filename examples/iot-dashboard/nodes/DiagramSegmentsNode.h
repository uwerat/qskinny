#pragma once

#include <QPolygonF>
#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>

class IdlChartSegmentsNode : public QSGGeometryNode
{
    public:
        IdlChartSegmentsNode();

        void update( const QRectF& rect, const QColor& color, const QVector<QPointF>& dataPoints, int xGridLines );

    private:
        QSGFlatColorMaterial m_material;
        QSGGeometry m_geometry;

        QRectF m_rect;
        QColor m_color;
        QVector<QPointF> m_dataPoints;
        int m_xGridLines;
};
