/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskShapeNode.h"
#include "QskGradientMaterial.h"
#include "QskGradient.h"
#include "QskGradientDirection.h"
#include "QskSGNode.h"

#include <qsgflatcolormaterial.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
#include <private/qvectorpath_p.h>
#include <private/qtriangulator_p.h>
QSK_QT_PRIVATE_END

static void qskUpdateGeometry( const QPainterPath& path,
    const QTransform& transform, QSGGeometry& geometry )
{
    const auto ts = qTriangulate( path, transform, 1, false );

#if 1
    geometry.allocate( ts.vertices.size(), ts.indices.size() );

    auto vertexData = reinterpret_cast< float* >( geometry.vertexData() );
    const auto points = ts.vertices.constData();

    for ( int i = 0; i < ts.vertices.count(); i++ )
        vertexData[i] = points[i];

    memcpy( geometry.indexData(), ts.indices.data(),
        ts.indices.size() * sizeof( quint16 ) );
#else
    /*
        As we have to iterate over the vertex buffer to copy qreal to float
        anyway we could reorder according to the index buffer and drop
        the index buffer then ???

        QTriangleSet:

        vertices: (x[i[n]], y[i[n]]), (x[j[n]], y[j[n]]), (x[k[n]], y[k[n]]), n = 0, 1, ...
        QVector<qreal> vertices; // [x[0], y[0], x[1], y[1], x[2], ...]
        QVector<quint16> indices; // [i[0], j[0], k[0], i[1], j[1], k[1], i[2], ...]
     */
    const auto points = ts.vertices.constData();
    const auto indices = reinterpret_cast< const quint16* >( ts.indices.data() );

    geometry.allocate( ts.indices.size() );

    auto vertexData = geometry.vertexDataAsPoint2D();
    for ( int i = 0; i < ts.indices.size(); i++ )
    {
        const int j = 2 * indices[i];
        vertexData[i].set( points[j], points[j + 1] );
    }
#endif
}

class QskShapeNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    QskShapeNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
    {
        geometry.setDrawingMode( QSGGeometry::DrawTriangles );
    }

    QSGGeometry geometry;
    int gradientType = -1;

    /*
        Is there a better way to find out if the path has changed
        beside storing a copy ( even, when internally with Copy On Write ) ?
     */
    QPainterPath path;
    QTransform transform;
};

QskShapeNode::QskShapeNode()
    : QSGGeometryNode( *new QskShapeNodePrivate )
{
    Q_D( QskShapeNode );

    setGeometry( &d->geometry );
    setMaterial( new QSGFlatColorMaterial() );
    setFlag( QSGNode::OwnsMaterial, true );
}

void QskShapeNode::updateNode( const QPainterPath& path,
    const QTransform& transform, const QRectF& rect, const QskGradient& gradient )
{
    Q_D( QskShapeNode );

    if ( path.isEmpty() || !gradient.isVisible() )
    {
        d->path = QPainterPath();
        d->transform = QTransform();
        QskSGNode::resetGeometry( this );

        return;
    }

    if ( ( transform != d->transform ) || ( path != d->path ) )
    {
        d->path = path;
        d->transform = transform;

        qskUpdateGeometry( path, transform, d->geometry );
        markDirty( QSGNode::DirtyGeometry );
    }

    if ( gradient.isMonochrome() )
    {
        if ( material() == nullptr || d->gradientType >= 0 )
        {
            setMaterial( new QSGFlatColorMaterial() );
            d->gradientType = -1;
        }

        const auto color = gradient.startColor().toRgb();

        auto mat = static_cast< QSGFlatColorMaterial* >( material() );
        if ( mat->color() != color )
        {
            mat->setColor( color );
            markDirty( QSGNode::DirtyMaterial );
        }
    }
    else
    {
        const auto effectiveGradient = gradient.effectiveGradient();
        const auto gradientType = effectiveGradient.type();

        if ( ( material() == nullptr ) || ( gradientType != d->gradientType ) )
        {
            setMaterial( QskGradientMaterial::createMaterial( gradientType ) );
            d->gradientType = gradientType;
        }

        auto mat = static_cast< QskGradientMaterial* >( material() );
        if ( mat->updateGradient( rect, effectiveGradient ) )
            markDirty( QSGNode::DirtyMaterial );
    }
}
