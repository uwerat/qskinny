/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskVertexRenderer.h"
#include "QskArcIterator.h"
#include <QSGGeometry>

namespace
{
    class Line2D
    {
    public:
        inline void setLine( float x1, float y1, float x2, float y2 )
        {
            p1.set( x1, y1 );
            p2.set( x2, y2 );
        }

        inline void setHLine( float x1, float x2, float y )
        {
            p1.x = x1;
            p2.x = x2;
            p1.y = p2.y = y;
        }

        inline void setVLine( float x, float y1, float y2 )
        {
            p1.x = p2.x = x;
            p1.y = y1;
            p2.y = y2;
        }

        QSGGeometry::Point2D p1;
        QSGGeometry::Point2D p2;
    };

    Line2D* allocateLines( QSGGeometry* geometry, int lineCount )
    {
        geometry->allocate( 2 * lineCount ); // 2 points per line
        return reinterpret_cast< Line2D* >( geometry->vertexData() );
    }

}

QskVertexRenderer::QskVertexRenderer():
    m_geometry( nullptr )
{
}

QskVertexRenderer::QskVertexRenderer( QSGGeometry& geometry ):
    m_geometry( nullptr )
{
    begin( geometry );
}

void QskVertexRenderer::begin( QSGGeometry& geometry )
{
    // check for Point2D TODO ...
    m_geometry = &geometry;
}

void QskVertexRenderer::end()
{
    m_geometry = nullptr;
}

void QskVertexRenderer::renderRoundedRect( const QRectF& rect, double radius )
{
    if ( m_geometry == nullptr )
        return;

    if ( rect.isEmpty() )
    {
        clearGeometry();
        return;
    }

    if ( radius <= 0.0 )
    {
        renderRect( rect );
        return;
    }

    stippleRoundedRect( rect, radius );
}

void QskVertexRenderer::renderBorder( const QRectF& rect, double borderWidth )
{
    if ( m_geometry == nullptr )
        return;

    if ( rect.isEmpty() || borderWidth <= 0.0 )
    {
        clearGeometry();
        return;
    }

    const double dim = 0.5 * qMin( rect.width(), rect.height() );
    if ( borderWidth < dim )
    {
        stippleBorder( rect, borderWidth );
    }
    else
    {
        // degenerated to a rectangle
        stippleRect( rect );
    }
}

void QskVertexRenderer::renderRect( const QRectF& rect )
{
    if ( m_geometry == nullptr )
        return;

    if ( rect.isEmpty() )
    {
        clearGeometry();
        return;
    }

    stippleRect( rect );
}

void QskVertexRenderer::renderRoundedBorder(
    const QRectF& rect, double radius, double borderWidth )
{
    if ( m_geometry == nullptr )
        return;

    if ( radius <= 0.0 )
    {
        renderBorder( rect, borderWidth );
        return;
    }

    const double dim = 0.5 * qMin( rect.width(), rect.height() );
    if ( radius > dim )
        radius = dim;


    if ( borderWidth < dim )
    {
        stippleRoundedBorder( rect, radius, borderWidth );
    }
    else
    {
        // degenerated to a rounded rectangle
        stippleRoundedRect( rect, radius );
    }
}

void QskVertexRenderer::stippleRect( const QRectF& rect )
{
    Line2D* lines = ::allocateLines( m_geometry, 2 );
    lines[0].setHLine( rect.left(), rect.right(), rect.top() );
    lines[1].setHLine( rect.left(), rect.right(), rect.bottom() );
}

void QskVertexRenderer::stippleBorder( const QRectF& rect, double borderWidth )
{
    const QRectF r = rect.adjusted( borderWidth, borderWidth,
            -borderWidth, -borderWidth );

    Line2D* lines = ::allocateLines( m_geometry, 5 );
    lines[0].setLine( r.left(), r.top(), rect.left(), rect.top() );
    lines[1].setLine( r.right(), r.top(), rect.right(), rect.top() );
    lines[2].setLine( r.right(), r.bottom(), rect.right(), rect.bottom() );
    lines[3].setLine( r.left(), r.bottom(), rect.left(), rect.bottom() );
    lines[4] = lines[0];
}

void QskVertexRenderer::stippleRoundedRect( const QRectF& rect, double radius )
{
    const int numSteps = QskArcIterator::segmentHint( radius );
    const int numLines = 2 * ( numSteps + 1 ); // top + bottom -> 2

    Line2D* lines = ::allocateLines( m_geometry, numLines );

    // coordinates of the inner rect
    const double left = rect.left() + radius;
    const double right = rect.right() - radius;
    const double top = rect.top() + radius;
    const double bottom = rect.bottom() - radius;

    // could also be done with M_PI_4 iteration only -> TODO ...

    for ( QskArcIterator it( M_PI_2, numSteps ); !it.isDone(); ++it )
    {
        const double dx = radius * it.cos();
        const double dy = radius * it.sin();

        const int j = it.step();
        const int k = ( numLines - 1 ) - j;

        lines[j].setHLine( left - dx, right + dx, top - dy );
        lines[k].setHLine( left - dx, right + dx, bottom + dy );
    }
}

void QskVertexRenderer::stippleRoundedBorder(
    const QRectF& rect, double radius, double borderWidth )
{
    const double left = rect.left() + radius;
    const double right = rect.right() - radius;
    const double top = rect.top() + radius;
    const double bottom = rect.bottom() - radius;

    const int segmentCount = qCeil( 0.5 * QskArcIterator::segmentHint( radius ) );
    const int arcLineCount = 2 * segmentCount + 1; // [0,45[, ]45, 90], one line at 45
    const int lineCount = 4 * arcLineCount + 1; // 4 arcs + the closing line

    Line2D* lines = allocateLines( m_geometry, lineCount );

    Line2D* linesBR = lines;
    Line2D* linesTR = linesBR + arcLineCount;
    Line2D* linesTL = linesTR + arcLineCount;
    Line2D* linesBL = linesTL + arcLineCount;

    for ( QskArcIterator it( 0.5 * M_PI_2, segmentCount ); !it.isDone(); ++it )
    {
        const double rcO = radius * it.sin();
        const double rsO = radius * it.cos();

        double rcI = radius - borderWidth;
        double rsI = radius - borderWidth;

        if ( radius > borderWidth ) // needed when having a non opaque color
        {
            rcI *= it.sin();
            rsI *= it.cos();
        }

        const int i = it.step();

        // inner -> outer

        linesTR[i].setLine( right + rcI, top - rsI, right + rcO, top - rsO );
        linesTL[i].setLine( left - rsI, top - rcI, left - rsO, top - rcO );

        linesBL[i].setLine( left - rcI, bottom + rsI, left - rcO, bottom + rsO );
        linesBR[i].setLine( right + rsI, bottom + rcI, right + rsO, bottom + rcO );

        if ( i != segmentCount ) // leading/trailing lines meet at 45 degrees
        {
            const int l = arcLineCount - 1 - i;

            linesTL[l].setLine( left - rcI, top - rsI, left - rcO, top - rsO );
            linesTR[l].setLine( right + rsI, top - rcI, right + rsO, top - rcO );
            linesBL[l].setLine( left - rsI, bottom + rcI, left - rsO, bottom + rcO );
            linesBR[l].setLine( right + rcI, bottom + rsI, right + rcO, bottom + rsO );
        }
    }

    lines[lineCount - 1] = lines[0]; // closing the border
}

void QskVertexRenderer::clearGeometry()
{
    if ( m_geometry )
        m_geometry->allocate( 0 );
}
