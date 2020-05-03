/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_VERTEX_H
#define QSK_VERTEX_H

#include "QskGlobal.h"

#include <qcolor.h>
#include <qsggeometry.h>

namespace QskVertex
{
    class QSK_EXPORT Color
    {
      public:
        constexpr Color() noexcept;

        constexpr Color( unsigned char red, unsigned char green,
            unsigned char blue, unsigned char alpha ) noexcept;

        Color( QRgb ) noexcept;
        Color( const QColor& );

        Color interpolatedTo( Color, double ratio ) const noexcept;

        constexpr bool operator==( const Color& ) const noexcept;
        constexpr bool operator!=( const Color& ) const noexcept;

        unsigned char r, g, b, a;
    };

    class QSK_EXPORT Line
    {
      public:
        inline void setLine( float x1, float y1, float x2, float y2 ) noexcept
        {
            p1.set( x1, y1 );
            p2.set( x2, y2 );
        }

        inline void setHLine( float x1, float x2, float y ) noexcept
        {
            setLine( x1, y, x2, y );
        }

        inline void setVLine( float x, float y1, float y2 ) noexcept
        {
            setLine( x, y1, x, y2 );
        }

        inline void setLine( float x1, float y1, float x2, float y2, Color ) noexcept
        {
            /* The color parameter makes no sense, but is useful
               when being using from templated code
             */
            setLine( x1, y1, x2, y2 );
        }

        QSGGeometry::Point2D p1;
        QSGGeometry::Point2D p2;
    };

    class QSK_EXPORT ColoredLine
    {
      public:
        inline void setLine( float x1, float y1, Color c1,
            float x2, float y2, Color c2 ) noexcept
        {
            p1.set( x1, y1, c1.r, c1.g, c1.b, c1.a );
            p2.set( x2, y2, c2.r, c2.g, c2.b, c2.a );
        }

        inline void setLine( float x1, float y1, float x2, float y2, Color color ) noexcept
        {
            setLine( x1, y1, color, x2, y2, color );
        }

        inline void setHLine( qreal x1, qreal x2, qreal y, Color color ) noexcept
        {
            setLine( x1, y, color, x2, y, color );
        }

        inline void setVLine( qreal x, qreal y1, qreal y2, Color color ) noexcept
        {
            setLine( x, y1, color, x, y2, color );
        }

        QSGGeometry::ColoredPoint2D p1;
        QSGGeometry::ColoredPoint2D p2;
    };

    template< class Line >
    static inline Line* allocateLines( QSGGeometry& geometry, int lineCount )
    {
        geometry.allocate( 2 * lineCount ); // 2 points per line
        return reinterpret_cast< Line* >( geometry.vertexData() );
    }

    void QSK_EXPORT debugGeometry( const QSGGeometry& );

    inline constexpr Color::Color() noexcept
        : r( 0 )
        , g( 0 )
        , b( 0 )
        , a( 255 )
    {
    }

    inline constexpr Color::Color( unsigned char red, unsigned char green,
            unsigned char blue, unsigned char alpha ) noexcept
        : r( red )
        , g( green )
        , b( blue )
        , a( alpha )
    {
    }

    inline Color::Color( QRgb rgb ) noexcept
    {
        r = qRed( rgb );
        g = qGreen( rgb );
        b = qBlue( rgb );
        a = qAlpha( rgb );

        if ( a < 255 )
        {
            const double af = a / 255.0;

            r *= af;
            g *= af;
            b *= af;
        }
    }

    inline Color::Color( const QColor& color )
        : Color( color.rgba() )
    {
    }

    inline Color Color::interpolatedTo( Color colorTo, double ratio ) const noexcept
    {
        if ( ratio <= 0.0 )
            return *this;

        if ( ratio >= 1.0 )
            return colorTo;

        const double t = ratio;
        const double rt = 1.0 - ratio;

        return Color( rt * r + t * colorTo.r, rt * g + t * colorTo.g,
            rt * b + t * colorTo.b, rt * a + t * colorTo.a );
    }

    inline constexpr bool Color::operator==( const Color& other ) const noexcept
    {
        return ( r == other.r ) && ( g == other.g )
               && ( b == other.b ) && ( a == other.a );
    }

    inline constexpr bool Color::operator!=( const Color& other ) const noexcept
    {
        return !( *this == other );
    }
}

#ifndef QT_NO_DEBUG_STREAM
class QDebug;
QDebug operator<<( QDebug debug, QskVertex::Color );
QDebug operator<<( QDebug debug, QskVertex::ColoredLine );
QDebug operator<<( QDebug debug, QskVertex::Line );
#endif

#endif
