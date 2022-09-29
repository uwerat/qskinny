/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskStrokeNode.h"
#include <qsgflatcolormaterial.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
#include <private/qtriangulatingstroker_p.h>
QSK_QT_PRIVATE_END

class QskStrokeNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    QskStrokeNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
    {
        geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );
    }

    QSGGeometry geometry;
    QSGFlatColorMaterial material;
};

QskStrokeNode::QskStrokeNode()
    : QSGGeometryNode( *new QskStrokeNodePrivate )
{
    Q_D( QskStrokeNode );

    setGeometry( &d->geometry );
    setMaterial( &d->material );
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

        QTriangulatingStroker stroker;
#if 0
        // can we do something useful with this factor ???
        stroker.setInvScale( 1.0 );
#endif

        if ( pen.style() == Qt::SolidLine )
        {
            // clipRect, renderHint are ignored in QTriangulatingStroker::process
            stroker.process( qtVectorPathForPath( scaledPath ), pen, {}, {} );
        }
        else
        {
            constexpr QRectF clipRect; // empty rect: no clipping

            QDashedStrokeProcessor dashStroker;
#if 0
            // can we do something useful with this factor ???
            dashStroker.setInvScale( 1.0 );
#endif
            dashStroker.process( qtVectorPathForPath( scaledPath ), pen, clipRect, {} );

            const QVectorPath dashedVectorPath( dashStroker.points(),
                dashStroker.elementCount(), dashStroker.elementTypes(), 0 );

            stroker.process( dashedVectorPath, pen, {}, {} );
        }

        d->geometry.allocate( stroker.vertexCount() );

        memcpy( d->geometry.vertexData(), stroker.vertices(),
            stroker.vertexCount() * sizeof( float ) );

        markDirty( QSGNode::DirtyGeometry );
    }

    const auto color = pen.color().toRgb();

    if ( d->material.color() != color )
    {
        d->material.setColor( color );
        markDirty( QSGNode::DirtyMaterial );
    }
}
