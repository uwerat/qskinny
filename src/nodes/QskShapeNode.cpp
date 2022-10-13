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

static inline void qskResetGeometry( QskShapeNode* node )
{
    auto g = node->geometry();
    if ( g->vertexCount() > 0 )
    {
        g->allocate( 0 );
        node->markDirty( QSGNode::DirtyGeometry );
    }
}

static inline bool qskIsGradientVisible( const QGradient* gradient )
{
    if ( gradient && gradient->type() != QGradient::NoGradient )
    {
        for ( const auto& stop : gradient->stops() )
        {
            if ( stop.second.alpha() > 0 )
                return true;
        }
    }

    return false;
}

static inline bool qskIsGradientMonochrome( const QGradient* gradient )
{
    if ( gradient && gradient->type() != QGradient::NoGradient )
    {
        const auto stops = gradient->stops();

        for ( int i = 1; i < stops.count(); i++ )
        {
            if ( stops[i].second != stops[i - 1].second )
                return false;
        }
    }

    return true;
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
    QGradient::Type gradientType = QGradient::NoGradient;

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
    const QTransform& transform, const QColor& color )
{
    Q_D( QskShapeNode );

    if ( path.isEmpty() || !color.isValid() || color.alpha() == 0 )
    {
        d->path = QPainterPath();
        qskResetGeometry( this );
        
        return;
    }

    if ( ( transform != d->transform ) || ( path != d->path ) )
    {
        d->path = path;
        d->transform = transform;

        qskUpdateGeometry( path, transform, d->geometry );
        markDirty( QSGNode::DirtyGeometry );
    }

    if ( material() == nullptr || d->gradientType != QGradient::NoGradient )
    {
        setMaterial( new QSGFlatColorMaterial() );
        d->gradientType = QGradient::NoGradient;
    }

    const auto c = color.toRgb();

    auto colorMaterial = static_cast< QSGFlatColorMaterial* >( material() );
    if ( colorMaterial->color() != c )
    {
        colorMaterial->setColor( c );
        markDirty( QSGNode::DirtyMaterial );
    }
}

void QskShapeNode::updateNode( const QPainterPath& path,
    const QTransform& transform, const QRectF& rect,
    const QGradient* gradient, qreal extraValue )
{
    Q_D( QskShapeNode );

    if ( path.isEmpty() || !qskIsGradientVisible( gradient ) )
    {
        d->path = QPainterPath();
        qskResetGeometry( this );

        return;
    }

    if ( qskIsGradientMonochrome( gradient ) )
    {
        updateNode( path, transform, gradient->stops().first().first );
        return;
    }

    if ( ( transform != d->transform ) || ( path != d->path ) )
    {
        d->path = path;
        d->transform = transform;

        qskUpdateGeometry( path, transform, d->geometry );
        markDirty( QSGNode::DirtyGeometry );
    }

    if ( ( material() == nullptr ) || ( gradient->type() != d->gradientType ) )
    {
        setMaterial( QskGradientMaterial::createMaterial( gradient->type() ) );
        d->gradientType = gradient->type();
    }

    auto gradientMaterial = static_cast< QskGradientMaterial* >( material() );
    if ( gradientMaterial->updateGradient( rect, gradient, extraValue ) )
        markDirty( QSGNode::DirtyMaterial );
}

