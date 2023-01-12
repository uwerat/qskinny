/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SCALE_RENDERER_H
#define QSK_SCALE_RENDERER_H

#include "QskVertex.h"
#include "QskRoundedRectRenderer.h"

class QskBoxShapeMetrics;
class QskBoxBorderMetrics;

namespace QskRoundedRect
{
    enum
    {
        TopLeft = Qt::TopLeftCorner,
        TopRight = Qt::TopRightCorner,
        BottomLeft = Qt::BottomLeftCorner,
        BottomRight = Qt::BottomRightCorner
    };

    int additionalGradientStops( const QskGradient& );
    int additionalGradientStops( const QskBoxBorderColors& );

    class Metrics
    {
      public:
        Metrics( const QRectF&, const QskBoxShapeMetrics&, const QskBoxBorderMetrics& );

        QskVertex::Quad outerQuad;
        QskVertex::Quad innerQuad;
        QskVertex::Quad centerQuad;

        struct Corner
        {
            bool isCropped;
            qreal centerX, centerY;
            qreal radiusX, radiusY;
            qreal radiusInnerX, radiusInnerY;

            int stepCount;

        } corner[ 4 ];

        bool isBorderRegular;
        bool isRadiusRegular;
        bool isTotallyCropped;
    };

    class BorderMapNone
    {
      public:
        static inline constexpr QskVertex::Color colorAt( int ) { return QskVertex::Color(); }
        inline QskGradient gradient() const { return QskGradient(); }
    };

    class BorderMapSolid
    {
      public:
        inline BorderMapSolid( const QskVertex::Color color ): m_color( color ) {}
        inline QskVertex::Color colorAt( int ) const { return m_color; }
        inline QskGradient gradient() const { return QskGradient(); }

        const QskVertex::Color m_color;
    };

    class BorderMapGradient
    {
      public:
        BorderMapGradient( int stepCount,
                const QskGradient& gradient1, const QskGradient& gradient2 )
            : m_stepCount( stepCount )
            , m_color1( gradient1.rgbStart() )
            , m_color2( gradient2.rgbEnd() )
            , m_gradient( gradient2 )
        {
        }

        inline QskVertex::Color colorAt( int step ) const
            { return m_color1.interpolatedTo( m_color2, step / m_stepCount ); }

        inline const QskGradient& gradient() const { return m_gradient; }

      private:
        const qreal m_stepCount;
        const QskVertex::Color m_color1, m_color2;
        const QskGradient m_gradient;
    };

    template< class T >
    class BorderMaps
    {
      public:
        BorderMaps( const T& );
        BorderMaps( const T&, const T&, const T&, const T& );

        int extraStops( int index ) const;
        inline int extraStops() const;

        T maps[4];
    };

    class BorderValues
    {
      public:
        BorderValues( const Metrics& );
        void setAngle( qreal cos, qreal sin );

        qreal dx1( int pos ) const;
        qreal dy1( int pos ) const;
        qreal dx2( int pos ) const;
        qreal dy2( int pos ) const;

      private:
        const Metrics& m_metrics;
        const bool m_isUniform;

        class Values
        {
          public:
            inline void setAngle( qreal cos, qreal sin )
            {
                dx = x0 + cos * rx;
                dy = y0 + sin * ry;
            }

            qreal dx, dy;
            qreal x0, y0, rx, ry;
        };

        union
        {
            struct
            {
                Values inner[ 4 ];
                Values outer[ 4 ];
            } m_multi;

            struct
            {
                qreal dx1, dy1, dx2, dy2;
            } m_uniform;
        };
    };

    template< class L >
    class Stroker
    {
      public:
        Stroker( const Metrics& );

        void createBorderLines( L* ) const;

        template< class FillMap >
        void createFillLines( Qt::Orientation, L*, FillMap& ) const;

        template< class BorderMap >
        void createBorderLines( Qt::Orientation, L*,
            const BorderMaps< BorderMap >& ) const;

        template< class BorderMap, class FillMap >
        inline void createUniformBox( Qt::Orientation, L*,
            const BorderMaps< BorderMap >&, L*, FillMap& ) const;

      private:
        void setBorderGradientLine( const QskVertex::Line&,
            float dx1, float dy1, float dx2, float dy2,
            const QskGradientStop&, L* ) const;

        void setBorderGradientLines( const BorderValues&,
            int corner, const QskGradient&, L* ) const;

        const Metrics& m_metrics;
    };
}

#include "QskRoundedRect.hpp"

#endif
