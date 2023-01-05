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

    inline ColoredLine* fillUp( ColoredLine* lines, const ColoredLine& l, int count )
    {
        for ( int i = 0; i < count; i++ )
            *lines++ = l;

        return lines;
    }

    template< class ContourIterator, class ColorIterator >
    ColoredLine* fillOrdered( ContourIterator& contourIt,
        ColorIterator& colorIt, int lineCount, ColoredLine* lines )
    {
        /*
             When the the vector exceeds [ 0.0, 1.0 ] we might have
             gradient lines lying outside the contour.
             This effect could be precalculated - however we might end
             up difficult code with potential bugs.

             So we allow the allocation code to ignore the effect by
             adding duplicates of the last line.
         */
        const auto value0 = contourIt.value();
        ColoredLine* l = lines;

        do
        {
            while ( !colorIt.isDone() && ( colorIt.value() < contourIt.value() ) )
            {
                const auto value = colorIt.value();

                /*
                    When having a gradient vector below 0.0 we
                    will have gradient lines outside of the contour
                 */

                if ( value > value0 )
                {
                    if ( contourIt.setGradientLine( value, colorIt.color(), l ) )
                        l++;
                }

                colorIt.advance();
            }

            const auto color = colorIt.colorAt( contourIt.value() );
            contourIt.setContourLine( color, l++ );

        } while ( contourIt.advance() );

        if ( lineCount >= 0 )
        {
            if ( const auto count = lineCount - ( l - lines ) )
                l = QskVertex::fillUp( l, *( l - 1 ), count );
        }

        return l;
    }

    template< class ContourIterator >
    ColoredLine* fillOrdered( ContourIterator& contourIt,
        const QskGradient& gradient, int lineCount, ColoredLine* lines )
    {
        if ( gradient.stepCount() == 1 )
        {
            SimpleColorIterator colorIt( gradient.rgbStart(), gradient.rgbEnd() );
            lines = fillOrdered( contourIt, colorIt, lineCount, lines );
        }
        else
        {
            GradientColorIterator colorIt( gradient.stops() );
            lines = fillOrdered( contourIt, colorIt, lineCount, lines );
        }

        return lines;
    }
}

#endif
