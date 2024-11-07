/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_VERTEX_H
#define QSK_VERTEX_H

#include "QskGlobal.h"

#include <qcolor.h>
#include <qsggeometry.h>
#include <qline.h>

namespace QskVertex
{
    class Color
    {
      public:
        constexpr Color() noexcept;

        constexpr Color( unsigned char red, unsigned char green,
            unsigned char blue, unsigned char alpha ) noexcept;

        Color( QRgb ) noexcept;
        Color( const QColor& );

        Color interpolatedTo( Color, qreal ratio ) const noexcept;

        constexpr bool operator==( const Color& ) const noexcept;
        constexpr bool operator!=( const Color& ) const noexcept;

        unsigned char r, g, b, a;
    };

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
            const auto af = a / 255.0;

            r *= af;
            g *= af;
            b *= af;
        }
    }

    inline Color::Color( const QColor& color )
        : Color( color.rgba() )
    {
    }

    inline Color Color::interpolatedTo( Color colorTo, qreal ratio ) const noexcept
    {
        if ( ratio <= 0.0 )
            return *this;

        if ( ratio >= 1.0 )
            return colorTo;

        const auto t = ratio;
        const auto rt = 1.0 - ratio;

        return Color(
            static_cast< unsigned char >( rt * r + t * colorTo.r ),
            static_cast< unsigned char >( rt * g + t * colorTo.g ),
            static_cast< unsigned char >( rt * b + t * colorTo.b ),
            static_cast< unsigned char >( rt * a + t * colorTo.a ) 
        );
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

namespace QskVertex
{
    class Line
    {
      public:
        inline void setLine( float x1, float y1, float x2, float y2 ) noexcept
        {
            p1.set( x1, y1 );
            p2.set( x2, y2 );
        }

        inline void setLine( const QPointF& p1, const QPointF& p2 ) noexcept
        {
            setLine( p1.x(), p1.y(), p2.x(), p2.y() );
        }

        inline void setHLine( float x1, float x2, float y ) noexcept
        {
            setLine( x1, y, x2, y );
        }

        inline void setVLine( float x, float y1, float y2 ) noexcept
        {
            setLine( x, y1, x, y2 );
        }

        inline void setLine( const QPointF& p1, const QPointF& p2, Color ) noexcept
        {
            /* The color parameter makes no sense, but is useful
               when being used from templated code
             */

            setLine( p1.x(), p1.y(), p2.x(), p2.y() );
        }

        inline void setLine( float x1, float y1, float x2, float y2, Color ) noexcept
        {
            /* The color parameter makes no sense, but is useful
               when being used from templated code
             */
            setLine( x1, y1, x2, y2 );
        }

        inline float x1() const noexcept { return p1.x; }
        inline float y1() const noexcept { return p1.y; }

        inline float x2() const noexcept { return p2.x; }
        inline float y2() const noexcept { return p2.y; }

        inline float dx() const noexcept { return p2.x - p1.x; }
        inline float dy() const noexcept { return p2.y - p1.y; }

        QSGGeometry::Point2D p1;
        QSGGeometry::Point2D p2;
    };

    class ColoredLine
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

        inline void setLine( const QPointF& p1, const QPointF& p2, Color color ) noexcept
        {
            setLine( p1.x(), p1.y(), color, p2.x(), p2.y(), color );
        }

        inline void setLine( const QLineF& line, Color color ) noexcept
        {
            setLine( line.x1(), line.y1(), color, line.x2(), line.y2(), color );
        }

        inline void setHLine( qreal x1, qreal x2, qreal y, Color color ) noexcept
        {
            setLine( x1, y, color, x2, y, color );
        }

        inline void setVLine( qreal x, qreal y1, qreal y2, Color color ) noexcept
        {
            setLine( x, y1, color, x, y2, color );
        }

        inline float x1() const noexcept { return p1.x; }
        inline float y1() const noexcept { return p1.y; }

        inline float x2() const noexcept { return p2.x; }
        inline float y2() const noexcept { return p2.y; }

        inline float dx() const noexcept { return p2.x - p1.x; }
        inline float dy() const noexcept { return p2.y - p1.y; }

        QSGGeometry::ColoredPoint2D p1;
        QSGGeometry::ColoredPoint2D p2;
    };

    template< class Line >
    static inline Line* allocateLines( QSGGeometry& geometry, int lineCount )
    {
        geometry.allocate( 2 * lineCount ); // 2 points per line
        return reinterpret_cast< Line* >( geometry.vertexData() );
    }
}

namespace QskVertex
{
    void debugGeometry( const QSGGeometry& );
}

#ifndef QT_NO_DEBUG_STREAM

    class QDebug;

    QDebug operator<<( QDebug debug, QskVertex::Color );
    QDebug operator<<( QDebug debug, const QskVertex::ColoredLine& );
    QDebug operator<<( QDebug debug, const QskVertex::Line& );

#endif

#endif
