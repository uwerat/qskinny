/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_VERTEX_H
#define QSK_VERTEX_H

#include "QskGlobal.h"

#include <qcolor.h>
#include <qsggeometry.h>
#include <qmath.h>

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

        Color interpolatedTo( Color, double ratio ) const noexcept;

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
}

namespace QskVertex
{
    class Quad
    {
      public:
        constexpr Quad() noexcept = default;

        inline constexpr Quad( const QRectF& rect ) noexcept
            : left( rect.left() )
            , top( rect.top() )
            , right( rect.right() )
            , bottom( rect.bottom() )
            , width( rect.width() )
            , height( rect.height() )
        {
        }

        inline constexpr bool operator==( const Quad& other ) const noexcept
        {
            return
                ( left == other.left ) &&
                ( right == other.right ) &&
                ( top == other.top ) &&
                ( bottom == other.bottom );
        }

        inline constexpr bool operator!=( const Quad& other ) const noexcept
        {
            return !( *this == other );
        }

        inline constexpr bool isEmpty() const noexcept
        {
            return ( width <= 0 ) || ( height <= 0 );
        }

        qreal left = 0.0;
        qreal top = 0.0;
        qreal right = 0.0;
        qreal bottom = 0.0;
        qreal width = 0.0;
        qreal height = 0.0;
    };
}

namespace
{
    class ArcIterator
    {
      public:
        inline ArcIterator() = default;

        inline ArcIterator( int stepCount, bool inverted = false )
        {
            reset( stepCount, inverted );
        }

        void reset( int stepCount, bool inverted )
        {
            m_inverted = inverted;

            if ( inverted )
            {
                m_cos = 1.0;
                m_sin = 0.0;
            }
            else
            {
                m_cos = 0.0;
                m_sin = 1.0;
            }

            m_stepIndex = 0;
            m_stepCount = stepCount;

            const double angleStep = M_PI_2 / stepCount;
            m_cosStep = qFastCos( angleStep );
            m_sinStep = qFastSin( angleStep );
        }

        inline bool isInverted() const { return m_inverted; }

        inline double cos() const { return m_cos; }
        inline double sin() const { return m_inverted ? -m_sin : m_sin; }

        inline int step() const { return m_stepIndex; }
        inline int stepCount() const { return m_stepCount; }
        inline bool isDone() const { return m_stepIndex > m_stepCount; }

        inline void increment()
        {
            const double cos0 = m_cos;

            m_cos = m_cos * m_cosStep + m_sin * m_sinStep;
            m_sin = m_sin * m_cosStep - cos0 * m_sinStep;

            ++m_stepIndex;
        }

        inline void operator++() { increment(); }

        static int segmentHint( double radius )
        {
            const double arcLength = radius * M_PI_2;
            return qBound( 3, qCeil( arcLength / 3.0 ), 18 ); // every 3 pixels
        }

      private:
        double m_cos;
        double m_sin;

        int m_stepIndex;
        double m_cosStep;
        double m_sinStep;

        int m_stepCount;
        bool m_inverted;
    };
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
