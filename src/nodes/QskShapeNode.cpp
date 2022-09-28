/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskShapeNode.h"
#include "QskGradientMaterial.h"

#include <qbrush.h>
#include <qsgflatcolormaterial.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
#include <private/qvectorpath_p.h>
#include <private/qtriangulator_p.h>
QSK_QT_PRIVATE_END

static void qskUpdateGeometry( const QPainterPath& path, QSGGeometry& geometry )
{
    const auto ts = qTriangulate( path, QTransform(), 1, false );

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
    QSGMaterial* material = nullptr;
};

QskShapeNode::QskShapeNode()
    : QSGGeometryNode( *new QskShapeNodePrivate )
{
    Q_D( QskShapeNode );

    setGeometry( &d->geometry );
    setFlag( QSGNode::OwnsMaterial, true );
}

void QskShapeNode::updateNode( const QPainterPath& path, const QColor& color )
{
    if ( true ) // For the moment we always update the geometry. TODO ...
    {
        qskUpdateGeometry( path, *geometry() );
        markDirty( QSGNode::DirtyGeometry );
    }

    if ( true ) // For the moment we always update the material
    {
        auto material = new QSGFlatColorMaterial();
        material->setColor( color );

        setMaterial( material );
        markDirty( QSGNode::DirtyMaterial );
    }
}

void QskShapeNode::updateNode( const QPainterPath& path, const QGradient* gradient )
{
    if ( true ) // For the moment we always update the geometry. TODO ...
    {
        qskUpdateGeometry( path, *geometry() );
        markDirty( QSGNode::DirtyGeometry );
    }

    if ( true ) // For the moment we always update the material
    {
        setMaterial( QskGradientMaterial::create( gradient ) );
        markDirty( QSGNode::DirtyMaterial );
    }
}