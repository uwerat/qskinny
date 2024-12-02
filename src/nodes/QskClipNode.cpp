/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskClipNode.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxRenderer.h"
#include "QskBoxShapeMetrics.h"
#include "QskFunctions.h"
#include "QskVertex.h"

#include <qquickwindow.h>

static inline QskHashValue qskMetricsHash(
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& border )
{
    QskHashValue hash = 13000;

    hash = shape.hash( hash );
    return border.hash( hash );
}

static inline void qskSetBoundingRect( QSGClipNode* node, const QRectF& rect )
{
    /*
        Depending on isRectangular: the "scene graph renderer can use
        scissoring instead of stencil, which is significantly faster."

        However the batch renderer ( qsgbatchrenderer.cpp ) is rounding
        the clip rectangle to integers and the clip might become too small/large.
        So we always have to use stencil clipping - even if it might have a negative
        impact on the performance.

        When isRectangular is set to false the clipRect is not used from the
        renderer and we use the memory for the storing the bounding rectangle.
     */

    node->setIsRectangular( false );
    node->setClipRect( rect );
}

static inline QskVertex::Line* qskAllocateLines(
    QSGGeometry& geometry, int lineCount )
{
    geometry.allocate( 2 * lineCount ); // 2 points per line
    return reinterpret_cast< QskVertex::Line* >( geometry.vertexData() );
}

QskClipNode::QskClipNode()
    : m_hash( 0 )
    , m_geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
{
    setGeometry( &m_geometry );
}

QskClipNode::~QskClipNode()
{
}

void QskClipNode::setRect( const QRectF& rect )
{
    setRegion( rect, QRectF() );
}

void QskClipNode::setRegion( const QRectF& rect, const QRectF& excludedRect )
{
    if ( rect.isEmpty() )
    {
        /*
            what about rectangles having a width/height
            of 0 ( f.e lines ) TODO ...
         */
        reset();
        return;
    }

    const auto innerRect = excludedRect.isEmpty()
        ? QRectF() : excludedRect.intersected( rect );

    const auto hash = qHashBits( &innerRect, sizeof( innerRect ), 1450 );
    if ( ( hash == m_hash ) && ( rect == Inherited::clipRect() ) )
        return;

    qskSetBoundingRect( this, rect );
    m_hash = hash;

    m_geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );

    if ( innerRect.isEmpty() )
    {
        const auto l = qskAllocateLines( m_geometry, 2 );

        l[0].setLine( rect.topLeft(), rect.topRight() );
        l[1].setLine( rect.bottomLeft(), rect.bottomRight() );
    }
    else
    {
        const auto l = qskAllocateLines( m_geometry, 5 );

        l[0].setLine( rect.topLeft(), innerRect.topLeft() );
        l[1].setLine( rect.topRight(), innerRect.topRight() );
        l[2].setLine( rect.bottomRight(), innerRect.bottomRight() );
        l[3].setLine( rect.bottomLeft(), innerRect.bottomLeft() );
        l[4] = l[0];
    }

    m_geometry.markVertexDataDirty();
    markDirty( QSGNode::DirtyGeometry );
}

void QskClipNode::setBox( const QQuickWindow* window, const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& border )
{
    if ( rect.isEmpty() )
    {
        reset();
        return;
    }

    const auto hash = qskMetricsHash( shape, border );
    if ( hash == m_hash && rect == boundingRectangle() )
        return;

    qskSetBoundingRect( this, rect );
    m_hash = hash;

    QskBoxRenderer renderer( window );
    renderer.setFillLines( rect, shape, border, m_geometry );

    m_geometry.markVertexDataDirty();
    markDirty( QSGNode::DirtyGeometry );
}

void QskClipNode::reset()
{
    Inherited::setIsRectangular( true );
    Inherited::setClipRect( QRectF() );

    if ( m_geometry.vertexData() )
    {
        m_geometry.allocate( 0 );
        m_geometry.markVertexDataDirty();
    }

    if ( m_hash != 0 )
    {
        m_hash = 0;
        markDirty( QSGNode::DirtyGeometry );
    }
}
