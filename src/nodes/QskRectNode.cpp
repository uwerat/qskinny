/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskRectNode.h"
#include "QskVertexRenderer.h"
#include "QskVertexRendererColored.h"

QskRectNode::QskRectNode():
    m_radius( 0.0 ),
    m_borderWidth( 0.0 ),
    m_isDirty( false ),
    m_geometryVertex( QSGGeometry::defaultAttributes_ColoredPoint2D(), 0 ),
    m_geometryFlat( QSGGeometry::defaultAttributes_Point2D(), 0 )
{
}

QskRectNode::~QskRectNode()
{
}

void QskRectNode::setRect( const QRectF& rect )
{
    if ( m_rect != rect )
    {
        m_rect = rect;
        m_isDirty = true;
    }
}

void QskRectNode::setRadius( double radius )
{
    if ( radius < 0.0 )
        radius = 0.0;

    if ( radius != m_radius )
    {
        m_radius = radius;
        m_isDirty = true;
    }
}

void QskRectNode::setBorderWidth( double width )
{
    if ( width < 0.0 )
        width = 0.0;

    if ( width != m_borderWidth )
    {
        m_borderWidth = width;
        m_isDirty = true;
    }
}

void QskRectNode::setBorderColor( const QColor& color  )
{
    if ( color != m_borderColor )
    {
        // only the material needs to be dirty !
        m_borderColor = color;
        m_isDirty = true;
    }
}

void QskRectNode::setFillColor( const QColor& color  )
{
    if ( color != m_fillColor )
    {
        m_fillColor = color;
        if ( !m_fillGradient.isValid() )
        {
            // only the material needs to be dirty !
            m_isDirty = true;
        }
    }
}

void QskRectNode::setFillGradient( const QskGradient& gradient )
{
    if ( gradient != m_fillGradient )
    {
        m_fillGradient = gradient;
        m_isDirty = true;
    }
}

void QskRectNode::update()
{
    if ( !m_isDirty )
        return;

    QColor fillColor = m_fillColor;

    bool hasGradient = m_fillGradient.isValid();
    if ( hasGradient && m_fillGradient.isMonochrome() )
    {
        // the gradient is simply one color
        fillColor = m_fillGradient.stops().first().color();
        hasGradient = false;
    }

    bool hasBorder = ( m_borderWidth > 0.0 )
        && m_borderColor.isValid() && ( m_borderColor.alpha() > 0 );

    bool hasFill = hasGradient || ( fillColor.isValid() && fillColor.alpha() > 0 );

    if ( hasBorder )
    {
        const double dim = 0.5 * qMax( m_rect.width(), m_rect.height() );
        if ( m_borderWidth >= dim )
        {
            // the border takes it all
            hasFill = false;
        }
    }

    if ( hasFill && hasBorder && !hasGradient )
    {
        if ( m_borderColor == m_fillColor )
        {
            // we can draw border and background in one
            hasBorder = false;
        }
    }

    if ( ( hasFill && hasBorder ) || hasGradient )
    {
        if ( material() != &m_materialVertex )
        {
            setMaterial( &m_materialVertex );
        }

        if ( geometry() != &m_geometryVertex )
        {
            m_geometryFlat.allocate( 0 );
            setGeometry( &m_geometryVertex );
        }

        const int vertexCount = geometry()->vertexCount();

        QskVertexPen pen;
        pen.setWidth( m_borderWidth );
        pen.setColor( m_borderColor );

        QskVertexRendererColored renderer( *geometry() );
        renderer.setPen( pen );

        if ( hasGradient )
            renderer.setBrush( m_fillGradient );
        else
            renderer.setBrush( m_fillColor );

        renderer.renderRoundedRect( m_rect, m_radius );

        if ( vertexCount == 0 && geometry()->vertexCount() > 0 )
        {
            // not sure why we have to set QSGNode::DirtyMaterial here,
            // but otherwise the node stays invisible. Need to check the
            // implementation of QSGGeometryNode ...
            markDirty( QSGNode::DirtyMaterial );
        }
    }
    else
    {
        if ( material() != &m_materialFlat )
        {
            setMaterial( &m_materialFlat );
        }

        if ( geometry() != &m_geometryFlat )
        {
            m_geometryVertex.allocate( 0 );
            setGeometry( &m_geometryFlat );
        }

        QskVertexRenderer renderer( *geometry() );
        if ( hasFill )
        {
            if ( m_materialFlat.color() != fillColor )
            {
                m_materialFlat.setColor( fillColor );
                markDirty( QSGNode::DirtyMaterial );
            }
            renderer.renderRoundedRect( m_rect, m_radius );
        }
        else
        {
            if ( m_materialFlat.color() != fillColor )
            {
                m_materialFlat.setColor( m_borderColor );
                markDirty( QSGNode::DirtyMaterial );
            }
            renderer.renderRoundedBorder( m_rect, m_radius, m_borderWidth );
        }
    }

    markDirty( QSGNode::DirtyGeometry );
    m_isDirty = false;
}
