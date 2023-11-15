/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskStrokeNode.h"
#include "QskVertex.h"
#include "QskGradient.h"

#include <qpainterpath.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qtriangulatingstroker_p.h>
QSK_QT_PRIVATE_END

static inline bool qskIsPenVisible( const QPen& pen )
{
    if ( pen.style() == Qt::NoPen )
        return false;

    if ( pen.brush().gradient() )
    {
        // TODO ...
    }
    else
    {
        if ( pen.color().isValid() || ( pen.color().alpha() == 0 ) )
            return false;
    }

    return true;
}

QskStrokeNode::QskStrokeNode()
{
}

QskStrokeNode::~QskStrokeNode() = default;

void QskStrokeNode::updateNode( const QPainterPath& path, const QPen& pen )
{
    updateNode( path, QTransform(), pen );
}

void QskStrokeNode::updateNode(
    const QPainterPath& path, const QTransform& transform, const QPen& pen )
{
    if ( path.isEmpty() || !qskIsPenVisible( pen ) )
    {
        resetGeometry();
        return;
    }

    if ( auto qGradient = pen.brush().gradient() )
    {
        const auto r = transform.mapRect( path.boundingRect() );

        QskGradient gradient( *qGradient );
        gradient.setStretchMode( QskGradient::StretchToSize );

        setColoring( r, gradient );
    }
    else
        setColoring( pen.color() );

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
            dashStroker.process( qtVectorPathForPath( scaledPath ),
                effectivePen, clipRect, {} );

            const QVectorPath dashedVectorPath( dashStroker.points(),
                dashStroker.elementCount(), dashStroker.elementTypes(), 0 );

            stroker.process( dashedVectorPath, effectivePen, {}, {} );
        }

        auto& geometry = *this->geometry();

        // 2 vertices for each point
        geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );
        geometry.allocate( stroker.vertexCount() / 2 );

        if ( isGeometryColored() )
        {
            const QskVertex::Color c( pen.color() );

            const auto v = stroker.vertices();
            auto points = geometry.vertexDataAsColoredPoint2D();

            for ( int i = 0; i < geometry.vertexCount(); i++ )
            {
                const auto j = 2 * i;
                points[i].set( v[j], v[j + 1], c.r, c.g, c.b, c.a );
            }
        }
        else
        {
            memcpy( geometry.vertexData(), stroker.vertices(),
                stroker.vertexCount() * sizeof( float ) );
        }

        geometry.markVertexDataDirty();
        markDirty( QSGNode::DirtyGeometry );
    }
}
