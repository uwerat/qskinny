/******************************************************************************
 * QSkinny - Copyright (C) 2021 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "DiagramDataNode.h"
#include <QTransform>
#include <QskFunctions.h>

DiagramDataNode::DiagramDataNode()
    : m_geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
{
    setGeometry( &m_geometry );
    setMaterial( &m_material );
}

void DiagramDataNode::update( const QRectF& rect, Type type, const QColor& color, const QVector<QPointF>& dataPoints,
                              const qreal yMax, Qsk::Position position, int lineWidth )
{
    Q_UNUSED( rect );

    if( color != m_color )
    {
        m_material.setColor( color );
        m_color = color;
        markDirty( QSGNode::DirtyMaterial );
    }

    if( m_rect == rect && m_dataPoints == dataPoints && m_yMax == yMax && m_position == position
        && m_type == type && m_lineWidth == lineWidth )
    {
        return;
    }

    if( lineWidth != m_lineWidth )
    {
        m_lineWidth = lineWidth;
        m_geometry.setLineWidth( lineWidth );

        markDirty( QSGNode::DirtyGeometry );
    }

    m_rect = rect;
    m_dataPoints = dataPoints;
    m_yMax = yMax;
    m_position = position;
    m_type = type;

    const auto drawingMode =
#if QT_VERSION >= QT_VERSION_CHECK( 5, 8, 0 )
        ( m_type == Line ) ? QSGGeometry::DrawLines : QSGGeometry::DrawTriangleStrip;
#else
        ( m_type == Line ) ? GL_LINES : GL_TRIANGLE_STRIP;
#endif

    m_geometry.setDrawingMode( drawingMode );

    const int vertexCount = ( m_type == Line ) ? m_dataPoints.size() * 2 - 1 : m_dataPoints.size() * 2;

    m_geometry.allocate( vertexCount );
    auto vertexData = m_geometry.vertexDataAsPoint2D();

    qreal xMin;
    qreal xMax;

    if( m_dataPoints.count() > 0 )
    {
        xMin =  m_dataPoints.at( 0 ).x();
        xMax = m_dataPoints.at( m_dataPoints.count() - 1 ).x();
    }
    else
    {
        xMin = 0;
        xMax = 0;
    }

    // ### we should have a different function for each chart type
    for( int i = 0; i < m_dataPoints.size(); ++i )
    {
        const qreal x = ( ( m_dataPoints.at( i ).x() - xMin ) / ( xMax - xMin ) ) * rect.width();
        const qreal fraction = ( m_dataPoints.at( i ).y() / yMax ) * rect.height();
        const qreal y = ( position == Qsk::Top ) ? fraction : rect.height() - fraction;

        if( m_type == Line && i < m_dataPoints.size() - 1 )
        {
            const qreal x2 = ( ( m_dataPoints.at( i + 1 ).x() - xMin ) / ( xMax - xMin ) ) * rect.width();
            const qreal fraction2 = ( m_dataPoints.at( i + 1 ).y() / yMax ) * rect.height();
            const qreal y2 = ( position == Qsk::Top ) ? fraction2 : rect.height() - fraction2;
            vertexData[2 * i].x = x;
            vertexData[2 * i].y = y;

            vertexData[2 * i + 1].x = x2;
            vertexData[2 * i + 1].y = y2;
        }
        else if( m_type == Area )
        {
            const qreal y0 = ( position == Qsk::Top ) ? 0 : rect.height();
            vertexData[2 * i].x = x;
            vertexData[2 * i].y = y;

            vertexData[2 * i + 1].x = x;
            vertexData[2 * i + 1].y = y0;
        }
    }

    markDirty( QSGNode::DirtyGeometry );
}
