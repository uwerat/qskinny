/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxFillNode.h"
#include "QskGradientMaterial.h"
#include "QskGradient.h"
#include "QskGradientDirection.h"
#include "QskBoxShapeMetrics.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxRenderer.h"

#include <qsgflatcolormaterial.h>
#include <qpainterpath.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
#include <private/qvectorpath_p.h>
#include <private/qtriangulator_p.h>
QSK_QT_PRIVATE_END

static inline QskHashValue qskMetricsHash(
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& borderMetrics )
{
    QskHashValue hash = 13000;

    hash = shape.hash( hash );
    return borderMetrics.hash( hash );
}

static inline QskGradient qskEffectiveGradient( const QskGradient& gradient )
{
    if ( gradient.type() == QskGradient::Stops || gradient.isMonochrome() )
    {
        // the shader for linear gradients is the fastest
        QskGradient g;
        g.setLinearDirection( Qt::Vertical );
        g.setStops( gradient.stops() );

        return g;
    }

    return gradient;
}

static void qskUpdateGeometry( const QPainterPath& path, QSGGeometry& geometry )
{
    const auto ts = qTriangulate( path, QTransform(), 1, false );

    /*
        As we have to iterate over the vertex buffer to copy qreal to float
        anyway we reorder and drop the index buffer.

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

static inline void qskResetGeometry( QskBoxFillNode* node )
{
    auto g = node->geometry();
    if ( g->vertexCount() > 0 )
    {
        g->allocate( 0 );
        node->markDirty( QSGNode::DirtyGeometry );
    }
}

class QskBoxFillNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    QskBoxFillNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
    {
        geometry.setDrawingMode( QSGGeometry::DrawTriangles );
    }

    QskHashValue metricsHash = 0;
    QskHashValue gradientHash = 0;
    QRectF rect;

    QSGGeometry geometry;
    int gradientType = -1;
};

QskBoxFillNode::QskBoxFillNode()
    : QSGGeometryNode( *new QskBoxFillNodePrivate )
{
    Q_D( QskBoxFillNode );

    setGeometry( &d->geometry );
    setMaterial( new QSGFlatColorMaterial() );
    setFlag( QSGNode::OwnsMaterial, true );
}

void QskBoxFillNode::updateNode(
    const QRectF& rect, const QskBoxShapeMetrics& shapeMetrics,
    const QskBoxBorderMetrics& borderMetrics, const QskGradient& gradient )
{
    Q_D( QskBoxFillNode );

    const auto effectiveGradient = qskEffectiveGradient( gradient );

    const auto metricsHash = qskMetricsHash( shapeMetrics, borderMetrics );
    const auto gradientHash = effectiveGradient.hash( 17321 );

    const bool dirtyGeometry = ( metricsHash != d->metricsHash ) || ( rect == d->rect );
    const bool dirtyMaterial = gradientHash != d->gradientHash;

    if ( !( dirtyGeometry || dirtyMaterial ) )
        return;

    d->metricsHash = metricsHash;
    d->gradientHash = gradientHash;
    d->rect = rect;

    if ( rect.isEmpty() || !effectiveGradient.isVisible() )
    {
        qskResetGeometry( this );
        return;
    }

    if ( dirtyGeometry )
    {
        const auto path = QskBoxRenderer().fillPath(
            rect, shapeMetrics, borderMetrics );

        if ( path.isEmpty() )
        {
            qskResetGeometry( this );
            return;
        }

        qskUpdateGeometry( path, d->geometry );

        markDirty( QSGNode::DirtyGeometry );
    }

    if ( dirtyMaterial )
    {
        if ( effectiveGradient.isMonochrome() )
        {
            if ( material() == nullptr || d->gradientType >= 0 )
            {
                setMaterial( new QSGFlatColorMaterial() );
                d->gradientType = -1;
            }

            auto colorMaterial = static_cast< QSGFlatColorMaterial* >( material() );
            colorMaterial->setColor( effectiveGradient.startColor().toRgb() );
        }
        else
        {
            const auto gradientType = effectiveGradient.type();

            if ( ( material() == nullptr ) || ( gradientType != d->gradientType ) )
            {
                setMaterial( QskGradientMaterial::createMaterial( gradientType ) );
                d->gradientType = gradientType;
            }

            auto gradientMaterial = static_cast< QskGradientMaterial* >( material() );
            gradientMaterial->updateGradient( rect, effectiveGradient );
        }

        markDirty( QSGNode::DirtyMaterial );
    }
}
