/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_ROUNDED_RECT_H
#define QSK_ROUNDED_RECT_H

#include "QskVertex.h"
#include "QskRoundedRectRenderer.h"

class QskBoxShapeMetrics;
class QskBoxBorderMetrics;
class QskBoxBorderColors;

namespace QskRoundedRect
{
    enum
    {
        TopLeft = Qt::TopLeftCorner,
        TopRight = Qt::TopRightCorner,
        BottomLeft = Qt::BottomLeftCorner,
        BottomRight = Qt::BottomRightCorner
    };

    int extraBorderStops( const QskBoxBorderColors& );

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

    class BorderValues
    {
      public:
        BorderValues( const Metrics& );
        void setAngle( qreal cos, qreal sin );

        inline qreal dx1( int pos ) const
        {
            return m_isUniform ? m_uniform.dx1 : m_multi.inner[ pos].dx;
        }

        inline qreal dy1( int pos ) const
        {
            return m_isUniform ? m_uniform.dy1 : m_multi.inner[ pos ].dy;
        }

        inline qreal dx2( int pos ) const
        {
            if ( m_isUniform )
                return m_uniform.dx2;

            const auto outer = m_multi.outer;
            return m_metrics.isRadiusRegular ? outer[ 0 ].dx : outer[ pos ].dx;
        }

        inline qreal dy2( int pos ) const
        {
            if ( m_isUniform )
                return m_uniform.dy2;

            const auto outer = m_multi.outer;
            return m_metrics.isRadiusRegular ? outer[ 0 ].dy : outer[ pos ].dy;
        }

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

    class Stroker
    {
      public:
        inline Stroker( const Metrics& metrics )
            : m_metrics( metrics )
        {
        }

        void createBorderLines( QskVertex::Line* ) const;

        void createFillLines( QskVertex::ColoredLine*, const QskGradient& ) const;

        void createBorderLines( Qt::Orientation,
            QskVertex::ColoredLine*, const QskBoxBorderColors& ) const;

        void createUniformBox( QskVertex::ColoredLine*, const QskBoxBorderColors&,
            QskVertex::ColoredLine*, const QskGradient& ) const;

      private:
        void setBorderGradientLines( const BorderValues&,
            int corner, const QskGradient&, QskVertex::ColoredLine* ) const;

        const Metrics& m_metrics;
    };
}

#endif
