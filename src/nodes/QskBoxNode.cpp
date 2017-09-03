/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxNode.h"
#include "QskBoxOptions.h"
#include "QskAspect.h"

#include <QOpenGLContext>
#include <qhashfunctions.h>

// hashes based on CRC32 -> not necessarily unique: TODO

static inline uint qskMetricsHash( const QskBoxOptions& options )
{
    uint hash = qHash( QOpenGLContext::currentContext() );

    hash = qHashBits( &options.shadows, sizeof( options.shadows ), hash );

    const auto& m = options.metrics;
    hash = qHashBits( &m.widths(), sizeof( QskMargins ), hash );

    QSizeF radius[4];
    for ( int i = 0; i < 4; i++ )
        radius[i] = m.radius( static_cast< Qt::Corner >( i ) );

    hash = qHashBits( radius, sizeof( radius ), hash );

    uint flags[2];
    flags[0] = m.radiusSizeMode();
    flags[1] = m.widthSizeMode();

    return qHashBits( flags, sizeof( flags ), hash );
}

static inline uint qskColorsHash( const QskBoxOptions& options )
{
    const auto& colors = options.colors;

    QRgb rgb[8];

    rgb[0] = colors.borderColor( Qt::TopEdge ).rgba();
    rgb[1] = colors.borderColor( Qt::LeftEdge ).rgba();
    rgb[2] = colors.borderColor( Qt::RightEdge ).rgba();
    rgb[3] = colors.borderColor( Qt::BottomEdge ).rgba();

    rgb[4] = colors.fillColor( Qt::TopLeftCorner ).rgba();
    rgb[5] = colors.fillColor( Qt::TopRightCorner ).rgba();
    rgb[6] = colors.fillColor( Qt::BottomLeftCorner ).rgba();
    rgb[7] = colors.fillColor( Qt::BottomRightCorner ).rgba();

    return qHashBits( rgb, sizeof( rgb ), 17000 );
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

    const uint metricsHash = qskMetricsHash( options );
    const uint colorsHash = qskColorsHash( options );

    QSGNode::DirtyState dirtyState = 0;

    if ( !m_material.isValid() || ( metricsHash != m_metricsHash ) )
    {
#if VM_SUPPORT
        if ( m_metricsHash > 0 )
            m_material.release( m_metricsHash );
#endif
        m_material.setBoxOptions( options, metricsHash );
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
        const auto& c = options.colors;

        m_geometry.setEdgeBackground( Qt::LeftEdge,
            c.fillColor( Qt::TopLeftCorner ).rgba(),
            c.fillColor( Qt::BottomLeftCorner ).rgba() );

        m_geometry.setEdgeBackground( Qt::TopEdge,
            c.fillColor( Qt::TopLeftCorner ).rgba(),
            c.fillColor( Qt::TopRightCorner ).rgba() );

        m_geometry.setEdgeBackground( Qt::RightEdge,
            c.fillColor( Qt::TopRightCorner ).rgba(),
            c.fillColor( Qt::BottomRightCorner ).rgba() );

        m_geometry.setEdgeBackground( Qt::BottomEdge,
            c.fillColor( Qt::BottomLeftCorner ).rgba(),
            c.fillColor( Qt::BottomRightCorner ).rgba() );

        m_geometry.setEdgeForeground( Qt::LeftEdge, c.borderColor( Qt::LeftEdge ).rgba() );
        m_geometry.setEdgeForeground( Qt::TopEdge, c.borderColor( Qt::TopEdge ).rgba() );
        m_geometry.setEdgeForeground( Qt::RightEdge, c.borderColor( Qt::RightEdge ).rgba() );
        m_geometry.setEdgeForeground( Qt::BottomEdge, c.borderColor( Qt::BottomEdge ).rgba() );

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
