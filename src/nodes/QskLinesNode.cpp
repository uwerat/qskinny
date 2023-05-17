/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskLinesNode.h"
#include "QskIntervalF.h"
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

void QskLinesNode::updateGrid( const QColor& color, qreal lineWidth,
    const QskStippleMetrics& stippleMetrics, const QTransform& transform,
    const QskIntervalF& xBoundaries, const QVector< qreal >& xValues,
    const QskIntervalF& yBoundaries, const QVector< qreal >& yValues )
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
    hash = qHashBits( &xBoundaries, sizeof( xBoundaries ), hash );
    hash = qHashBits( &yBoundaries, sizeof( yBoundaries ), hash );
    hash = qHash( xValues, hash );
    hash = qHash( yValues, hash );

    if ( hash != d->hash )
    {
        d->dirty = true;
        d->hash = hash;
    }

    if( d->dirty )
    {
        if ( !( stippleMetrics.isValid()
            && color.isValid() && color.alpha() > 0 ) )
        {
            QskSGNode::resetGeometry( this );
        }
        else
        {
            updateGeometry( stippleMetrics, transform,
                xBoundaries, xValues, yBoundaries, yValues );
        }

        markDirty( QSGNode::DirtyGeometry );
        d->dirty = false;
    }

    const float lineWidthF = lineWidth;
    if( lineWidthF != d->geometry.lineWidth() )
        d->geometry.setLineWidth( lineWidthF );
}

void QskLinesNode::updateGeometry(
    const QskStippleMetrics& stippleMetrics, const QTransform& transform,
    const QskIntervalF& xBoundaries, const QVector< qreal >& xValues,
    const QskIntervalF& yBoundaries, const QVector< qreal >& yValues )
{
    Q_D( QskLinesNode );

    const auto x1 = mapX( transform, xBoundaries.lowerBound() );
    const auto x2 = mapX( transform, xBoundaries.upperBound() );

    const auto y1 = mapY( transform, yBoundaries.lowerBound() );
    const auto y2 = mapY( transform, yBoundaries.upperBound() );

    if ( stippleMetrics.isSolid() )
    {
        using namespace QskVertex;

        auto lines = allocateLines< Line >(
            d->geometry, xValues.count() + yValues.count() );

        for ( auto x : xValues )
        {
            x = mapX( transform, x );
            x = d->round( true, x );

            lines++->setVLine( x, y1, y2 );
        }

        for ( auto y : yValues )
        {
            y = mapY( transform, y );
            y = d->round( false, y );

            lines++->setHLine( x1, x2, y );
        }
    }
    else
    {
        DashStroker stroker( stippleMetrics );

        const int countX = stroker.pointCount( 0.0, y1, 0.0, y2 );
        const int countY = stroker.pointCount( x1, 0.0, x2, 0.0 );

        auto count = xValues.count() * countX + yValues.count() * countY;

        d->geometry.allocate( count );
        auto points = d->geometry.vertexDataAsPoint2D();

        /*
            We have to calculate the first line only. All others are
            translation without changing the positions of the dashes.
         */
        auto p0 = points;

        for ( int i = 0; i < xValues.count(); i++ )
        {
            auto x = mapX( transform, xValues[i] );
            x = d->round( true, x );

            if ( i == 0 )
            {
                points = stroker.addDashes( points, x, y1, x, y2 );
            }
            else
            {
                for ( int j = 0; j < countX; j++ )
                    points++->set( x, p0[j].y );
            }
        }

        p0 = points;

        for ( int i = 0; i < yValues.count(); i++ )
        {
            auto y = mapY( transform, yValues[i] );
            y = d->round( false, y );

            if ( i == 0 )
            {
                points = stroker.addDashes( points, x1, y, x2, y );
            }
            else
            {
                for ( int j = 0; j < countY; j++ )
                    points++->set( p0[j].x, y );
            }
        }
    }
}
