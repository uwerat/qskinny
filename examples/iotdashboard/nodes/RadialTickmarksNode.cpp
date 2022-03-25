/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "RadialTickmarksNode.h"

#include <QskScaleTickmarks.h>
#include <QskArcMetrics.h>
#include <QtMath>

RadialTickmarksNode::RadialTickmarksNode()
    : m_geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
{
    m_geometry.setDrawingMode( QSGGeometry::DrawLines );
    m_geometry.setVertexDataPattern( QSGGeometry::StaticPattern );

    setGeometry( &m_geometry );
    setMaterial( &m_material );
}

RadialTickmarksNode::~RadialTickmarksNode()
{
}

void RadialTickmarksNode::update( const QColor& color, const QRectF& rect,
    const QskArcMetrics& arcMetrics, const QskScaleTickmarks& tickmarks, int lineWidth )
{
    if( lineWidth != m_lineWidth )
    {
        m_lineWidth = lineWidth;
        m_geometry.setLineWidth( lineWidth );

        markDirty( QSGNode::DirtyGeometry );
    }

    const auto hash = tickmarks.hash( 17435 );

    if( ( hash != m_hash ) || ( rect != m_rect ) )
    {
        m_hash = hash;
        m_rect = rect;

        m_geometry.allocate( tickmarks.tickCount() * 2 );
        auto vertexData = m_geometry.vertexDataAsPoint2D();

        const auto center = rect.center();
        const auto radius = 0.5 * rect.width();
        const auto needleRadius = radius - arcMetrics.width();

        using TM = QskScaleTickmarks;

        for( int i = TM::MinorTick; i <= TM::MajorTick; i++ )
        {
            const auto tickType = static_cast< TM::TickType >( i );
            const auto ticks = tickmarks.ticks( tickType );

            const auto startAngle = arcMetrics.startAngle();
            const auto endAngle = startAngle + arcMetrics.spanAngle();

            for( const auto tick : ticks )
            {
                const qreal ratio = ( tick - startAngle ) / ( endAngle - startAngle );
                const qreal angle = ratio * ( endAngle - startAngle );

                const qreal cos = qFastCos( qDegreesToRadians( angle ) );
                const qreal sin = qFastSin( qDegreesToRadians( angle ) );

                const auto xStart = center.x() - radius * cos;
                const auto yStart = center.y() - radius * sin;

                const auto xEnd = center.x() - needleRadius * cos;
                const auto yEnd = center.y() - needleRadius * sin;

                vertexData[ 0 ].set( xStart, yStart );
                vertexData[ 1 ].set( xEnd, yEnd );

                vertexData += 2;
            }
        }

        m_geometry.markVertexDataDirty();
        markDirty( QSGNode::DirtyGeometry );
    }

    if ( color != m_material.color() )
    {
        m_material.setColor( color );
        markDirty( QSGNode::DirtyMaterial );
    }
}
