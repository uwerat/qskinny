/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxNode.h"
#include "QskBoxOptions.h"
#include "QskBoxRenderer.h"

#include <QSGVertexColorMaterial>
#include <QSGFlatColorMaterial>

static QSGVertexColorMaterial qskMaterialVertex;

static inline bool qskHasAlpha( const QRgb* rgbValues )
{
    return qAlpha( rgbValues[0] ) || qAlpha( rgbValues[1] )
        || qAlpha( rgbValues[2] ) || qAlpha( rgbValues[3] );
}

static inline bool qskIsMonochrome( const QRgb* rgbValues )
{
    return ( rgbValues[0] == rgbValues[1] )
        && ( rgbValues[1] == rgbValues[2] )
        && ( rgbValues[2] == rgbValues[3] );
}

static inline uint qskMetricsHash( const QskBoxOptions& options )
{
    uint hash = 13000;

    hash = options.shape.hash( hash );
    return options.border.hash( hash );
}

static inline uint qskColorsHash( const QskBoxOptions& options )
{
    uint hash = 13000;
    hash = options.borderColors.hash( hash );
    return options.fillGradient.hash( hash );
}

QskBoxNode::QskBoxNode():
    m_metricsHash( 0 ),
    m_colorsHash( 0 ),
    m_geometry( QSGGeometry::defaultAttributes_ColoredPoint2D(), 0 )
{
    setMaterial( &qskMaterialVertex );
    setGeometry( &m_geometry );
}

QskBoxNode::~QskBoxNode()
{
    if ( material() != &qskMaterialVertex )
        delete material();
}

void QskBoxNode::setBoxData( const QRectF& rect, const QskBoxOptions& options )
{
#if 1
    const uint metricsHash = qskMetricsHash( options );
    const uint colorsHash = qskColorsHash( options );

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

    const auto& gradient = options.fillGradient;
    const auto& borderMetrics = options.border;
    const auto& borderColors = options.borderColors;

    bool hasFill = options.fillGradient.isValid();

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

    const bool isFillMonochrome = hasFill ? gradient.isMonochrome() : true;
    const bool isBorderMonochrome = hasBorder ? borderColors.isMonochrome() : true;

    if ( hasFill && hasBorder )
    {
        if ( isFillMonochrome && isBorderMonochrome )
        {
            if ( borderColors.color( Qsk::Left ) == gradient.startColor() )
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
        if ( ( hasFill && hasBorder ) || ( hasFill && !isFillMonochrome )
            || ( hasBorder && !isBorderMonochrome ) )
        {
            maybeFlat = false;
        }
    }

    QskBoxRenderer renderer;

    if ( !maybeFlat )
    {
        setMonochrome( false );

        renderer.renderBox( m_rect, options.shape, options.border,
            options.borderColors, options.fillGradient, *geometry() );
    }
    else
    {
        // all is done with one color
        setMonochrome( true );

        auto* flatMaterial = static_cast< QSGFlatColorMaterial *>( material() );

        if ( hasFill )
        {
            flatMaterial->setColor( gradient.startColor() );
            renderer.renderFill( m_rect, options.shape,
                QskBoxBorderMetrics(), *geometry() );
        }
        else
        {
            flatMaterial->setColor( borderColors.color( Qsk::Left ).rgba() );
            renderer.renderBorder( m_rect, options.shape,
                options.border, *geometry() );
        }
    }
}

void QskBoxNode::setMonochrome( bool on )
{
    const auto material = this->material();

    if ( on == ( material != &qskMaterialVertex ) )
        return;

    m_geometry.allocate( 0 );

    if ( on )
    {
        setMaterial( new QSGFlatColorMaterial() );

        const QSGGeometry g( QSGGeometry::defaultAttributes_Point2D(), 0 );
        memcpy( &m_geometry, &g, sizeof( QSGGeometry ) );
    }
    else
    {
        setMaterial( &qskMaterialVertex );
        delete material;

        const QSGGeometry g( QSGGeometry::defaultAttributes_ColoredPoint2D(), 0 );
        memcpy( &m_geometry, &g, sizeof( QSGGeometry ) );
    }
}
