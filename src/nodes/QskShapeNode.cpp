/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskShapeNode.h"
#include "QskGradient.h"
#include "QskGradientDirection.h"
#include "QskFillNodePrivate.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qvectorpath_p.h>
#include <private/qtriangulator_p.h>
QSK_QT_PRIVATE_END

#if 0

// keeping the index list
static void qskUpdateGeometry( const QPainterPath& path,
    const QTransform& transform, QSGGeometry& geometry )
{
    const auto ts = qTriangulate( path, transform, 1, false );

    geometry.allocate( ts.vertices.size(), ts.indices.size() );

    auto vertexData = reinterpret_cast< float* >( geometry.vertexData() );
    const auto points = ts.vertices.constData();

    for ( int i = 0; i < ts.vertices.count(); i++ )
        vertexData[i] = points[i];

    memcpy( geometry.indexData(), ts.indices.data(),
        ts.indices.size() * sizeof( quint16 ) );
}

#endif

static void qskUpdateGeometry( const QPainterPath& path,
    const QTransform& transform, QSGGeometry& geometry )
{
    const auto ts = qTriangulate( path, transform, 1, false );

    /*
        The triangulation of a random path does not lead to index lists
        that are substantially reducing the number of vertices.

        As we have to iterate over the vertex buffer to copy qreal to float
        anyway we reorder according to the index buffer and drop
        the index buffer. 

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
}

class QskShapeNodePrivate final : public QskFillNodePrivate
{
  public:
    /*
        Is there a better way to find out if the path has changed
        beside storing a copy ( even, when internally with Copy On Write ) ?
     */
    QPainterPath path;
    QTransform transform;
};

QskShapeNode::QskShapeNode()
    : QskFillNode( *new QskShapeNodePrivate )
{
    setColoring( Monochrome );
    geometry()->setDrawingMode( QSGGeometry::DrawTriangles );
}

QskShapeNode::~QskShapeNode()
{
}

void QskShapeNode::updateNode( const QPainterPath& path,
    const QTransform& transform, const QRectF& rect, const QskGradient& gradient )
{
    Q_D( QskShapeNode );

    if ( path.isEmpty() || !gradient.isVisible() )
    {
        d->path = QPainterPath();
        d->transform = QTransform();
        resetGeometry();

        return;
    }

    setColoring( rect, gradient );

    if ( ( transform != d->transform ) || ( path != d->path ) )
    {
        d->path = path;
        d->transform = transform;

        qskUpdateGeometry( path, transform, *geometry() );

        geometry()->markVertexDataDirty();
        markDirty( QSGNode::DirtyGeometry );
    }
}
