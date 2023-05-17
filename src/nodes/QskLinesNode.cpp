/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskLinesNode.h"
#include "QskVertex.h"
#include "QskStippleMetrics.h"
#include "QskSGNode.h"

#include <qsgflatcolormaterial.h>
#include <qsgvertexcolormaterial.h>
#include <qtransform.h>
#include <qquickitem.h>
#include <qquickwindow.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
#include <private/qstroker_p.h>
QSK_QT_PRIVATE_END

namespace
{
    inline qreal mapX( const QTransform& t, qreal x )
    {
        return t.dx() + t.m11() * x;
    }

    inline qreal mapY( const QTransform& t, qreal y )
    {
        return t.dy() + t.m22() * y;
    }

    /*
        Thanks to the hooks of the stroker classes we can make use
        of QDashStroker without having to deal with the overhead of
        QPainterPaths. But it might be worth to check if this could
        be done in a shader. TODO ...
     */
    class DashStroker : public QDashStroker
    {
      public:
        DashStroker( const QskStippleMetrics& metrics )
            : QDashStroker( nullptr )
        {
            setDashOffset( metrics.offset() );
            setDashPattern( metrics.pattern() );

            m_elements.reserve( 2 );
        }

        QSGGeometry::Point2D* addDashes( QSGGeometry::Point2D* points,
            qreal x1, qreal y1, qreal x2, qreal y2 )
        {
            setMoveToHook( addPoint );
            setLineToHook( addPoint );

            m_points = points;

            begin( this );

            m_elements.add( { QPainterPath::MoveToElement, x1, y1 } );
            m_elements.add( { QPainterPath::LineToElement, x2, y2 } );

            processCurrentSubpath();

            end();

            return m_points;
        }

        int pointCount( qreal x1, qreal y1, qreal x2, qreal y2 )
        {
            /*
                There should be a faster way to calculate the
                number of points. TODO ...
             */
            setMoveToHook( countPoint );
            setLineToHook( countPoint );

            m_count = 0;

            begin( this );

            m_elements.add( { QPainterPath::MoveToElement, x1, y1 } );
            m_elements.add( { QPainterPath::LineToElement, x2, y2 } );

            processCurrentSubpath();

            end();

            return m_count;
        }

      private:
        static void addPoint( qfixed x, qfixed y, void* data )
        {
            auto stroker = reinterpret_cast< DashStroker* >( data );
            ( stroker->m_points++ )->set( x, y );
        }

        static void countPoint( qfixed, qfixed, void* data )
        {
            auto stroker = reinterpret_cast< DashStroker* >( data );
            stroker->m_count++;
        }

        int m_count = 0;
        QSGGeometry::Point2D* m_points;
    };
}

class QskLinesNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    QskLinesNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
    {
        geometry.setDrawingMode( QSGGeometry::DrawLines );
        geometry.setVertexDataPattern( QSGGeometry::StaticPattern );
    }

    inline qreal round( bool isHorizontal, qreal v ) const
    {
        const auto r2 = 2.0 * devicePixelRatio;
        const qreal v0 = isHorizontal ? p0.x() : p0.y();

        const int d = qRound( r2 * ( v + v0 ) );
        const auto f = ( d % 2 ? d : d - 1 ) / r2;

        return f / devicePixelRatio - v0;
    }

    QSGGeometry geometry;
    QSGFlatColorMaterial material;

    // position of [0,0] in device coordinates
    QPointF p0;
    qreal devicePixelRatio = 1.0;

    QskHashValue hash = 0.0;
    bool dirty = true;
};

QskLinesNode::QskLinesNode()
    : QSGGeometryNode( *new QskLinesNodePrivate )
{
    Q_D( QskLinesNode );

    setGeometry( &d->geometry );
    setMaterial( &d->material );
}

QskLinesNode::~QskLinesNode()
{
}

void QskLinesNode::setGlobalPosition( const QQuickItem* item )
{
    QPointF p0;
    qreal devicePixelRatio = 1.0;

    if ( item )
    {
        p0 = item->mapToGlobal( QPointF() );

        if ( auto w = item->window() )
            devicePixelRatio = w->devicePixelRatio();
    }

    setGlobalPosition( p0, devicePixelRatio );
}

void QskLinesNode::setGlobalPosition(
    const QPointF& pos, qreal devicePixelRatio )
{
    Q_D( QskLinesNode );

    if ( pos != d->p0 || devicePixelRatio != d->devicePixelRatio )
    {
        d->p0 = pos;
        d->devicePixelRatio = devicePixelRatio;

        d->dirty = true;
    }
}

void QskLinesNode::updateLines( const QColor& color,
    qreal lineWidth, const QskStippleMetrics& stippleMetrics,
    const QTransform& transform, const QRectF& rect )
{
    // using QVarLengthArray instead. TODO ...
    updateLines( color, lineWidth, stippleMetrics, transform,
        rect, { rect.left(), rect.right() }, { rect.top(), rect.bottom() } );
}

void QskLinesNode::updateLines( const QColor& color,
    qreal lineWidth, const QskStippleMetrics& stippleMetrics,
    const QTransform& transform, const QRectF& rect,
    const QVector< qreal >& xValues, const QVector< qreal >& yValues )
{
    Q_D( QskLinesNode );

    if ( color != d->material.color() )
    {
        d->material.setColor( color );
        markDirty( QSGNode::DirtyMaterial );
    }

    QskHashValue hash = 9784;

    hash = stippleMetrics.hash( hash );
    hash = qHash( transform, hash );
    hash = qHashBits( &rect, sizeof( QRectF ), hash );
    hash = qHash( xValues, hash );
    hash = qHash( yValues, hash );

    if ( hash != d->hash )
    {
        d->dirty = true;
        d->hash = hash;
    }

    if( d->dirty )
    {
        if ( rect.isEmpty() || !stippleMetrics.isValid()
            || !color.isValid() || color.alpha() == 0 )
        {
            QskSGNode::resetGeometry( this );
        }
        else
        {
            updateGeometry( stippleMetrics,
                transform, rect, xValues, yValues );
        }

        markDirty( QSGNode::DirtyGeometry );
        d->dirty = false;
    }

    const float lineWidthF = lineWidth;
    if( lineWidthF != d->geometry.lineWidth() )
        d->geometry.setLineWidth( lineWidthF );
}

void QskLinesNode::updateGeometry(
    const QskStippleMetrics& stippleMetrics,
    const QTransform& transform, const QRectF& rect,
    const QVector< qreal >& xValues, const QVector< qreal >& yValues )
{
    Q_D( QskLinesNode );

    auto& geom = d->geometry;

    const auto y1 = mapY( transform, rect.top() );
    const auto y2 = mapY( transform, rect.bottom() );

    const auto x1 = mapX( transform, rect.left() );
    const auto x2 = mapX( transform, rect.right() );

    QSGGeometry::Point2D* points = nullptr;

    if ( stippleMetrics.isSolid() )
    {
        using namespace QskVertex;

        geom.allocate( 2 * ( xValues.count() + yValues.count() ) );
        points = geom.vertexDataAsPoint2D();

        points = setSolidLines( Qt::Vertical, y1, y2,
            transform, xValues.count(), xValues.constData(), points );

        points = setSolidLines( Qt::Horizontal, x1, x2,
            transform, yValues.count(), yValues.constData(), points );
    }
    else
    {
        DashStroker stroker( stippleMetrics );

        const int countX = stroker.pointCount( 0.0, y1, 0.0, y2 );
        const int countY = stroker.pointCount( x1, 0.0, x2, 0.0 );

        d->geometry.allocate( xValues.count() * countX + yValues.count() * countY );
        points = d->geometry.vertexDataAsPoint2D();

        points = setStippledLines( Qt::Vertical, y1, y2,
            transform, xValues.count(), xValues.constData(),
            stippleMetrics, points );

        points = setStippledLines( Qt::Horizontal, x1, x2,
            transform, yValues.count(), yValues.constData(),
            stippleMetrics, points );
    }

    Q_ASSERT( geom.vertexCount() == ( points - geom.vertexDataAsPoint2D() ) );
}

QSGGeometry::Point2D* QskLinesNode::setStippledLines(
    Qt::Orientation orientation, qreal v1, qreal v2,
    const QTransform& transform, int count, const qreal* values,
    const QskStippleMetrics& stippleMetrics, QSGGeometry::Point2D* points ) const
{
    Q_D( const QskLinesNode );

    if ( count <= 0 )
        return points;

    DashStroker stroker( stippleMetrics );

    // Calculating the dashes for the first line

    const auto line0 = points;
    int dashCount = 0;

    if ( orientation == Qt::Vertical )
    {
        auto x = mapX( transform, values[0] );
        x = d->round( true, x );

        points = stroker.addDashes( points, x, v1, x, v2 );
        dashCount = points - line0;
    }
    else
    {
        auto y = mapY( transform, values[0] );
        y = d->round( false, y );

        points = stroker.addDashes( points, v1, y, v2, y );
        dashCount = points - line0;
    }

    // all other dashes are translations of the dashes of the first line

    if ( orientation == Qt::Vertical )
    {
        for ( int i = 1; i < count; i++ )
        {
            auto x = mapX( transform, values[i] );
            x = d->round( true, x );

            for ( int j = 0; j < dashCount; j++ )
                points++->set( x, line0[j].y );
        }
    }
    else
    {
        for ( int i = 1; i < count; i++ )
        {
            auto y = mapY( transform, values[i] );
            y = d->round( false, y );

            for ( int j = 0; j < dashCount; j++ )
                points++->set( line0[j].x, y );
        }
    }

    return points;
}

QSGGeometry::Point2D* QskLinesNode::setSolidLines(
    Qt::Orientation orientation, qreal v1, qreal v2,
    const QTransform& transform, int count, const qreal* values,
    QSGGeometry::Point2D* points ) const
{
    Q_D( const QskLinesNode );

    if ( count <= 0 )
        return points;

    auto lines = reinterpret_cast< QskVertex::Line* >( points );

    if ( orientation == Qt::Vertical )
    {
        for ( int i = 0; i < count; i++ )
        {
            auto x = mapX( transform, values[i] );
            x = d->round( true, x );

            lines++->setVLine( x, v1, v2 );
        }
    }
    else
    {
        for ( int i = 0; i < count; i++ )
        {
            auto y = mapY( transform, values[i] );
            y = d->round( false, y );

            lines++->setHLine( v1, v2, y );
        }
    }

    return reinterpret_cast< QSGGeometry::Point2D* >( lines );
}
