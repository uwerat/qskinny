/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskStrokeNode.h"
#include "QskVertex.h"

#include <qsgflatcolormaterial.h>
#include <qsgvertexcolormaterial.h>
#include <qpainterpath.h>
#include <qpolygon.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
#include <private/qtriangulatingstroker_p.h>
QSK_QT_PRIVATE_END

Q_GLOBAL_STATIC( QSGVertexColorMaterial, qskMaterialColorVertex )

static inline void qskMapPolygon( const QPolygonF& polygon,
    const QTransform& transform, const QColor& color, QSGGeometry& geometry )
{
    const QskVertex::Color c( color );

    auto p = geometry.vertexDataAsColoredPoint2D();

    if ( transform.isIdentity() )
    {
        for ( int i = 0; i < polygon.count(); i++ )
        {
            const auto& pos = polygon[i];
            p[i].set( pos.x(), pos.y(), c.r, c.g, c.b, c.a );
        }
    }
    else
    {
        for ( int i = 0; i < polygon.count(); i++ )
        {
            const auto pos = transform.map( polygon[i] );
            p[i].set( pos.x(), pos.y(), c.r, c.g, c.b, c.a );
        }
    }
}

static inline void qskMapPolygon( const QPolygonF& polygon,
    const QTransform& transform, QSGGeometry& geometry )
{
    auto p = geometry.vertexDataAsPoint2D();

    if ( transform.isIdentity() )
    {
        for ( int i = 0; i < polygon.count(); i++ )
        {
            const auto& pos = polygon[i];
            p[i].set( pos.x(), pos.y() );
        }
    }
    else
    {
        for ( int i = 0; i < polygon.count(); i++ )
        {
            const auto pos = transform.map( polygon[i] );
            p[i].set( pos.x(), pos.y() );
        }
    }
}

class QskStrokeNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    QskStrokeNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_ColoredPoint2D(), 0 )
    {
    }

    QSGGeometry geometry;
};

QskStrokeNode::QskStrokeNode()
    : QSGGeometryNode( *new QskStrokeNodePrivate )
{
    Q_D( QskStrokeNode );

    setGeometry( &d->geometry );
    setMaterial( qskMaterialColorVertex );
}

void QskStrokeNode::setRenderHint( RenderHint renderHint )
{
    Q_D( QskStrokeNode );

    const auto material = this->material();

    if ( renderHint == Colored )
    {
        if ( material != qskMaterialColorVertex )
        {
            setMaterial( qskMaterialColorVertex );
            delete material;

            const QSGGeometry g( QSGGeometry::defaultAttributes_ColoredPoint2D(), 0 );
            memcpy( ( void* ) &d->geometry, ( void* ) &g, sizeof( QSGGeometry ) );
        }
    }
    else
    {
        if ( material == qskMaterialColorVertex )
        {
            setMaterial( new QSGFlatColorMaterial() );

            const QSGGeometry g( QSGGeometry::defaultAttributes_Point2D(), 0 );
            memcpy( ( void* ) &d->geometry, ( void* ) &g, sizeof( QSGGeometry ) );
        }
    }
}

QskStrokeNode::RenderHint QskStrokeNode::renderHint() const
{
    return ( material() == qskMaterialColorVertex ) ? Colored : Flat;
}

void QskStrokeNode::updateNode(
    const QPainterPath& path, const QTransform& transform,  const QPen& pen )
{
    Q_D( QskStrokeNode );

    if ( path.isEmpty() || ( pen.style() == Qt::NoPen ) ||
         !pen.color().isValid() || ( pen.color().alpha() == 0 ) )
    {
        if ( d->geometry.vertexCount() > 0 )
        {
            d->geometry.allocate( 0 );
            markDirty( QSGNode::DirtyGeometry );
        }

        return;
    }

    if ( true ) // For the moment we always update the geometry. TODO ...
    {
        /*
            Unfortunately QTriangulatingStroker does not offer on the fly
            transformations - like with qTriangulate. TODO ...
         */
        const auto scaledPath = transform.map( path );

        auto effectivePen = pen;

        if ( !effectivePen.isCosmetic() )
        {
            const auto scaleFactor = qMin( transform.m11(), transform.m22() );
            if ( scaleFactor != 1.0 )
            {
                effectivePen.setWidth( effectivePen.widthF() * scaleFactor );
                effectivePen.setCosmetic( false );
            }
        }

        QTriangulatingStroker stroker;

        if ( pen.style() == Qt::SolidLine )
        {
            // clipRect, renderHint are ignored in QTriangulatingStroker::process
            stroker.process( qtVectorPathForPath( scaledPath ), effectivePen, {}, {} );
        }
        else
        {
            constexpr QRectF clipRect; // empty rect: no clipping

            QDashedStrokeProcessor dashStroker;
            dashStroker.process( qtVectorPathForPath( scaledPath ), effectivePen, clipRect, {} );

            const QVectorPath dashedVectorPath( dashStroker.points(),
                dashStroker.elementCount(), dashStroker.elementTypes(), 0 );

            stroker.process( dashedVectorPath, effectivePen, {}, {} );
        }

        // 2 vertices for each point
        d->geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );
        d->geometry.allocate( stroker.vertexCount() / 2 );

        if ( material() == qskMaterialColorVertex )
        {
            const QskVertex::Color c( pen.color() );

            const auto v = stroker.vertices();
            auto points = d->geometry.vertexDataAsColoredPoint2D();

            for ( int i = 0; i < d->geometry.vertexCount(); i++ )
            {
                const auto j = 2 * i;
                points[i].set( v[j], v[j + 1], c.r, c.g, c.b, c.a );
            }
        }
        else
        {
            memcpy( d->geometry.vertexData(), stroker.vertices(),
                stroker.vertexCount() * sizeof( float ) );
        }

        markDirty( QSGNode::DirtyGeometry );
    }

    if ( material() != qskMaterialColorVertex )
    {
        const auto color = pen.color().toRgb();

        auto flatMaterial = static_cast< QSGFlatColorMaterial* >( material() );
        if ( flatMaterial->color() != color )
        {
            flatMaterial->setColor( color );
            markDirty( QSGNode::DirtyMaterial );
        }
    }
}

/*
    For polygons with a small lineWidth ( < 2 ) or a line without
    connections we might get away with a simple and fast implementation
    using DrawLineStrip/DrawLineLoop
 */
void QskStrokeNode::updateNode( const QPolygonF& polygon,
    const QTransform& transform, qreal lineWidth, const QColor& color )
{
    Q_D( QskStrokeNode );

    if ( polygon.isEmpty() || !color.isValid() || ( color.alpha() == 0 ) )
    {
        if ( d->geometry.vertexCount() > 0 )
        {
            d->geometry.allocate( 0 );
            markDirty( QSGNode::DirtyGeometry );
        }

        return;
    }

    d->geometry.setDrawingMode( QSGGeometry::DrawLineStrip );

    const float lineWidthF = lineWidth;

    if( lineWidthF != d->geometry.lineWidth() )
        d->geometry.setLineWidth( lineWidthF );

    if ( true ) // TODO
    {
        d->geometry.allocate( polygon.count() );

        if ( material() == qskMaterialColorVertex )
        {
            qskMapPolygon( polygon, transform, color, d->geometry );
            markDirty( QSGNode::DirtyGeometry );
        }
        else
        {
            qskMapPolygon( polygon, transform, d->geometry );
            markDirty( QSGNode::DirtyGeometry );

            auto flatMaterial = static_cast< QSGFlatColorMaterial* >( material() );
            if ( flatMaterial->color() != color )
            {
                flatMaterial->setColor( color );
                markDirty( QSGNode::DirtyMaterial );
            }
        }
    }
}
