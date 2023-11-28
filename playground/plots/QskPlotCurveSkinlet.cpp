/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskPlotCurveSkinlet.h"
#include "QskPlotCurveData.h"
#include "QskPlotCurve.h"

#include <QskSGNode.h>
#include <QskVertex.h>

#include <qsggeometry.h>
#include <qsgvertexcolormaterial.h>

namespace
{
    class CurveNode : public QSGGeometryNode
    {
      public:
        CurveNode()
            : m_geometry( QSGGeometry::defaultAttributes_ColoredPoint2D(), 0 )
        {
            setGeometry( &m_geometry );
            setMaterial( &m_material );
        }

        void updateCurve( const QRectF& scaleRect, const QskPlotCurveData* data,
            const QColor& color, qreal lineWidth )
        {
            m_geometry.setDrawingMode( QSGGeometry::DrawLineStrip );

            const float lineWidthF = lineWidth;
            if( lineWidthF != m_geometry.lineWidth() )
                m_geometry.setLineWidth( lineWidthF );

            const QskVertex::Color c( color );

            int from = 0;
            int to = data->count() - 1;

            auto point1 = data->pointAt( from );
            auto point2 = data->pointAt( to );

            if ( data->hints() & QskPlotCurveData::MonotonicX )
            {
                const qreal x1 = scaleRect.left();
                const qreal x2 = scaleRect.right();

                if ( x1 > point2.x() || x2 < point1.x() )
                {
                    QskSGNode::resetGeometry( this );
                    return;
                }

                const int index1 = data->upperIndex( Qt::Horizontal, x1 );
                if ( index1 > 0 )
                {
                    from = index1 - 1;
                    point1 = data->interpolatedPoint( Qt::Horizontal, x1 );
                }

                const int index2 = data->upperIndex( Qt::Horizontal, x2 );
                if ( index2 > 0 )
                {
                    to = index2;
                    point2 = data->interpolatedPoint( Qt::Horizontal, x2 );
                }
            }
            else if ( data->hints() & QskPlotCurveData::MonotonicY )
            {
                const qreal y1 = scaleRect.top();
                const qreal y2 = scaleRect.bottom();

                if ( y1 > point2.y() || y2 < point1.y() )
                {
                    QskSGNode::resetGeometry( this );
                    return;
                }

                const int index1 = data->upperIndex( Qt::Vertical, y1 );
                if ( index1 > 0 )
                {
                    from = index1 - 1;
                    point1 = data->interpolatedPoint( Qt::Vertical, y1 );
                }

                const int index2 = data->upperIndex( Qt::Vertical, y2 );
                if ( index2 > 0 )
                {
                    to = index2;
                    point2 = data->interpolatedPoint( Qt::Vertical, y2 );
                }
            }

            m_geometry.allocate( to - from + 1 );

            auto p = m_geometry.vertexDataAsColoredPoint2D();

            p++->set( point1.x(), point1.y(), c.r, c.g, c.b, c.a );

            for ( int i = from + 1; i < to; i++ )
            {
                const auto point = data->pointAt( i );
                p++->set( point.x(), point.y(), c.r, c.g, c.b, c.a );
            }

            p++->set( point2.x(), point2.y(), c.r, c.g, c.b, c.a );

            markDirty( QSGNode::DirtyGeometry );
        }

      private:
        QSGGeometry m_geometry;
        QSGVertexColorMaterial m_material;
    };
}

QskPlotCurveSkinlet::QskPlotCurveSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { Polygon } );
}

QskPlotCurveSkinlet::~QskPlotCurveSkinlet()
{
}

QSGNode* QskPlotCurveSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    if ( nodeRole == Polygon )
        return updatePolygonNode( skinnable, node );

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskPlotCurveSkinlet::updatePolygonNode(
    const QskSkinnable* skinnable, QSGNode* node ) const
{
    using Q = QskPlotCurve;

    auto curve = static_cast< const QskPlotCurve* >( skinnable );

    const auto curveData = curve->data();
    if ( curveData == nullptr || curveData->count() == 0 )
        return nullptr;

    const auto color = curve->color( Q::Line );
    if ( !color.isValid() || color.alpha() == 0 )
        return nullptr;

    const auto lineWidth = curve->metric( Q::Line | QskAspect::Size );
    if ( lineWidth <= 0.0 )
        return nullptr;

    auto curveNode = QskSGNode::ensureNode< CurveNode >( node );
    curveNode->updateCurve( curve->scaleRect(), curveData, color, lineWidth );

    return curveNode;
}

#include "moc_QskPlotCurveSkinlet.cpp"
