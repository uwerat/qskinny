/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskFrameNode.h"
#include "QskVertexRendererColored.h"

QskFrameNode::QskFrameNode():
    m_fillColor( Qt::white ),
    m_borderWidth( 3 ),
    m_radius( 0 ),
    m_shadeFactor( 0.0 ),
    m_isDirty( false ),
    m_geometry( QSGGeometry::defaultAttributes_ColoredPoint2D(), 0 )
{
    setGeometry( &m_geometry );
    setMaterial( &m_material );
}

QskFrameNode::~QskFrameNode()
{
}

void QskFrameNode::setRect( const QRectF& rect )
{
    if ( m_rect != rect )
    {
        m_rect = rect;
        m_isDirty = true;
    }
}

void QskFrameNode::setRadius( double radius )
{
    if ( radius < 0.0 )
        radius = 0.0;

    if ( radius != m_radius )
    {
        m_radius = radius;
        m_isDirty = true;
    }
}

void QskFrameNode::setColors(
    const QColor& darkColor,
    const QColor& fillColor, const QColor& lightColor )
{
    if ( ( darkColor != m_darkColor )
        || ( lightColor != m_lightColor )
        || ( fillColor != m_fillColor ) )
    {
        m_lightColor = lightColor;
        m_darkColor = darkColor;
        m_fillColor = fillColor;

        m_isDirty = true;
    }
}

void QskFrameNode::setBorderWidth( double width )
{
    if ( width < 0.0 )
        width = 0.0;

    if ( width != m_borderWidth )
    {
        m_borderWidth = width;
        m_isDirty = true;
    }
}

void QskFrameNode::setShadeFactor( double factor )
{
    factor = qBound( -1.0, factor, 1.0 );

    if ( factor != m_shadeFactor )
    {
        m_shadeFactor = factor;
        m_isDirty = true;
    }
}

void QskFrameNode::update()
{
    if ( m_isDirty )
    {
        QskVertexPen pen;
        pen.setWidth( m_borderWidth );
        pen.setColors( m_darkColor, m_lightColor );
        pen.setShadeFactor( m_shadeFactor );

        QskVertexRendererColored renderer( m_geometry );
        renderer.setPen( pen );
        renderer.setBrush( m_fillColor );
        renderer.renderRoundedRect( m_rect, m_radius );

        markDirty( QSGNode::DirtyGeometry );
        m_isDirty = false;
    }
}
