/******************************************************************************
 * QSkinny - Copyright (C) 2021 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "DiagramSegmentsNode.h"

#include <QtMath>

DiagramSegmentsNode::DiagramSegmentsNode()
    : m_geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 8, 0 )
    m_geometry.setDrawingMode( QSGGeometry::DrawLines );
#else
    m_geometry.setDrawingMode( GL_LINES );
#endif

    setGeometry( &m_geometry );
    setMaterial( &m_material );
}

void DiagramSegmentsNode::update( const QRectF& rect, const QColor& color, const QVector<QVector<QPointF> >& dataPoints, int xGridLines )
{
    Q_UNUSED( rect );

    if( color != m_color )
    {
        m_material.setColor( color );
        m_color = color;
        markDirty( QSGNode::DirtyMaterial );
    }

    if( m_rect == rect && m_dataPoints == dataPoints && m_xGridLines == xGridLines )
    {
        return;
    }

    m_rect = rect;
    m_dataPoints = dataPoints;
    m_xGridLines = xGridLines;

    const qreal step = rect.width() / ( xGridLines - 1 );

    m_geometry.allocate( m_xGridLines * 2 );
    auto vertexData = m_geometry.vertexDataAsPoint2D();

    for( int i = 0; i < m_xGridLines; ++i )
    {
        const qreal x = i * step;

        vertexData[2 * i].x = x;
        vertexData[2 * i].y = rect.height();

        vertexData[2 * i + 1].x = x;
        vertexData[2 * i + 1].y = 0;
    }

    markDirty( QSGNode::DirtyGeometry );
}
