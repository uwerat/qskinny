/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxClipNode.h"
#include "QskBoxRenderer.h"
#include "QskBoxShapeMetrics.h"
#include "QskBoxBorderMetrics.h"
#include "QskFunctions.h"

static inline uint qskMetricsHash( const QskBoxShapeMetrics& shape,
    const QskBoxBorderMetrics& border )
{
    uint hash = 13000;

    hash = shape.hash( hash );
    return border.hash( hash );
}

QskBoxClipNode::QskBoxClipNode():
    m_hash( 0 ),
    m_geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
{
    setGeometry( &m_geometry );
}

QskBoxClipNode::~QskBoxClipNode()
{
}

void QskBoxClipNode::setBox( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& border )
{
    const uint hash = qskMetricsHash( shape, border );
    if ( hash == m_hash && rect == m_rect )
        return;

    m_rect = rect;
    m_hash = hash;

    if ( shape.isRectangle() )
    {
        if ( m_geometry.vertexCount() > 0 )
            m_geometry.allocate( 0 );

        setIsRectangular( true );
    }
    else
    {
        setIsRectangular( false );
        QskBoxRenderer().renderFill( rect, shape, border, m_geometry );
    }

    /*
        Even in situations, where the clipping is not rectangular, it is
        useful to know its bounding rectangle
     */
    setClipRect( qskValidOrEmptyInnerRect( rect, border.widths() ) );

    markDirty( QSGNode::DirtyGeometry );
}

template< class Point >
static inline void qskCopyPoints( const Point* from, Point* to,
    int numPoints, const QPointF& offset )
{
    if ( offset.isNull() )
    {
        memcpy( to, from, numPoints * sizeof( Point ) );
    }
    else
    {
        const float dx = offset.x();
        const float dy = offset.y();

        for ( int i = 0; i < numPoints; i++ )
        {
            to[i].x = from[i].x + dx;
            to[i].y = from[i].y + dy;
        }
    }
}

QSGGeometry* QskBoxClipNode::translatedGeometry(
    const QSGGeometry* geometry, const QPointF& offset )
{
    if ( geometry == nullptr || geometry->vertexCount() == 0 )
        return nullptr;

    QSGGeometry* g = nullptr;

    if ( geometry->sizeOfVertex() == sizeof( QSGGeometry::Point2D ) )
    {
        g = new QSGGeometry(
            QSGGeometry::defaultAttributes_Point2D(), geometry->vertexCount(),
            geometry->indexCount(), geometry->indexType() );

        qskCopyPoints( geometry->vertexDataAsPoint2D(),
            g->vertexDataAsPoint2D(), g->vertexCount(), offset );
    }
    else if ( geometry->sizeOfVertex() == sizeof( QSGGeometry::ColoredPoint2D ) )
    {
        g = new QSGGeometry(
            QSGGeometry::defaultAttributes_ColoredPoint2D(), geometry->vertexCount(),
            geometry->indexCount(), geometry->indexType() );
            
        qskCopyPoints( geometry->vertexDataAsColoredPoint2D(),
            g->vertexDataAsColoredPoint2D(), g->vertexCount(), offset );
    }

    return g;
}
