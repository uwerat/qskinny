#pragma once

#include <QskNamespace.h>

#include <QPolygonF>
#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>

class IdlChartNode : public QSGGeometryNode
{
    public:
        enum Type
        {
            Line,
            Area,
        };

        IdlChartNode();

        void update( const QRectF& rect, Type type, const QColor& color, const QVector<QPointF>& dataPoints, const qreal yMax, Qsk::Position position, int lineWidth );

    private:
        QSGFlatColorMaterial m_material;
        QSGGeometry m_geometry;

        QRectF m_rect;
        Type m_type;
        QColor m_color;
        QVector<QPointF> m_dataPoints;
        qreal m_yMax;
        Qsk::Position m_position;
        int m_lineWidth;
};
