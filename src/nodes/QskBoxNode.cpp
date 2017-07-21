/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxNode.h"
#include "QskBoxOptions.h"
#include "QskAspect.h"

#include <qhashfunctions.h>

static inline Qt::Edge qskAspectToEdge( QskAspect::Edge edge )
{
    switch ( edge )
    {
        case QskAspect::LeftEdge:
            return Qt::LeftEdge;

        case QskAspect::TopEdge:
            return Qt::TopEdge;

        case QskAspect::RightEdge:
            return Qt::RightEdge;

        case QskAspect::BottomEdge:
            return Qt::BottomEdge;

        default:
            break;
    }
    return static_cast< Qt::Edge >( 0 );
}

QskBoxNode::QskBoxNode():
    m_metricsHash( 0 ),
    m_colorsHash( 0 )
{
    setGeometry( &m_geometry );
    setMaterial( &m_material );
}

QskBoxNode::~QskBoxNode()
{
#if VM_SUPPORT
    if ( m_metricsHash > 0 )
        m_material.release( m_metricsHash );
#endif
}

void QskBoxNode::setBoxData( const QRectF& rect, const QskBoxOptions& options )
{
    using namespace QskAspect;

    const uint metricsHash = options.metricsHash();
    const uint colorsHash = options.colorsHash();

    QSGNode::DirtyState dirtyState = 0;

    if ( !m_material.isValid() || ( metricsHash != m_metricsHash ) )
    {
#if VM_SUPPORT
        if ( m_metricsHash > 0 )
            m_material.release( m_metricsHash );
#endif
        m_material.setBoxOptions( options );
        dirtyState |= QSGNode::DirtyMaterial;
    }

    if ( ( rect != m_rect ) || ( metricsHash != m_metricsHash ) )
    {
        m_geometry.setBorder( rect, options.unitedMargins(),
            m_material.textureCoordinates(), m_material.textureSize() );

        dirtyState |= QSGNode::DirtyGeometry;

        m_metricsHash = metricsHash;
        m_rect = rect;
    }

    if ( colorsHash != m_colorsHash )
    {
        m_geometry.setEdgeBackground( Qt::LeftEdge,
            options.color.fillTopLeft, options.color.fillBottomLeft );

        m_geometry.setEdgeBackground( Qt::TopEdge,
            options.color.fillTopLeft, options.color.fillTopRight );

        m_geometry.setEdgeBackground( Qt::RightEdge,
            options.color.fillTopRight, options.color.fillBottomRight );

        m_geometry.setEdgeBackground( Qt::BottomEdge,
            options.color.fillBottomLeft, options.color.fillBottomRight );

        m_geometry.setEdgeForeground( qskAspectToEdge( LeftEdge ), options.color.borderLeft );
        m_geometry.setEdgeForeground( qskAspectToEdge( TopEdge ), options.color.borderTop );
        m_geometry.setEdgeForeground( qskAspectToEdge( RightEdge ), options.color.borderRight );
        m_geometry.setEdgeForeground( qskAspectToEdge( BottomEdge ), options.color.borderBottom );

        dirtyState |= QSGNode::DirtyGeometry;
        m_colorsHash = colorsHash;
    }

    if ( dirtyState )
        markDirty( dirtyState );
}

QRectF QskBoxNode::rect() const
{
    return m_rect;
}
