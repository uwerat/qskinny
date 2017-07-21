/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskClipNode.h"
#include "QskVertexRenderer.h"

QskClipNode::QskClipNode():
    m_radius( 0 ),
    m_isDirty( true ),
    m_geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
{
    setGeometry( &m_geometry );
    setIsRectangular( true );
}

QskClipNode::~QskClipNode()
{
}

void QskClipNode::setRect( const QRectF& rect )
{
    if ( clipRect() != rect )
    {
        setClipRect( rect );
        m_isDirty = true;
    }
}

void QskClipNode::setRadius( double radius )
{
    if ( radius < 0.0 )
        radius = 0.0;

    if ( radius != m_radius )
    {
        m_radius = radius;
        setIsRectangular( m_radius == 0.0 );
        m_isDirty = true;
    }
}

void QskClipNode::update()
{
    if ( !m_isDirty )
        return;

    if ( isRectangular() )
    {
        if ( m_geometry.vertexCount() > 0 )
        {
            m_geometry.allocate( 0 );
            markDirty( QSGNode::DirtyGeometry ); // probably pointless to do
        }
    }
    else
    {
        QskVertexRenderer renderer( m_geometry );
        renderer.renderRoundedRect( clipRect(), m_radius );

        markDirty( QSGNode::DirtyGeometry );
    }

    m_isDirty = false;
}
