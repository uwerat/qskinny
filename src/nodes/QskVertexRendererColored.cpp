/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskVertexRendererColored.h"
#include "QskVertexColor.h"
#include "QskArcIterator.h"
#include <QLinearGradient>
#include <QSGGeometry>
#include <QDebug>

namespace
{
#ifndef QT_NO_DEBUG_STREAM

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Wunused-function")

    void debugGeometry( const QSGGeometry& geometry )
    {
        const QSGGeometry::ColoredPoint2D* vertices = geometry.vertexDataAsColoredPoint2D();

        bool doVLines = false;
        if ( geometry.vertexCount() % 2 == 0 )
        {
            doVLines = true;

            for ( int i = 0; i < geometry.vertexCount(); i += 2 )
            {
                if ( vertices[i].y != vertices[i + 1].y )
                {
                    doVLines = false;
                    break;
                }
            }
        }

        if ( doVLines )
        {
            for ( int i = 0; i < geometry.vertexCount(); i += 2 )
            {
                const QSGGeometry::ColoredPoint2D& v = vertices[i];
                qDebug() << i / 2 << ":" << qRound( v.x ) << qRound( vertices[i + 1].x )
                         << qRound( v.y );
            }
        }
        else
        {
            for ( int i = 0; i < geometry.vertexCount(); i++ )
            {
                const QSGGeometry::ColoredPoint2D& v = vertices[i];
                qDebug() << i / 2 << ":" << qRound( v.x ) << qRound( v.y );
            }
        }
    }

QT_WARNING_POP
#endif

    class GradientMap
    {
    public:
        GradientMap( double min, double max, const QskGradient& gradient ):
            m_index( 0 ),
            m_min( min ),
            m_max( max ),
            m_stops( gradient.stops() )
        {
            incrementStop();
        }

        inline bool contains( double value ) const
        {
            if ( m_index == m_stops.size() - 1 )
                return true; // we are at the end of the gardient

            return value <= m_map.value2();
        }

        void incrementStop()
        {
            const QskGradientStop& s1 = m_stops[m_index];
            const QskGradientStop& s2 = m_stops[++m_index];

            m_map.setRange( toRange( s1 ), toRange( s2 ) );
            m_map.setColors( s1.color(), s2.color() );
        }

        double currentValue() const
        {
            return m_map.value1();
        }

        QskVertexColor currentColor() const
        {
            return m_map.color1();
        }

        inline QskVertexColor colorAt( double value ) const
        {
            return m_map.colorAt( value );
        }

    private:
        inline double toRange( const QskGradientStop& stop ) const
        {
            return m_min + ( m_max - m_min ) * stop.position();
        }

        int m_index;

        const double m_min;
        const double m_max;

        const QVector< QskGradientStop > m_stops;
        QskVertexColorMap m_map;
    };

    class LineC
    {
    public:
        inline void setLine( float x1, float y1, QskVertexColor c1,
            float x2, float y2, QskVertexColor c2 )
        {
            p1.set( x1, y1, c1.r, c1.g, c1.b, c1.a );
            p2.set( x2, y2, c2.r, c2.g, c2.b, c2.a );
        }

        inline void setLine( float x1, float y1, float x2, float y2, QskVertexColor color )
        {
            p1.set( x1, y1, color.r, color.g, color.b, color.a );
            p2.set( x2, y2, color.r, color.g, color.b, color.a );
        }

        inline void setHLine( double x1, double x2, double y, QskVertexColor color )
        {
            p1.set( x1, y, color.r, color.g, color.b, color.a );
            p2.set( x2, y, color.r, color.g, color.b, color.a );
        }

        inline void setInterpolatedHLine( const LineC& lineStart,
            double x1To, double x2To, double y, double yTo, QskVertexColor color )
        {
            const double f = ( y - lineStart.p1.y ) / ( yTo - lineStart.p1.y );
            const double x1 = lineStart.p1.x + f * ( x1To - lineStart.p1.x );
            const double x2 = lineStart.p2.x + f * ( x2To - lineStart.p2.x );

            setHLine( x1, x2, y, color );
        }

        inline void setInterpolatedVLine( const LineC& lineStart,
            double x, double xTo, double y1To, double y2To, QskVertexColor color )
        {
            const double f = ( x - lineStart.p1.x ) / ( xTo - lineStart.p1.x );
            const double y1 = lineStart.p1.y + f * ( y1To - lineStart.p1.y );
            const double y2 = lineStart.p2.y + f * ( y2To - lineStart.p2.y );

            setVLine( x, y1, y2, color );
        }


        inline void setVLine( double x, double y1, double y2, QskVertexColor color )
        {
            p1.set( x, y1, color.r, color.g, color.b, color.a );
            p2.set( x, y2, color.r, color.g, color.b, color.a );
        }

        QSGGeometry::ColoredPoint2D p1;
        QSGGeometry::ColoredPoint2D p2;
    };

    inline LineC* vertexLines( QSGGeometry* geometry )
    {
        return reinterpret_cast< LineC* >( geometry->vertexData() );
    }

#if 0
    int vertexLineCount( QSGGeometry* geometry )
    {
        return geometry->vertexCount() / 2;
    }
#endif

    inline int lineCountBorder( const QskVertexPen& pen )
    {
        // We can build a rectangular border from the 4 diagonal
        // lines at the corners, but need an additional line
        // for closing the border.

        int n = 4 + 1;
        if ( !pen.isMonochrome() )
        {
            // we need to add 2 extra lines at the corners
            // where the colors are changing: see stippleBorder()
            // unfortunately the closing corner is none of them.

            n += 2;
        }

        return n;
    }

    QPair< QskVertexColor, QskVertexColor > effectivePenColors( const QskVertexPen& pen )
    {
        const QskVertexColor light = pen.lightColor();
        const QskVertexColor dark = pen.darkColor();

        if ( light != dark )
        {
            const double f = 0.5 * ( pen.shadeFactor() + 1.0 );
            return qMakePair( light.interpolatedTo( dark, f ),
                dark.interpolatedTo( light, f ) );
        }
        else
        {
            return qMakePair( dark, light );
        }
    }

    int gradientLineCount( const QskGradient& gradient )
    {
        const QVector< QskGradientStop >& stops = gradient.stops();

        int n = gradient.stops().count();

        if ( gradient.orientation() == QskGradient::Diagonal )
        {
            // check if we have a stop at 0.5, otherwise we need
            // to add an extra line

            for ( int i = 1; i < stops.size(); i++ )
            {
                if ( stops[i].position() == 0.5 )
                    break;

                if ( stops[i].position() > 0.5 )
                {
                    n++;
                    break;
                }
            }
        }

        return n;
    }
}

QskVertexRendererColored::QskVertexRendererColored():
    m_geometry( nullptr )
{
}

QskVertexRendererColored::QskVertexRendererColored( QSGGeometry& geometry ):
    m_geometry( nullptr )
{
    begin( geometry );
}

void QskVertexRendererColored::begin( QSGGeometry& geometry )
{
    // check for ColoredPoint2D TODO ...
    m_geometry = &geometry;
}

void QskVertexRendererColored::end()
{
    m_geometry = nullptr;
}

void QskVertexRendererColored::setPen( const QskVertexPen& pen )
{
    m_pen = pen;
}

const QskVertexPen& QskVertexRendererColored::pen() const
{
    return m_pen;
}

void QskVertexRendererColored::setBrush( const QskGradient& gradient )
{
    m_gradient = gradient;
}

const QskGradient& QskVertexRendererColored::brush() const
{
    return m_gradient;
}

void QskVertexRendererColored::renderRect( const QRectF& rect )
{
    renderRoundedRect( rect, 0.0 );
}

void QskVertexRendererColored::renderRoundedRect( const QRectF& rect, double radius )
{
    if ( m_geometry == nullptr )
        return;

    const double dim = 0.5 * qMin( rect.width(), rect.height() );

    const bool hasPen = m_pen.isVisible();

    bool hasBrush = m_gradient.isVisible();
    if ( m_pen.isValid() && ( m_pen.width() >= dim ) )
        hasBrush = false;

    if ( rect.isEmpty() || !( hasPen || hasBrush ) )
    {
        m_geometry->allocate( 0 );
        return;
    }

    int offsetBorder = 0;
    int numLines = 0;

    if ( radius > 0.0 )
    {
        const int arcStepCount = QskArcIterator::segmentHint( radius );
        const int numSteps = 2 * ( arcStepCount + 1 ); // top + bottom corners

        if ( hasBrush )
        {
            offsetBorder = numSteps + m_gradient.stops().size() - 2;
            numLines += offsetBorder;
        }

        if ( hasPen )
        {
            numLines += 2 * numSteps + 1;
        }
    }
    else
    {
        if ( hasBrush )
        {
            offsetBorder = ::gradientLineCount( m_gradient );
            numLines += offsetBorder;
        }

        if ( hasPen )
        {
            numLines += ::lineCountBorder( m_pen );
        }
    }

    m_geometry->allocate( 2 * numLines ); // 2 points per line

    if ( radius > 0.0 )
    {
        if ( hasBrush )
            stippleRoundedRect( 0, rect, radius );

        if ( hasPen )
            stippleRoundedBorder( offsetBorder, rect, radius );
    }
    else
    {
        if ( hasBrush )
        {
            QRectF fillRect = rect;

            if ( hasPen )
            {
                const double bw = m_pen.width();
                fillRect.adjust( bw, bw, -bw, -bw );
            }

            stippleRect( 0, fillRect );
        }

        if ( hasPen )
        {
            stippleBorder( offsetBorder, rect );
        }
    }
}

void QskVertexRendererColored::stippleRect( int offset, const QRectF& rect )
{
    LineC* lines = ::vertexLines( m_geometry ) + offset;
    const QVector< QskGradientStop >& stops = m_gradient.stops();

    const double x1 = rect.left();
    const double x2 = rect.right();
    const double w = rect.width();

    const double y1 = rect.top();
    const double y2 = rect.bottom();
    const double h = rect.height();

    switch( m_gradient.orientation() )
    {
        case QskGradient::Diagonal:
        {
            LineC* line = lines;
            for ( int i = 0; i < stops.size(); i++ )
            {
                const QskGradientStop& s = stops[i];
                const double pos = s.position();

                if ( i > 0 && pos > 0.5 && stops[i - 1].position() < 0.5 )
                {
                    // we have to add an extra line
                    // at 0.5 - for the shape, not the color

                    QskVertexColorMap colorMap;
                    colorMap.setRange( stops[i - 1].position(), pos );
                    colorMap.setColors( stops[i - 1].color(), s.color() );

                    ( *line++ ).setLine( x2, y1, x1, y2, colorMap.colorAt( 0.5 ) );
                }

                if ( pos <= 0.5 )
                {
                    const double f = 2 * pos;
                    ( *line++ ).setLine( x1 + f * w, y1, x1, y1 + f * h, s.color() );
                }
                else
                {
                    const double f = 2 * ( pos - 0.5 );
                    ( *line++ ).setLine( x2, y1 + f * h, x2 - f * w, y2, s.color() );
                }
            }

            break;
        }
        case QskGradient::Horizontal:
        {
            for ( int i = 0; i < stops.size(); i++ )
            {
                const QskGradientStop& s = stops[i];
                lines[i].setVLine( x1 + s.position() * w, y2, y1, s.color() );
            }
            break;
        }
        case QskGradient::Vertical:
        default:
        {
            for ( int i = 0; i < stops.size(); i++ )
            {
                const QskGradientStop& s = stops[i];
                lines[i].setHLine( x1, x2, y1 + s.position() * h, s.color() );
            }
            break;
        }
    }
}

void QskVertexRendererColored::stippleBorder( int offset, const QRectF& rect )
{
    const double dx = qMin( m_pen.width(), 0.5 * rect.width() );
    const double dy = qMin( m_pen.width(), 0.5 * rect.height() );

    const QRectF r = rect.adjusted( dx, dy, -dx, -dy );;

    const QPair< QskVertexColor, QskVertexColor > c = effectivePenColors( m_pen );

    LineC* lines = ::vertexLines( m_geometry ) + offset;

    LineC* line = lines;
    ( *line++ ).setLine( r.right(), r.bottom(), rect.right(), rect.bottom(), c.first );
    ( *line++ ).setLine( r.left(), r.bottom(), rect.left(), rect.bottom(), c.first );

    if ( c.first != c.second )
        ( *line++ ).setLine( r.left(), r.bottom(), rect.left(), rect.bottom(), c.second );

    ( *line++ ).setLine( r.left(), r.top(), rect.left(), rect.top(), c.second );
    ( *line++ ).setLine( r.right(), r.top(), rect.right(), rect.top(), c.second );

    if ( c.first != c.second )
        ( *line++ ).setLine( r.right(), r.top(), rect.right(), rect.top(), c.first );

    *line = lines[0];
}

void QskVertexRendererColored::stippleRoundedRect( int offset,
    const QRectF& rect, double radius )
{
    const double r = radius - m_pen.width();

    const double left = rect.left() + radius;
    const double right = rect.right() - radius;
    const double top = rect.top() + radius;
    const double bottom = rect.bottom() - radius;

    const int arcStepCount = QskArcIterator::segmentHint( radius );
    const int numSteps = 2 * ( arcStepCount + 1 ); // top + bottom corners

    const int orientation = m_gradient.isMonochrome() ? -1 : m_gradient.orientation();

    switch( orientation )
    {
        case QskGradient::Diagonal:
        case QskGradient::Horizontal:
        {
            GradientMap map( rect.left(), rect.right(), m_gradient );

            LineC* lines = ::vertexLines( m_geometry ) + offset;

            for ( QskArcIterator it( M_PI_2, arcStepCount, true ); !it.isDone(); ++it )
            {
                const double rc = r * it.cos();
                const double rs = r * it.sin();

                const double y1 = top - rs;
                const double y2 = bottom + rs;

                const double x = left - rc;
                while ( !map.contains( x ) )
                {
                    map.incrementStop();

                    lines->setInterpolatedVLine( lines[-1],
                        map.currentValue(), x, y1, y2, map.currentColor() );
                    lines++;
                }

                ( *lines++ ).setVLine( x, y1, y2, map.colorAt( x ) );
            }

            for ( QskArcIterator it( M_PI_2, arcStepCount, false ); !it.isDone(); ++it )
            {
                const double rc = r * it.cos();
                const double rs = r * it.sin();

                const double y1 = top - rs;
                const double y2 = bottom + rs;

                const double x = right + rc;
                while ( !map.contains( x ) )
                {
                    map.incrementStop();

                    lines->setInterpolatedVLine( lines[-1],
                        map.currentValue(), x, y1, y2, map.currentColor() );
                    lines++;
                }

                ( *lines++ ).setVLine( x, y1, y2, map.colorAt( x ) );
            }

            break;
        }
        case QskGradient::Vertical:
        {
            GradientMap map( rect.top(), rect.bottom(), m_gradient );

            LineC* lines = ::vertexLines( m_geometry ) + offset;

            for ( QskArcIterator it( M_PI_2, arcStepCount, false ); !it.isDone(); ++it )
            {
                const double rc = r * it.cos();
                const double rs = r * it.sin();

                const double x1 = left - rc;
                const double x2 = right + rc;

                const double y = top - rs;
                while ( !map.contains( y ) )
                {
                    map.incrementStop();

                    lines->setInterpolatedHLine( lines[-1],
                        x1, x2, map.currentValue(), y, map.currentColor() );
                    lines++;
                }

                ( *lines++ ).setHLine( x1, x2, y, map.colorAt( y ) );
            }

            for ( QskArcIterator it( M_PI_2, arcStepCount, true ); !it.isDone(); ++it )
            {
                const double rc = r * it.cos();
                const double rs = r * it.sin();

                const double x1 = left - rc;
                const double x2 = right + rc;

                const double y = bottom + rs;
                while ( !map.contains( y ) )
                {
                    map.incrementStop();

                    lines->setInterpolatedHLine( lines[-1],
                        x1, x2, map.currentValue(), y, map.currentColor() );
                    lines++;
                }

                ( *lines++ ).setHLine( x1, x2, y, map.colorAt( y ) );
            }

            break;
        }
        default:
        {
            const QskVertexColor c = m_gradient.stops().first().color();

            LineC* linesT = ::vertexLines( m_geometry ) + offset;
            LineC* linesB = linesT + numSteps - 1;

            for ( QskArcIterator it( M_PI_2, arcStepCount ); !it.isDone(); ++it )
            {
                const double rc = r * it.cos();
                const double rs = r * it.sin();

                ( *linesT++ ).setHLine( left - rc, right + rc, top - rs, c );
                ( *linesB-- ).setHLine( left - rc, right + rc, bottom + rs, c );
            }
        }
    }
}

void QskVertexRendererColored::stippleRoundedBorder(
    int offset, const QRectF& rect, double radius )
{
    const double dx = qMin( m_pen.width(), 0.5 * rect.width() );
    const double dy = qMin( m_pen.width(), 0.5 * rect.height() );

    const double outerRadius = radius;
    const double innerRadius = radius - qMin( dx, dy );

    const double left = rect.left() + outerRadius;
    const double right = rect.right() - outerRadius;
    const double top = rect.top() + outerRadius;
    const double bottom = rect.bottom() - outerRadius;

    const int arcStepCount = QskArcIterator::segmentHint( outerRadius );
    const int numSteps = 2 * ( arcStepCount + 1 ); // top + bottom corners

    const QPair< QskVertexColor, QskVertexColor > colors = effectivePenColors( m_pen );

    QskVertexColorMap colorMap;
    colorMap.setRange( 0, arcStepCount );
    colorMap.setColors( colors.first, colors.second );

    if ( m_gradient.orientation() != QskGradient::Horizontal )
    {
        LineC* linesR = vertexLines( m_geometry ) + offset;
        LineC* linesL = linesR + numSteps;

        for ( QskArcIterator it( M_PI_2, arcStepCount, false ); !it.isDone(); ++it )
        {
            const double rcI = innerRadius * it.cos();
            const double rsI = innerRadius * it.sin();
            const double rcO = outerRadius * it.cos();
            const double rsO = outerRadius * it.sin();

            const int j = it.step();
            const int k = numSteps - 1 - j;

            linesR[j].setLine( right + rcI, bottom + rsI,
                right + rcO, bottom + rsO, colors.first );

            linesR[k].setLine( right + rcI, top - rsI,
                right + rcO, top - rsO, colorMap.colorAt( arcStepCount - j ) );

            linesL[j].setLine( left - rcI, top - rsI,
                left - rcO, top - rsO, colors.second );

            linesL[k].setLine( left - rcI, bottom + rsI,
                left - rcO, bottom + rsO, colorMap.colorAt( j ) );
        }

        linesL[numSteps] = linesR[0];
    }
    else
    {
        LineC* linesB = vertexLines( m_geometry ) + offset;
        LineC* linesT = linesB + numSteps;

        for ( QskArcIterator it( M_PI_2, arcStepCount, true ); !it.isDone(); ++it )
        {
            const double rcI = innerRadius * it.cos();
            const double rsI = innerRadius * it.sin();
            const double rcO = outerRadius * it.cos();
            const double rsO = outerRadius * it.sin();

            const int j = it.step();
            const int k = numSteps - 1 - j;

            linesB[j].setLine( right + rcI, bottom + rsI,
                right + rcO, bottom + rsO, colors.first );

            linesB[k].setLine( left - rcI, bottom + rsI,
                left - rcO, bottom + rsO, colorMap.colorAt( arcStepCount - j ) );

            linesT[j].setLine( left - rcI, top - rsI,
                left - rcO, top - rsO, colors.second );

            linesT[k].setLine( right + rcI, top - rsI,
                right + rcO, top - rsO, colorMap.colorAt( j ) );
        }

        linesT[numSteps] = linesB[0];
    }
}
