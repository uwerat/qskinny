#ifndef DIAGRAMSEGMENTSNODE_H
#define DIAGRAMSEGMENTSNODE_H

#include <QPolygonF>
#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>

class IdlChartSegmentsNode : public QSGGeometryNode
{
    public:
        IdlChartSegmentsNode();

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
