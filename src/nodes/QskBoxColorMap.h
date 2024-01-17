/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_BOX_RENDERER_COLOR_MAP_H
#define QSK_BOX_RENDERER_COLOR_MAP_H

#include <QskGradient.h>
#include <QskGradientDirection.h>
#include <QskVertex.h>

namespace QskBoxRenderer
{
    class ColorMap
    {
      public:
        inline ColorMap()
            : ColorMap( QskGradient() )
        {
        }

        inline ColorMap( const QskGradient& gradient )
            : m_isTransparent( !gradient.isVisible() )
            , m_isMonochrome( gradient.isMonochrome() )
            , m_color1( gradient.rgbStart() )
            , m_color2( gradient.rgbEnd() )
        {
            if ( !m_isMonochrome )
            {
                const auto dir = gradient.linearDirection();

                m_x = dir.x1();
                m_y = dir.y1();
                m_dx = dir.x2() - dir.x1();
                m_dy = dir.y2() - dir.y1();
                m_dot = m_dx * m_dx + m_dy * m_dy;
            }
        }

        inline void setLine( qreal x1, qreal y1, qreal x2, qreal y2,
            QskVertex::ColoredLine* line ) const
        {
            if ( m_isMonochrome )
            {
                line->setLine( x1, y1, x2, y2, m_color1 );
            }
            else
            {
                const auto c1 = colorAt( x1, y1 );
                const auto c2 = colorAt( x2, y2 );

                line->setLine( x1, y1, c1, x2, y2, c2 );
            }
        }

        inline bool isMonochrome() const { return m_isMonochrome; }
        inline bool isTransparent() const { return m_isTransparent; }

        static inline bool isGradientSupported(
            const QskGradient& gradient, const QRectF& rect )
        {
            if ( gradient.isMonochrome() )
                return true;

            switch( gradient.stepCount() )
            {
                case 0:
                    return true;

                case 1:
                {
                    Q_ASSERT( gradient.stretchMode() != QskGradient::StretchToSize );
                    return gradient.linearDirection().contains( rect );
                }

                default:
                    return false;
            }
        }

      private:
        inline QskVertex::Color colorAt( qreal x, qreal y ) const
        {
            return m_color1.interpolatedTo( m_color2, valueAt( x, y ) );
        }

        inline qreal valueAt( qreal x, qreal y ) const
        {
            const qreal dx = x - m_x;
            const qreal dy = y - m_y;

            return ( dx * m_dx + dy * m_dy ) / m_dot;
        }

        const bool m_isTransparent;
        const bool m_isMonochrome;

        qreal m_x, m_y, m_dx, m_dy, m_dot;

        const QskVertex::Color m_color1;
        const QskVertex::Color m_color2;
    };

    class GradientIterator
    {
      public:
        GradientIterator() = default;

        inline GradientIterator( const QskVertex::Color color )
            : m_color1( color )
            , m_color2( color )
        {
        }

        inline GradientIterator(
                const QskVertex::Color& color1, const QskVertex::Color& color2 )
            : m_color1( color1 )
            , m_color2( color2 )
        {
        }

        inline GradientIterator( const QskGradientStops& stops )
            : m_stops( stops )
            , m_color1( stops.first().rgb() )
            , m_color2( m_color1 )
            , m_pos1( stops.first().position() )
            , m_pos2( m_pos1 )
            , m_index( 0 )
        {
        }

        inline void reset( const QskVertex::Color color )
        {
            m_index = -1;
            m_color1 = m_color2 = color;
        }

        inline void reset( const QskVertex::Color& color1,
            const QskVertex::Color& color2 )
        {
            m_index = -1;
            m_color1 = color1;
            m_color2 = color2;
        }

        inline void reset( const QskGradientStops& stops )
        {
            m_stops = stops;

            m_index = 0;
            m_color1 = m_color2 = stops.first().rgb();
            m_pos1 = m_pos2 = stops.first().position();
        }

        inline qreal position() const
        {
            return m_pos2;
        }

        inline QskVertex::Color color() const
        {
            return m_color2;
        }

        inline QskVertex::Color colorAt( qreal pos ) const
        {
            if ( m_color1 == m_color2 )
                return m_color1;

            if ( m_index < 0 )
            {
                return m_color1.interpolatedTo( m_color2, pos );
            }
            else
            {
                if ( m_pos2 == m_pos1 )
                    return m_color1;

                const auto r = ( pos - m_pos1 ) / ( m_pos2 - m_pos1 );
                return m_color1.interpolatedTo( m_color2, r );
            }
        }

        inline bool advance()
        {
            if ( m_index < 0 )
                return true;

            m_pos1 = m_pos2;
            m_color1 = m_color2;

            if ( ++m_index < m_stops.size() )
            {
                const auto& s = m_stops[ m_index ];

                m_pos2 = s.position();
                m_color2 = s.rgb();
            }

            return !isDone();
        }

        inline bool isDone() const
        {
            if ( m_index < 0 )
                return true;

            return m_index >= m_stops.size();
        }

      private:
        QskGradientStops m_stops;

        QskVertex::Color m_color1, m_color2;

        qreal m_pos1 = 0.0;
        qreal m_pos2 = 1.0;

        int m_index = -1;
    };
}

#endif
