/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskBoxClipNode.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxRenderer.h"
#include "QskBoxShapeMetrics.h"
#include "QskFunctions.h"

static inline QskHashValue qskMetricsHash(
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& border )
{
    QskHashValue hash = 13000;

    hash = shape.hash( hash );
    return border.hash( hash );
}

QskBoxClipNode::QskBoxClipNode()
    : m_hash( 0 )
    , m_geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
{
    setGeometry( &m_geometry );
}

QskBoxClipNode::~QskBoxClipNode()
{
}

void QskBoxClipNode::setBox( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& border )
{
    const auto hash = qskMetricsHash( shape, border );
    if ( hash == m_hash && rect == m_rect )
        return;

    m_rect = rect;
    m_hash = hash;

    bool isRectangular = false;

#if 0
    /*
        Depending on isRectangular the "renderer can use scissoring instead of stencil,
        which is significantly faster."

        However the batch renderer ( qsgbatchrenderer.cpp ) is rounding the clip rectangle
        to integers and the clip might become too small/large.

        So we always have to use stencil clipping - even if it might have a negative
        impact on the performance. TODO ...
     */

    if ( shape.isRectangle() )
        isRectangular = true;
#endif

    if ( isRectangular )
    {
        if ( m_geometry.vertexCount() > 0 )
            m_geometry.allocate( 0 );

        setIsRectangular( true );
    }
    else
    {
        setIsRectangular( false );
        QskBox::renderFillGeometry( rect, shape, border, m_geometry );
    }

    /*
        Even in situations, where the clipping is not rectangular, it is
        useful to know its bounding rectangle
     */
    setClipRect( qskValidOrEmptyInnerRect( rect, border.widths() ) );

    m_geometry.markVertexDataDirty();
    markDirty( QSGNode::DirtyGeometry );
}
