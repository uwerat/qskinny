/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskLinesNode.h"
#include "QskVertex.h"
#include "QskStippleMetrics.h"
#include "QskStippledLineRenderer.h"
#include "QskSGNode.h"

#include <qsgflatcolormaterial.h>
#include <qsgvertexcolormaterial.h>
#include <qtransform.h>
#include <qquickitem.h>
#include <qquickwindow.h>
#include <qline.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
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

    class Renderer : public QskStippledLineRenderer
    {
      public:
        inline Renderer( const QskStippleMetrics& metrics )
            : QskStippledLineRenderer( metrics )
        {
        }

        inline QSGGeometry::Point2D* addDashes( QSGGeometry::Point2D* points,
            qreal x1, qreal y1, qreal x2, qreal y2 )
        {
            m_points = points;
            renderLine( x1, y1, x2, y2 );
            return m_points;
        }

      private:
        void renderDash( qreal x1, qreal y1, qreal x2, qreal y2 ) override
        {
            m_points++->set( x1, y1 );
            m_points++->set( x2, y2 );
        }

        QSGGeometry::Point2D* m_points;
    };
}

static QSGGeometry::Point2D* qskAddDashes( const QTransform& transform,
    int count, const QLineF* lines, const QskStippleMetrics& metrics,
    QSGGeometry::Point2D* points )
{
    if ( count <= 0 )
        return points;

    const bool doTransform = !transform.isIdentity();

    Renderer renderer( metrics );

    for ( int i = 0; i < count; i++ )
    {
        auto p1 = lines[i].p1();
        auto p2 = lines[i].p2();

        if ( doTransform )
        {
            p1 = transform.map( p1 );
            p2 = transform.map( p2 );
        }

        points = renderer.addDashes( points, p1.x(), p1.y(), p2.x(), p2.y() );
    }

    return points;
}

static QSGGeometry::Point2D* qskAddLines( const QTransform& transform,
    int count, const QLineF* lines, QSGGeometry::Point2D* points )
{
    if ( count <= 0 )
        return points;

    const bool doTransform = !transform.isIdentity();

    auto vlines = reinterpret_cast< QskVertex::Line* >( points );

    for ( int i = 0; i < count; i++ )
    {
        auto p1 = lines[i].p1();
        auto p2 = lines[i].p2();

        if ( doTransform )
        {
            p1 = transform.map( p1 );
            p2 = transform.map( p2 );
        }

        vlines++->setLine( p1.x(), p1.y(), p2.x(), p2.y() );
    }

    return reinterpret_cast< QSGGeometry::Point2D* >( vlines );
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
        if ( !doRound )
            return v;

        const auto r2 = 2.0 * devicePixelRatio;
        const qreal v0 = isHorizontal ? p0.x() : p0.y();

        const int d = qRound( r2 * ( v + v0 ) );
        const auto f = ( d % 2 ? d : d - 1 ) / r2;

        return f / devicePixelRatio - v0;
    }

    inline void setLineAttributes( QskLinesNode* node,
        const QColor& color, float lineWidth )
    {
        if ( color != material.color() )
        {
            material.setColor( color );
            node->markDirty( QSGNode::DirtyMaterial );
        }

        if( lineWidth != geometry.lineWidth() )
            geometry.setLineWidth( lineWidth );
    }

    QSGGeometry geometry;
    QSGFlatColorMaterial material;

    // position of [0,0] in device coordinates
    QPointF p0;
    qreal devicePixelRatio = 1.0;

    QskHashValue hash = 0.0;

    bool dirty = true;
    bool doRound = false;
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

    if ( d->doRound == false )
    {
        d->doRound = true;
        d->dirty = true;
    }

    if ( pos != d->p0 || devicePixelRatio != d->devicePixelRatio )
    {
        d->p0 = pos;
        d->devicePixelRatio = devicePixelRatio;

        d->dirty = true;
    }
}

void QskLinesNode::resetGlobalPosition()
{
    Q_D( QskLinesNode );

    if ( d->doRound == true )
    {
        d->doRound = false;
        d->dirty = true;
    }
}

void QskLinesNode::updateRect( const QColor& color,
    qreal lineWidth, const QskStippleMetrics& stippleMetrics,
    const QTransform& transform, const QRectF& rect )
{
    // using QVarLengthArray instead. TODO ...
    updateGrid( color, lineWidth, stippleMetrics, transform,
        rect, { rect.left(), rect.right() }, { rect.top(), rect.bottom() } );
}

void QskLinesNode::updateLine( const QColor& color,
    qreal lineWidth, const QPointF& p1, const QPointF& p2 )
{
    updateLine( color, lineWidth, QskStippleMetrics(), QTransform(), p1, p2 );
}

void QskLinesNode::updateLine( const QColor& color,
    qreal lineWidth, const QskStippleMetrics& stippleMetrics,
    const QTransform& transform, const QPointF& p1, const QPointF& p2 )
{
    if ( p1 == p2 )
    {
        updateLines( color, lineWidth, stippleMetrics, transform, 0, nullptr );
    }
    else
    {
        const QLineF line( p1, p2 );
        updateLines( color, lineWidth, stippleMetrics, transform, 1, &line );
    }
}

void QskLinesNode::updateLines( const QColor& color,
    qreal lineWidth, const QVector< QLineF >& lines )
{
    updateLines( color, lineWidth, QskStippleMetrics(),
        QTransform(), lines.count(), lines.constData() );
}

void QskLinesNode::updateLines( const QColor& color,
    qreal lineWidth, const QskStippleMetrics& stippleMetrics,
    const QTransform& transform, const QVector< QLineF >& lines )
{
    updateLines( color, lineWidth, stippleMetrics,
        transform, lines.count(), lines.constData() );
}

void QskLinesNode::updateLines( const QColor& color,
    qreal lineWidth, const QskStippleMetrics& stippleMetrics,
    const QTransform& transform, int count, const QLineF* lines )
{
    Q_D( QskLinesNode );

    if ( !stippleMetrics.isValid() || !color.isValid()
        || color.alpha() == 0 || count == 0 )
    {
        QskSGNode::resetGeometry( this );
        return;
    }

    QskHashValue hash = 9784;

    hash = stippleMetrics.hash( hash );
    hash = qHash( transform, hash );
    hash = qHashBits( lines, count * sizeof( QLineF ) );

    if ( hash != d->hash )
    {
        d->dirty = true;
        d->hash = hash;
    }

    if( d->dirty )
    {
        updateGeometry( stippleMetrics, transform, count, lines );

        markDirty( QSGNode::DirtyGeometry );
        d->dirty = false;
    }

    d->setLineAttributes( this, color, lineWidth );
}

void QskLinesNode::updateGrid( const QColor& color,
    qreal lineWidth, const QskStippleMetrics& stippleMetrics,
    const QTransform& transform, const QRectF& rect,
    const QVector< qreal >& xValues, const QVector< qreal >& yValues )
{
    Q_D( QskLinesNode );

    if ( !stippleMetrics.isValid() || !color.isValid() || color.alpha() == 0 )
    {
        QskSGNode::resetGeometry( this );
        return;
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
        updateGeometry( stippleMetrics, transform, rect, xValues, yValues );

        markDirty( QSGNode::DirtyGeometry );
        d->dirty = false;
    }

    d->setLineAttributes( this, color, lineWidth );
}

void QskLinesNode::updateGeometry( const QskStippleMetrics& stippleMetrics,
    const QTransform& transform, int count, const QLineF* lines )
{
    Q_D( QskLinesNode );

    auto& geom = d->geometry;

    QSGGeometry::Point2D* points = nullptr;

    if ( stippleMetrics.isSolid() )
    {
        using namespace QskVertex;

        geom.allocate( 2 * count );
        points = geom.vertexDataAsPoint2D();

        points = qskAddLines( transform, count, lines, points );
    }
    else
    {
        const bool doTransform = !transform.isIdentity();

        Renderer renderer( stippleMetrics );

        int lineCount = 0;
        for ( int i = 0; i < count; i++ )
        {
            auto p1 = lines[i].p1();
            auto p2 = lines[i].p2();

            if ( doTransform )
            {
                p1 = transform.map( p1 );
                p2 = transform.map( p2 );
            }
            lineCount += renderer.dashCount( p1, p2 );
        }

        d->geometry.allocate( 2 * lineCount );
        points = d->geometry.vertexDataAsPoint2D();

        points = qskAddDashes( transform,
            count, lines, stippleMetrics, points );

    }

    Q_ASSERT( geom.vertexCount() == ( points - geom.vertexDataAsPoint2D() ) );
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
        Renderer renderer( stippleMetrics );

        const auto countX = renderer.dashCount( 0.0, y1, 0.0, y2 );
        const auto countY = renderer.dashCount( x1, 0.0, x2, 0.0 );
        const auto count = xValues.count() * countX + yValues.count() * countY;

        d->geometry.allocate( 2 * count );
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

    Renderer renderer( stippleMetrics );

    // Calculating the dashes for the first line

    const auto line0 = points;
    int dashCount = 0;

    if ( orientation == Qt::Vertical )
    {
        auto x = mapX( transform, values[0] );
        x = d->round( true, x );

        points = renderer.addDashes( points, x, v1, x, v2 );
        dashCount = points - line0;
    }
    else
    {
        auto y = mapY( transform, values[0] );
        y = d->round( false, y );

        points = renderer.addDashes( points, v1, y, v2, y );
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
