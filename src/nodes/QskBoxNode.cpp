/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxNode.h"
#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxRenderer.h"
#include "QskBoxShapeMetrics.h"
#include "QskGradient.h"

#include <qglobalstatic.h>
#include <qsgflatcolormaterial.h>
#include <qsgvertexcolormaterial.h>

Q_GLOBAL_STATIC( QSGVertexColorMaterial, qskMaterialVertex )

static inline uint qskMetricsHash(
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& borderMetrics )
{
    uint hash = 13000;

    hash = shape.hash( hash );
    return borderMetrics.hash( hash );
}

static inline uint qskColorsHash(
    const QskBoxBorderColors& borderColors, const QskGradient& fillGradient )
{
    uint hash = 13000;
    hash = borderColors.hash( hash );
    return fillGradient.hash( hash );
}

QskBoxNode::QskBoxNode()
    : m_metricsHash( 0 )
    , m_colorsHash( 0 )
    , m_geometry( QSGGeometry::defaultAttributes_ColoredPoint2D(), 0 )
{
    setMaterial( qskMaterialVertex );
    setGeometry( &m_geometry );
}

QskBoxNode::~QskBoxNode()
{
    if ( material() != qskMaterialVertex )
        delete material();
}

void QskBoxNode::setBoxData( const QRectF& rect, const QskGradient& fillGradient )
{
    setBoxData( rect, QskBoxShapeMetrics(), QskBoxBorderMetrics(),
        QskBoxBorderColors(), fillGradient );
}

void QskBoxNode::setBoxData( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& borderMetrics,
    const QskBoxBorderColors& borderColors, const QskGradient& fillGradient )
{
#if 1
    const uint metricsHash = qskMetricsHash( shape, borderMetrics );
    const uint colorsHash = qskColorsHash( borderColors, fillGradient );

    if ( ( metricsHash == m_metricsHash ) &&
        ( colorsHash == m_colorsHash ) && ( rect == m_rect ) )
    {
        return;
    }

    m_metricsHash = metricsHash;
    m_colorsHash = colorsHash;
    m_rect = rect;

    markDirty( QSGNode::DirtyMaterial );
    markDirty( QSGNode::DirtyGeometry );
#endif

    if ( rect.isEmpty() )
    {
        m_geometry.allocate( 0 );
        return;
    }

    bool hasFill = fillGradient.isValid();

    bool hasBorder = !borderMetrics.isNull();
    if ( hasBorder )
    {
        /*
            Wrong as the border width should have an
            effect - even if not being visible. TODO ...
         */

        hasBorder = borderColors.isVisible();
    }

    if ( !hasBorder && !hasFill )
    {
        m_geometry.allocate( 0 );
        return;
    }

    const bool isFillMonochrome = hasFill ? fillGradient.isMonochrome() : true;
    const bool isBorderMonochrome = hasBorder ? borderColors.isMonochrome() : true;

    if ( hasFill && hasBorder )
    {
        if ( isFillMonochrome && isBorderMonochrome )
        {
            if ( borderColors.color( Qsk::Left ) == fillGradient.startColor() )
            {
                // we can draw border and background in one
                hasBorder = false;
            }
        }
    }

    bool maybeFlat = false;

    /*
        Always using the same material result in a better batching
        but wastes some memory. when we have a solid color.
        Maybe its worth to introduce a flag to control the behaviour,
        but for the moment we go with performance.
     */

    if ( maybeFlat )
    {
        if ( ( hasFill && hasBorder ) ||
            ( hasFill && !isFillMonochrome ) ||
            ( hasBorder && !isBorderMonochrome ) )
        {
            maybeFlat = false;
        }
    }

    QskBoxRenderer renderer;

    if ( !maybeFlat )
    {
        setMonochrome( false );

        renderer.renderBox( m_rect, shape, borderMetrics,
            borderColors, fillGradient, *geometry() );
    }
    else
    {
        // all is done with one color
        setMonochrome( true );

        auto* flatMaterial = static_cast< QSGFlatColorMaterial* >( material() );

        if ( hasFill )
        {
            flatMaterial->setColor( fillGradient.startColor() );
            renderer.renderFill( m_rect, shape, QskBoxBorderMetrics(), *geometry() );
        }
        else
        {
            flatMaterial->setColor( borderColors.color( Qsk::Left ).rgba() );
            renderer.renderBorder( m_rect, shape, borderMetrics, *geometry() );
        }
    }
}

void QskBoxNode::setMonochrome( bool on )
{
    const auto material = this->material();

    if ( on == ( material != qskMaterialVertex ) )
        return;

    m_geometry.allocate( 0 );

    if ( on )
    {
        setMaterial( new QSGFlatColorMaterial() );

        const QSGGeometry g( QSGGeometry::defaultAttributes_Point2D(), 0 );
        memcpy( ( void* ) &m_geometry, ( void* ) &g, sizeof( QSGGeometry ) );
    }
    else
    {
        setMaterial( qskMaterialVertex );
        delete material;

        const QSGGeometry g( QSGGeometry::defaultAttributes_ColoredPoint2D(), 0 );
        memcpy( ( void* ) &m_geometry, ( void* ) &g, sizeof( QSGGeometry ) );
    }
}
