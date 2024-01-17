/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskBoxBasicStroker.h"
#include "QskBoxColorMap.h"

namespace
{
    inline int gradientLineCount( const QskGradient& borderGradient )
    {
        // only the intermediate gradient lines !
        return qMax( 0, borderGradient.stepCount() - 1 );
    }

    inline void setGradientLineAt(
        Qt::Orientation orientation, qreal x1, qreal y1, qreal x2, qreal y2,
        const QskGradientStop& stop, QskVertex::ColoredLine* line )
    {
        if ( orientation == Qt::Horizontal )
        {
            const auto pos = x1 + stop.position() * ( x2 - x1 );
            line->setLine( pos, y1, pos, y2, stop.rgb() );
        }
        else
        {
            const auto pos = y1 + stop.position() * ( y2 - y1 );
            line->setLine( x1, pos, x2, pos, stop.rgb() );
        }
    }

    inline int edgeToIndex( Qt::Edge edge )
        { return qCountTrailingZeroBits( static_cast< quint8 >( edge ) ); }

    class CornerIterator : public QskVertex::ArcIterator
    {
      public:
        inline CornerIterator( const QskBoxMetrics& metrics )
            : m_corners( metrics.corners )
        {
        }

        inline void resetSteps( int corner, bool inverted = false )
        {
            reset( m_corners[ corner ].stepCount, inverted );
        }

        inline void setBorderLine( int corner, QskVertex::Line* line ) const
        {
            const auto& c = m_corners[ corner ];

            line->setLine( c.xInner( cos() ), c.yInner( sin() ),
                c.xOuter( cos() ), c.yOuter( sin() ) );
        }

      protected:
        const QskBoxMetrics::Corner* m_corners;
    };

    class CornerIteratorColor : public CornerIterator
    {
      public:
        inline CornerIteratorColor( const QskBoxMetrics& metrics,
                const QskBoxBorderColors& colors )
            : CornerIterator( metrics )
            , m_colors{
                { colors.top().rgbStart(), colors.left().rgbEnd() },
                { colors.top().rgbEnd(), colors.right().rgbStart() },
                { colors.bottom().rgbEnd(), colors.left().rgbStart() },
                { colors.bottom().rgbStart(), colors.right().rgbEnd() } }
        {
        }

        inline void setBorderLine( int corner, QskVertex::ColoredLine* line ) const
        {
            const auto& c = m_corners[ corner ];

            line->setLine( c.xInner( cos() ), c.yInner( sin() ),
                c.xOuter( cos() ), c.yOuter( sin() ), color( corner ) );
        }

      private:
        inline QskVertex::Color color( int corner ) const
        {
            const auto& cs = m_colors[ corner ];

            if ( cs.first == cs.second )
                return cs.first;

            const auto ratio = step() / qreal( m_corners[ corner ].stepCount );
            return cs.first.interpolatedTo( cs.second, ratio );
        }

        const QPair< QskVertex::Color, QskVertex::Color > m_colors[4];
    };

    class LineMap
    {
      public:
        inline LineMap( const QskBoxMetrics& metrics )
            : m_corners( metrics.corners )
        {
        }

        inline void setHLine( int corner1, int corner2,
            qreal cos, qreal sin, QskVertex::Line* line ) const
        {
            const qreal y = m_corners[ corner1 ].yInner( sin );

            const qreal x1 = m_corners[ corner1 ].xInner( cos );
            const qreal x2 = m_corners[ corner2 ].xInner( cos );

            line->setLine( x1, y, x2, y );
        }

        inline void setVLine( int corner1, int corner2,
            qreal cos, qreal sin, QskVertex::Line* line ) const
        {
            const qreal x = m_corners[ corner1 ].xInner( cos );

            const qreal y1 = m_corners[ corner1 ].yInner( sin );
            const qreal y2 = m_corners[ corner2 ].yInner( sin );

            line->setLine( x, y1, x, y2 );
        }

        inline void setLine( int corner1, int corner2,
            qreal cos, qreal sin, QskVertex::Line* line ) const
        {
            const qreal x1 = m_corners[ corner1 ].xInner( cos );
            const qreal x2 = m_corners[ corner2 ].xInner( cos );

            const qreal y1 = m_corners[ corner1 ].yInner( sin );
            const qreal y2 = m_corners[ corner2 ].yInner( sin );

            line->setLine( x1, y1, x2, y2 );
        }

        const QskBoxMetrics::Corner* m_corners;
    };

    class FillMap
    {
      public:
        inline FillMap( const QskBoxMetrics& metrics,
                const QskBoxRenderer::ColorMap& colorMap )
            : m_colorMap( colorMap )
            , m_corners( metrics.corners )
        {
        }

        inline void setHLine( int corner1, int corner2,
            qreal cos, qreal sin, QskVertex::ColoredLine* line ) const
        {
            const qreal y = m_corners[ corner1 ].yInner( sin );

            const qreal x1 = m_corners[ corner1 ].xInner( cos );
            const qreal x2 = m_corners[ corner2 ].xInner( cos );

            m_colorMap.setLine( x1, y, x2, y, line );
        }

        inline void setVLine( int corner1, int corner2,
            qreal cos, qreal sin, QskVertex::ColoredLine* line ) const
        {
            const qreal x = m_corners[ corner1 ].xInner( cos );

            const qreal y1 = m_corners[ corner1 ].yInner( sin );
            const qreal y2 = m_corners[ corner2 ].yInner( sin );

            m_colorMap.setLine( x, y1, x, y2, line );
        }

        inline void setLine( int corner1, int corner2,
            qreal cos, qreal sin, QskVertex::ColoredLine* line ) const
        {
            const qreal x1 = m_corners[ corner1 ].xInner( cos );
            const qreal x2 = m_corners[ corner2 ].xInner( cos );

            const qreal y1 = m_corners[ corner1 ].yInner( sin );
            const qreal y2 = m_corners[ corner2 ].yInner( sin );

            m_colorMap.setLine( x1, y1, x2, y2, line );
        }

        const QskBoxRenderer::ColorMap& m_colorMap;
        const QskBoxMetrics::Corner* m_corners;
    };
}

static inline QskVertex::ColoredLine* qskAddGradientLines(
    const QLineF& l1, const QLineF& l2, const QskGradient& gradient,
    QskVertex::ColoredLine* lines )
{
    const auto stops = gradient.stops();

    if ( stops.first().position() > 0.0 )
        ( lines++ )->setLine( l1, stops.first().rgb() );

    for( const auto& stop : stops )
    {
        const auto p1 = l1.p1() + stop.position() * ( l2.p1() - l1.p1() );
        const auto p2 = l1.p2() + stop.position() * ( l2.p2() - l1.p2() );

        ( lines++ )->setLine( p1, p2, stop.rgb() );
    }

    if ( stops.last().position() < 1.0 )
        ( lines++ )->setLine( l2, stops.last().rgb() );

    return lines;
}

inline void qskSetRectBorderLines( const QRectF& in, const QRectF& out,
    const QskBoxBorderColors& colors, QskVertex::ColoredLine* lines )
{
    const QLineF cl[4] =
    {
        { in.right(), in.bottom(), out.right(), out.bottom() },
        { in.left(), in.bottom(), out.left(), out.bottom() },
        { in.left(), in.top(), out.left(), out.top() },
        { in.right(), in.top(), out.right(), out.top() }
    };

    if ( colors.isMonochrome() )
    {
        const QskVertex::Color c = colors.left().rgbStart();

        lines[0].setLine( cl[0], c );
        lines[1].setLine( cl[1], c );
        lines[2].setLine( cl[2], c );
        lines[3].setLine( cl[3], c );
        lines[4] = lines[ 0 ];
    }
    else
    {
        lines = qskAddGradientLines( cl[0], cl[1], colors.bottom(), lines );
        lines = qskAddGradientLines( cl[1], cl[2], colors.left(), lines );
        lines = qskAddGradientLines( cl[2], cl[3], colors.top(), lines );
        lines = qskAddGradientLines( cl[3], cl[0], colors.right(), lines );
    }
}

template< class Line, class FillMap >
static inline void qskCreateFill(
    const QskBoxMetrics& m_metrics, const FillMap& map, Line* lines )
{
    using namespace QskVertex;
    using namespace Qt;

    const auto cn = m_metrics.corners;
    const bool isHorizontal = m_metrics.preferredOrientation == Qt::Horizontal;

    if ( !m_metrics.isInsideRounded )
    {
        map.setHLine( TopLeftCorner, TopRightCorner, 0.0, 1.0, lines );
        map.setHLine( BottomLeftCorner, BottomRightCorner, 0.0, 1.0, lines + 1 );
    }
    else if ( m_metrics.isOutsideSymmetric )
    {
        const int stepCount = cn[ 0 ].stepCount;

        if ( isHorizontal )
        {
            Line* l1 = lines + stepCount;
            Line* l2 = lines + stepCount + 1;

            for ( ArcIterator it( stepCount ); !it.isDone(); ++it )
            {
                map.setVLine( TopLeftCorner, BottomLeftCorner, it.cos(), it.sin(), l1++ );
                map.setVLine( TopRightCorner, BottomRightCorner, it.cos(), it.sin(), l2-- );
            }
        }
        else
        {
            Line* l1 = lines;
            Line* l2 = lines + 2 * stepCount + 1;

            for ( ArcIterator it( stepCount ); !it.isDone(); ++it )
            {
                map.setHLine( TopLeftCorner, TopRightCorner, it.cos(), it.sin(), l1++ );
                map.setHLine( BottomLeftCorner, BottomRightCorner, it.cos(), it.sin(), l2-- );
            }
        }
    }
    else
    {
        auto line = lines;
        int stepCount;

        if ( isHorizontal )
        {
            stepCount = m_metrics.innerStepCount( TopLeftCorner, BottomLeftCorner );

            for ( ArcIterator it( stepCount, true ); !it.isDone(); ++it )
                map.setLine( TopLeftCorner, BottomLeftCorner, it.cos(), it.sin(), line++ );

            stepCount = m_metrics.innerStepCount( TopRightCorner, BottomRightCorner );

            for ( ArcIterator it( stepCount, false ); !it.isDone(); ++it )
                map.setLine( TopRightCorner, BottomRightCorner, it.cos(), it.sin(), line++ );
        }
        else
        {
            stepCount = m_metrics.innerStepCount( TopLeftCorner, TopRightCorner );

            for ( ArcIterator it( stepCount, false ); !it.isDone(); ++it )
                map.setLine( TopLeftCorner, TopRightCorner, it.cos(), it.sin(), line++ );

            stepCount = m_metrics.innerStepCount( BottomLeftCorner, BottomRightCorner );

            for ( ArcIterator it( stepCount, true ); !it.isDone(); ++it )
                map.setLine( BottomLeftCorner, BottomRightCorner, it.cos(), it.sin(), line++ );
        }
    }
}

QskBoxBasicStroker::GeometryLayout::GeometryLayout(
    const QskBoxMetrics& metrics, const QskBoxBorderColors& colors )
{
    const struct
    {
        Qt::Corner corner;
        Qt::Edge edge;
    } order[4] =
    {
        // counter clockwise
        { Qt::BottomRightCorner, Qt::RightEdge },
        { Qt::TopRightCorner, Qt::TopEdge },
        { Qt::TopLeftCorner, Qt::LeftEdge },
        { Qt::BottomLeftCorner, Qt::BottomEdge }
    };

    /*
        In case of horizontal filling the lines end at right edge,
        while for vertical filling it is the bottom edge.
     */
    const int index0 = ( metrics.preferredOrientation == Qt::Horizontal ) ? 1 : 0;

    int pos = index0;

    for ( int i = 0; i < 4; i++ )
    {
        const int idx = ( index0 + i ) % 4;

        const auto corner = order[ idx ].corner;
        const auto edge = order[ idx ].edge;

        this->cornerOffsets[ corner ] = pos;
        pos += metrics.corners[ corner ].stepCount + 1;

        this->edgeOffsets[ edgeToIndex( edge ) ] = pos;
        pos += gradientLineCount( colors.gradientAt( edge ) );
    }

    if ( index0 == 0 )
    {
        this->closingOffsets[ 0 ] = 0;
        this->closingOffsets[ 1 ] = pos;
        this->lineCount = pos + 1;
    }
    else
    {
        pos--;

        this->closingOffsets[ 0 ] = pos;
        this->closingOffsets[ 1 ] = 0;
        this->lineCount = pos + 1;
    }
}

QskBoxBasicStroker::QskBoxBasicStroker( const QskBoxMetrics& metrics )
    : m_metrics( metrics )
    , m_geometryLayout( metrics, QskBoxBorderColors() )
    , m_isColored( false )
{
}

QskBoxBasicStroker::QskBoxBasicStroker( const QskBoxMetrics& metrics,
        const QskBoxBorderColors& borderColors )
    : QskBoxBasicStroker( metrics, borderColors, QskBoxRenderer::ColorMap() )
{
}

QskBoxBasicStroker::QskBoxBasicStroker( const QskBoxMetrics& metrics,
        const QskBoxBorderColors& borderColors, const QskBoxRenderer::ColorMap& colorMap )
    : m_metrics( metrics )
    , m_borderColors( borderColors )
    , m_colorMap( colorMap )
    , m_geometryLayout( metrics, m_borderColors )
    , m_isColored( true )
{
}

void QskBoxBasicStroker::setBorderGradientLines(
    QskVertex::ColoredLine* lines ) const
{
    const auto off = m_geometryLayout.edgeOffsets;

    setBorderGradientLines( Qt::TopEdge, lines + off[0] );
    setBorderGradientLines( Qt::LeftEdge, lines + off[1] );
    setBorderGradientLines( Qt::RightEdge, lines + off[2] );
    setBorderGradientLines( Qt::BottomEdge, lines + off[3] );
}

void QskBoxBasicStroker::setBorderGradientLines(
    Qt::Edge edge, QskVertex::ColoredLine* lines ) const
{
    const auto& gradient = m_borderColors.gradientAt( edge );
    if( gradient.stepCount() <= 1 )
    {
        // everything done as contour lines
        return;
    }

    const auto cn = m_metrics.corners;

    qreal x1, x2, y1, y2;
    Qt::Orientation orientation;

    switch( edge )
    {
        case Qt::LeftEdge:
        {
            orientation = Qt::Vertical;

            x1 = m_metrics.innerRect.left();
            x2 = m_metrics.outerRect.left();
            y1 = cn[ Qt::BottomLeftCorner ].yInner( 0.0 );
            y2 = cn[ Qt::TopLeftCorner ].yInner( 0.0 );

            break;
        }
        case Qt::TopEdge:
        {
            orientation = Qt::Horizontal;

            x1 = cn[ Qt::TopLeftCorner ].xInner( 0.0 );
            x2 = cn[ Qt::TopRightCorner ].xInner( 0.0 );
            y1 = m_metrics.innerRect.top();
            y2 = m_metrics.outerRect.top();

            break;
        }
        case Qt::BottomEdge:
        {
            orientation = Qt::Horizontal;

            x1 = cn[ Qt::BottomRightCorner ].xInner( 0.0 );
            x2 = cn[ Qt::BottomLeftCorner ].xInner( 0.0 );
            y1 = m_metrics.innerRect.bottom();
            y2 = m_metrics.outerRect.bottom();

            break;
        }
        case Qt::RightEdge:
        default:
        {
            orientation = Qt::Vertical;

            x1 = m_metrics.innerRect.right();
            x2 = m_metrics.outerRect.right();
            y1 = cn[ Qt::TopRightCorner ].yInner( 0.0 );
            y2 = cn[ Qt::BottomRightCorner ].yInner( 0.0 );

            break;
        }
    }

    auto line = lines;
    const auto& stops = gradient.stops();

    if ( stops.last().position() < 1.0 )
        setGradientLineAt( orientation, x1, y1, x2, y2, stops.last(), line++ );

    for( int i = stops.count() - 2; i >= 1; i-- )
        setGradientLineAt( orientation, x1, y1, x2, y2, stops[i], line++ );

    if ( stops.first().position() > 0.0 )
        setGradientLineAt( orientation, x1, y1, x2, y2, stops.first(), line++ );
}

void QskBoxBasicStroker::setBorderLines( QskVertex::Line* lines ) const
{
    Q_ASSERT( !m_isColored );

    if ( !m_metrics.isOutsideRounded )
    {
        const auto& out = m_metrics.outerRect;
        const auto& in = m_metrics.innerRect;

        lines[0].setLine( in.right(), in.bottom(), out.right(), out.bottom() );
        lines[1].setLine( in.left(), in.bottom(), out.left(), out.bottom() );
        lines[2].setLine( in.left(), in.top(), out.left(), out.top() );
        lines[3].setLine( in.right(), in.top(), out.right(), out.top() );
        lines[4] = lines[ 0 ];

        return;
    }

    const auto& gl = m_geometryLayout;
    const auto cn = m_metrics.corners;

    auto linesTL = lines + gl.cornerOffsets[ Qt::TopLeftCorner ];

    auto linesTR = lines + gl.cornerOffsets[ Qt::TopRightCorner ]
        + cn[ Qt::TopRightCorner ].stepCount;

    auto linesBL = lines + gl.cornerOffsets[ Qt::BottomLeftCorner ]
        + cn[ Qt::BottomLeftCorner ].stepCount;

    auto linesBR = lines + gl.cornerOffsets[ Qt::BottomRightCorner ];

    CornerIterator it( m_metrics );

    if ( m_metrics.isOutsideSymmetric && m_metrics.isInsideRounded )
    {
        for ( it.resetSteps( Qt::TopLeftCorner ); !it.isDone(); ++it )
        {
            it.setBorderLine( Qt::TopLeftCorner, linesTL++ );
            it.setBorderLine( Qt::TopRightCorner, linesTR-- );
            it.setBorderLine( Qt::BottomLeftCorner, linesBL-- );
            it.setBorderLine( Qt::BottomRightCorner, linesBR++ );
        }
    }
    else
    {
        for ( it.resetSteps( Qt::TopLeftCorner ); !it.isDone(); ++it )
            it.setBorderLine( Qt::TopLeftCorner, linesTL++ );

        for ( it.resetSteps( Qt::TopRightCorner ); !it.isDone(); ++it )
            it.setBorderLine( Qt::TopRightCorner, linesTR-- );

        for ( it.resetSteps( Qt::BottomLeftCorner ); !it.isDone(); ++it )
            it.setBorderLine( Qt::BottomLeftCorner, linesBL-- );

        for ( it.resetSteps( Qt::BottomRightCorner ); !it.isDone(); ++it )
            it.setBorderLine( Qt::BottomRightCorner, linesBR++);
    }

    lines[ gl.closingOffsets[ 1 ] ] = lines[ gl.closingOffsets[ 0 ] ];
}

void QskBoxBasicStroker::setBorderLines( QskVertex::ColoredLine* lines ) const
{
    Q_ASSERT( m_isColored );
    Q_ASSERT( lines );

    if ( !m_metrics.isOutsideRounded )
    {
        qskSetRectBorderLines( m_metrics.innerRect,
            m_metrics.outerRect, m_borderColors, lines );
        return;
    }

    const auto& gl = m_geometryLayout;
    const auto cn = m_metrics.corners;

    auto linesTL = lines + gl.cornerOffsets[ Qt::TopLeftCorner ];

    auto linesTR = lines + gl.cornerOffsets[ Qt::TopRightCorner ]
        + cn[ Qt::TopRightCorner ].stepCount;

    auto linesBL = lines + gl.cornerOffsets[ Qt::BottomLeftCorner ]
        + cn[ Qt::BottomLeftCorner ].stepCount;

    auto linesBR = lines + gl.cornerOffsets[ Qt::BottomRightCorner ];

    CornerIteratorColor it( m_metrics, m_borderColors );

    if ( m_metrics.isOutsideSymmetric && m_metrics.isInsideRounded )
    {
        for ( it.resetSteps( Qt::TopLeftCorner ); !it.isDone(); ++it )
        {
            it.setBorderLine( Qt::TopLeftCorner, linesTL++ );
            it.setBorderLine( Qt::TopRightCorner, linesTR-- );
            it.setBorderLine( Qt::BottomLeftCorner, linesBL-- );
            it.setBorderLine( Qt::BottomRightCorner, linesBR++ );
        }
    }
    else
    {
        for ( it.resetSteps( Qt::TopLeftCorner ); !it.isDone(); ++it )
            it.setBorderLine( Qt::TopLeftCorner, linesTL++ );

        for ( it.resetSteps( Qt::TopRightCorner ); !it.isDone(); ++it )
            it.setBorderLine( Qt::TopRightCorner, linesTR-- );

        for ( it.resetSteps( Qt::BottomLeftCorner ); !it.isDone(); ++it )
            it.setBorderLine( Qt::BottomLeftCorner, linesBL-- );

        for ( it.resetSteps( Qt::BottomRightCorner ); !it.isDone(); ++it )
            it.setBorderLine( Qt::BottomRightCorner, linesBR++ );
    }

    setBorderGradientLines( lines );
    lines[ gl.closingOffsets[ 1 ] ] = lines[ gl.closingOffsets[ 0 ] ];
}

void QskBoxBasicStroker::setFillLines( QskVertex::Line* lines ) const
{
    Q_ASSERT( !m_isColored );
    Q_ASSERT( lines );

    if ( m_metrics.isInsideRounded )
    {
        const LineMap map( m_metrics );
        qskCreateFill( m_metrics, map, lines );
    }
    else // a rectangle
    {
        const auto& in = m_metrics.innerRect;

        lines[0].setLine( in.left(), in.top(), in.right(), in.top() );
        lines[1].setLine( in.left(), in.bottom(), in.right(), in.bottom() );
    }
}

void QskBoxBasicStroker::setFillLines( QskVertex::ColoredLine* lines ) const
{
    Q_ASSERT( m_isColored );
    Q_ASSERT( lines );
    Q_ASSERT( !m_colorMap.isTransparent() );

    if ( m_metrics.isInsideRounded )
    {
        const FillMap map( m_metrics, m_colorMap );
        qskCreateFill( m_metrics, map, lines );
    }
    else // a rectangle
    {
        const auto& in = m_metrics.innerRect;

        m_colorMap.setLine( in.left(), in.top(), in.right(), in.top(), lines + 0 );
        m_colorMap.setLine( in.left(), in.bottom(), in.right(), in.bottom(), lines + 1 );
    }
}

void QskBoxBasicStroker::setBoxLines( QskVertex::ColoredLine* borderLines,
    QskVertex::ColoredLine* fillLines ) const
{
    Q_ASSERT( m_isColored );
    Q_ASSERT( borderLines || fillLines );
    Q_ASSERT( fillLines == nullptr || !m_colorMap.isTransparent() );

    if ( m_metrics.isOutsideSymmetric && m_metrics.isInsideRounded )
    {
        if ( borderLines && fillLines )
        {
            /*
                Doing all in one allows a slightly faster implementation.
                As this is the by far most common situation we do this
                micro optimization.
             */
            setBorderAndFillLines( borderLines, fillLines );
            return;
        }
    }

    if ( borderLines )
        setBorderLines( borderLines );

    if ( fillLines )
        setFillLines( fillLines );
}

void QskBoxBasicStroker::setBorderAndFillLines(
    QskVertex::ColoredLine* borderLines, QskVertex::ColoredLine* fillLines ) const
{
    using namespace Qt;

    const auto& gl = m_geometryLayout;

    const FillMap fillMap( m_metrics, m_colorMap );
    CornerIteratorColor it( m_metrics, m_borderColors );

    /*
        It would be possible to run over [0, 0.5 * M_PI_2]
        and create 8 values ( instead of 4 ) in each step. TODO ...
     */

    const auto stepCount = m_metrics.corners[0].stepCount;

    auto linesTL = borderLines + gl.cornerOffsets[ TopLeftCorner ];
    auto linesTR = borderLines + gl.cornerOffsets[ TopRightCorner ] + stepCount;
    auto linesBL = borderLines + gl.cornerOffsets[ BottomLeftCorner ] + stepCount;
    auto linesBR = borderLines + gl.cornerOffsets[ BottomRightCorner ];

    if ( m_metrics.preferredOrientation == Qt::Horizontal )
    {
        auto l1 = fillLines + stepCount;
        auto l2 = fillLines + stepCount + 1;

        for ( it.resetSteps( TopLeftCorner ); !it.isDone(); ++it )
        {
            it.setBorderLine( TopLeftCorner, linesTL++ );
            it.setBorderLine( TopRightCorner, linesTR-- );
            it.setBorderLine( BottomLeftCorner, linesBL-- );
            it.setBorderLine( BottomRightCorner, linesBR++ );

            fillMap.setVLine( TopLeftCorner, BottomLeftCorner, it.cos(), it.sin(), l1-- );
            fillMap.setVLine( TopRightCorner, BottomRightCorner, it.cos(), it.sin(), l2++ );
        }
    }
    else
    {
        auto l1 = fillLines;
        auto l2 = fillLines + 2 * stepCount + 1;

        for ( it.resetSteps( TopLeftCorner ); !it.isDone(); ++it )
        {
            it.setBorderLine( TopLeftCorner, linesTL++ );
            it.setBorderLine( TopRightCorner, linesTR-- );
            it.setBorderLine( BottomLeftCorner, linesBL-- );
            it.setBorderLine( BottomRightCorner, linesBR++ );

            fillMap.setHLine( TopLeftCorner, TopRightCorner, it.cos(), it.sin(), l1++ );
            fillMap.setHLine( BottomLeftCorner, BottomRightCorner, it.cos(), it.sin(), l2-- );
        }
    }

    if ( borderLines )
    {
        setBorderGradientLines( borderLines );
        borderLines[ gl.closingOffsets[ 1 ] ] = borderLines[ gl.closingOffsets[ 0 ] ];
    }
}

int QskBoxBasicStroker::borderCount() const
{
    if ( !m_metrics.hasBorder )
        return 0;

    if ( m_isColored && !m_borderColors.isVisible() )
        return 0;

    int n = 0;

    if ( m_metrics.isOutsideRounded )
    {
        /*
            4: Number of lines is always one more than the number of steps.
            1: extra line at the end to close the border path
         */
        n = m_metrics.outerStepCount() + 4 + 1;

        if ( m_isColored && !m_borderColors.isMonochrome() )
        {
            n += gradientLineCount( m_borderColors.left() );
            n += gradientLineCount( m_borderColors.top() );
            n += gradientLineCount( m_borderColors.right() );
            n += gradientLineCount( m_borderColors.bottom() );
        }
    }
    else
    {
        /*
            4: One for each corner
            1: extra line at the end to close the border path
         */
        n = 4 + 1;

        if ( m_isColored && !m_borderColors.isMonochrome() )
        {
            const int gradientLines = -1
                + m_borderColors.left().stepCount()
                + m_borderColors.top().stepCount()
                + m_borderColors.right().stepCount()
                + m_borderColors.bottom().stepCount();

            n += qMax( gradientLines, 0 );
        }
    }

    return n;
}

int QskBoxBasicStroker::fillCount() const
{
    if ( m_metrics.innerRect.isEmpty() )
        return 0;

    if ( m_isColored && m_colorMap.isTransparent() )
        return 0;

    int n = 2;

    if ( m_metrics.isInsideRounded )
    {
        if ( m_metrics.preferredOrientation == Qt::Horizontal )
        {
            n += m_metrics.innerStepCount( Qt::TopLeftCorner, Qt::BottomLeftCorner );
            n += m_metrics.innerStepCount( Qt::TopRightCorner, Qt::BottomRightCorner );
        }
        else
        {
            n += m_metrics.innerStepCount( Qt::TopLeftCorner, Qt::TopRightCorner );
            n += m_metrics.innerStepCount( Qt::BottomLeftCorner, Qt::BottomRightCorner );
        }
    }

    return n;
}
