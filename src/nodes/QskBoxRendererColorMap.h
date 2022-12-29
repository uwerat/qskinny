/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_RENDERER_COLOR_MAP_H
#define QSK_BOX_RENDERER_COLOR_MAP_H

#include <QskGradient.h>
#include <QskVertex.h>

#include <cassert>

class QskBoxShapeMetrics;

namespace QskVertex
{
    class ColorMapNone
    {
      public:
        constexpr inline Color colorAt( qreal ) const
        {
            return Color();
        }
    };

    class ColorMapSolid
    {
      public:
        constexpr inline ColorMapSolid() = default;

        inline ColorMapSolid( const QskGradient& gradient )
            : m_color( gradient.rgbStart() )
        {
        }

        inline Color colorAt( qreal ) const
        {
            return m_color;
        }

      private:
        const Color m_color;
    };

    class ColorMapGradient
    {
      public:
        inline ColorMapGradient( const QskGradient& gradient )
            : m_color1( gradient.rgbStart() )
            , m_color2( gradient.rgbEnd() )
        {
        }

        inline Color colorAt( qreal value ) const
        {
            return m_color1.interpolatedTo( m_color2, value );
        }

      private:
        const Color m_color1;
        const Color m_color2;
    };

    class ColorIterator
    {
      public:
        static inline bool advance()
        {
            return false;
        }

        inline qreal value() const
        {
            assert( false );
            return 0.0;
        }

        inline Color color() const
        {
            assert( false );
            return Color();
        }

        static inline bool isDone()
        {
            return true;
        }
    };

    class SimpleColorIterator : public ColorIterator
    {
      public:
        inline SimpleColorIterator( const QColor& color )
            : m_color1( color )
            , m_color2( color )
            , m_isMonochrome( true )
        {
        }

        inline SimpleColorIterator( const QColor& color1, const QColor& color2 )
            : m_color1( color1 )
            , m_color2( color2 )
            , m_isMonochrome( false )
        {
        }

        inline Color colorAt( qreal value ) const
        {
            /*
                When having only 1 color or 2 colors at 0.0/1.0 only we do not
                need gradient lines. So all what this iterator has to provide is
                an implementation of colorAt to colourize the contour lines.
             */

            if ( m_isMonochrome )
                return m_color1;

            return m_color1.interpolatedTo( m_color2, value );
        }

      private:
        const Color m_color1, m_color2;
        const bool m_isMonochrome;
    };

    class GradientColorIterator : public ColorIterator
    {
      public:
        inline GradientColorIterator( const QskGradientStops& stops )
            : m_stops( stops )
        {
            if ( stops.first().position() > 0.0 )
            {
                m_color1 = m_color2 = stops[ 0 ].rgb();
                m_index = 0;
            }
            else
            {
                m_color1 = stops[ 0 ].rgb();
                m_color2 = stops[ 1 ].rgb();
                m_index = 1;
            }

            m_finalIndex = m_stops.count() - 1;
            if ( stops.last().position() < 1.0 )
                m_finalIndex++;

            m_valueStep1 = 0.0;
            m_valueStep2 = stops[ m_index ].position();
            m_stepSize = m_valueStep2 - m_valueStep1;
        }

        inline qreal value() const
        {
            return m_valueStep2;
        }

        inline Color color() const
        {
            return m_color2;
        }

        inline Color colorAt( qreal value ) const
        {
            const qreal r = ( value - m_valueStep1 ) / m_stepSize;
            return m_color1.interpolatedTo( m_color2, r );
        }

        inline bool advance()
        {
            m_index++;

            m_color1 = m_color2;
            m_valueStep1 = m_valueStep2;

            if ( m_index >= m_stops.count() )
            {
                m_color2 = m_color1;
                m_valueStep2 = 1.0;
            }
            else
            {
                const auto& stop = m_stops[ m_index ];

                m_color2 = stop.rgb();
                m_valueStep2 = stop.position();
            }

            m_stepSize = m_valueStep2 - m_valueStep1;

            return !isDone();
        }

        inline bool isDone() const
        {
            return m_index >= m_finalIndex;
        }

      private:
        const QskGradientStops m_stops;

        int m_index, m_finalIndex;
        qreal m_valueStep1, m_valueStep2, m_stepSize;
        Color m_color1, m_color2;
    };

    template< class ContourIterator, class ColorIterator >
    ColoredLine* fillOrdered( ContourIterator& contourIt,
        ColorIterator& colorIt, ColoredLine* line )
    {
        do
        {
            while ( !colorIt.isDone() && ( colorIt.value() < contourIt.value() ) )
            {
                if ( contourIt.setGradientLine( colorIt.value(), colorIt.color(), line ) )
                    line++;

                colorIt.advance();
            }

            const auto color = colorIt.colorAt( contourIt.value() );
            contourIt.setContourLine( color, line++ );

        } while ( contourIt.advance() );

        return line;
    }

    template< class ContourIterator >
    ColoredLine* fillOrdered( ContourIterator& contourIt,
        const QskGradient& gradient, ColoredLine* line )
    {
        if ( gradient.stepCount() == 1 )
        {
            SimpleColorIterator colorIt( gradient.rgbStart(), gradient.rgbEnd() );
            line = fillOrdered( contourIt, colorIt, line );
        }
        else
        {
            GradientColorIterator colorIt( gradient.stops() );
            line = fillOrdered( contourIt, colorIt, line );
        }

        return line;
    }
}

#endif
