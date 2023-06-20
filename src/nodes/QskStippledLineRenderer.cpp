/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskStippledLineRenderer.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qstroker_p.h>
QSK_QT_PRIVATE_END

namespace
{
    /*
        Thanks to the hooks of the stroker classes we can make use
        of QDashStroker without having to deal with the overhead of
        QPainterPaths. But it might be worth to check if this could
        be done in a shader. TODO ...
     */
    class DashStroker : public QDashStroker
    {
      public:
        DashStroker( QskStippledLineRenderer* renderer )
            : QDashStroker( nullptr )
            , m_renderer( renderer )
        {
            setDashOffset( renderer->metrics().offset() );
            setDashPattern( renderer->metrics().pattern() );

            m_elements.reserve( 2 );
        }

        void renderDashes( qreal x1, qreal y1, qreal x2, qreal y2 )
        {
            if ( ( x1 == x2 ) && ( y1 == y2 ) )
                return;

            setMoveToHook( moveTo );
            setLineToHook( lineTo );

            begin( this );

            m_elements.add( { QPainterPath::MoveToElement, x1, y1 } );
            m_elements.add( { QPainterPath::LineToElement, x2, y2 } );

            processCurrentSubpath();

            end();
        }

        qsizetype dashCount( qreal x1, qreal y1, qreal x2, qreal y2 )
        {
            if ( ( x1 == x2 ) && ( y1 == y2 ) )
                return 0;

            /*
                There should be a faster way to calculate the
                number of points. TODO ...
             */
            setMoveToHook( countMoveTo );
            setLineToHook( countLineTo );

            m_count = 0;

            begin( this );

            m_elements.add( { QPainterPath::MoveToElement, x1, y1 } );
            m_elements.add( { QPainterPath::LineToElement, x2, y2 } );

            processCurrentSubpath();

            end();

            return m_count;
        }

      private:
        static void moveTo( qfixed x, qfixed y, void* data )
        {
            auto stroker = reinterpret_cast< DashStroker* >( data );

            stroker->m_x = x;
            stroker->m_y = y;
        }

        static void lineTo( qfixed x, qfixed y, void* data )
        {
            auto stroker = reinterpret_cast< DashStroker* >( data );
            stroker->m_renderer->renderDash( stroker->m_x, stroker->m_y, x, y );
        }

        static void countMoveTo( qfixed, qfixed, void* )
        {
        }

        static void countLineTo( qfixed, qfixed, void* data )
        {
            auto stroker = reinterpret_cast< DashStroker* >( data );
            stroker->m_count++;
        }

        QskStippledLineRenderer* m_renderer;

        qsizetype m_count = 0;
        qreal m_x, m_y;
    };
}

QskStippledLineRenderer::QskStippledLineRenderer( const QskStippleMetrics& metrics )
    : m_metrics( metrics )
{
}

QskStippledLineRenderer::~QskStippledLineRenderer()
{
}

qsizetype QskStippledLineRenderer::dashCount(
    const QPointF& p1, const QPointF& p2 ) const
{
    return dashCount( p1.x(), p1.y(), p2.x(), p2.y() );
}

qsizetype QskStippledLineRenderer::dashCount( const QLineF& line ) const
{
    return dashCount( line.x1(), line.y1(), line.x2(), line.y2() );
}

qsizetype QskStippledLineRenderer::dashCount(
    qreal x1, qreal y1, qreal x2, qreal y2 ) const
{
    auto that = const_cast< QskStippledLineRenderer* >( this );
    return DashStroker( that ).dashCount( x1, y1, x2, y2 );
}

void QskStippledLineRenderer::renderLine( const QPointF& p1, const QPointF& p2 )
{
    renderLine( p1.x(), p1.y(), p2.x(), p2.y() );
}

void QskStippledLineRenderer::renderLine( const QLineF& line )
{
    renderLine( line.x1(), line.y1(), line.x2(), line.y2() );
}

void QskStippledLineRenderer::renderLine( qreal x1, qreal y1, qreal x2, qreal y2 )
{
    DashStroker( this ).renderDashes( x1, y1, x2, y2 );
}

void QskStippledLineRenderer::renderDash( qreal, qreal, qreal, qreal )
{
    // nop
}
