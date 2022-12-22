/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskRectangleNode.h"
#include "QskGradient.h"
#include "QskSGNode.h"
#include "QskBoxRenderer.h"
#include "QskGradientMaterial.h"

#include "QskBoxBorderMetrics.h"
#include "QskBoxBorderColors.h"
#include "QskBoxShapeMetrics.h"

#include <qglobalstatic.h>
#include <qsgvertexcolormaterial.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
QSK_QT_PRIVATE_END

Q_GLOBAL_STATIC( QSGVertexColorMaterial, qskMaterialColorVertex )

static inline void qskUpdateColoredPoint2D( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskGradient& gradient,
    QSGGeometry& geometry )
{
    QskBoxRenderer::renderBox( rect, shape,
        QskBoxBorderMetrics(), QskBoxBorderColors(), gradient, geometry );
}

static inline void qskUpdatePoint2D( const QRectF& rect,
    const QskBoxShapeMetrics& shape, QSGGeometry& geometry )
{
    QskBoxRenderer::renderFill( rect, shape,
        QskBoxBorderMetrics(), geometry );
}

class QskRectangleNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    QskRectangleNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_ColoredPoint2D(), 0 )
    {
    }

    inline void resetValues()
    {
        rect = QRectF();
        shape = QskBoxShapeMetrics();
        gradientHash = 0;
        metricsHash = 0;
    }

    QSGGeometry geometry;

    QRectF rect;
    QskBoxShapeMetrics shape;

    QskHashValue gradientHash = 0;
    QskHashValue metricsHash = 0;

    int gradientType = -1;
};

QskRectangleNode::QskRectangleNode()
    : QSGGeometryNode( *new QskRectangleNodePrivate )
{
    Q_D( QskRectangleNode );

    setFlag( OwnsMaterial, true );
    setGeometry( &d->geometry );
}

QskRectangleNode::~QskRectangleNode()
{
}

void QskRectangleNode::updateNode(
    const QRectF& rect, const QskGradient& gradient )
{
    updateNode( rect, QskBoxShapeMetrics(), gradient );
}

void QskRectangleNode::updateNode(
    const QRectF& rect, const QskBoxShapeMetrics& shape, const QskGradient& gradient )
{
    Q_D( QskRectangleNode );

    if ( rect.isEmpty() || !gradient.isVisible() )
    {
        d->resetValues();
        QskSGNode::resetGeometry( this );

        return;
    }

    const auto effectiveGradient = gradient.effectiveGradient();
    const auto effectiveShape = shape.toAbsolute( rect.size() );

    const auto gradientHash = effectiveGradient.hash( 54228 );
    const auto metricsHash = effectiveShape.hash( 44564 );

    const bool dirtyColors = gradientHash != d->gradientHash;
    const bool dirtyMetrics = ( rect != d->rect ) || ( metricsHash != d->metricsHash );

    if ( !( dirtyColors || dirtyMetrics ) )
        return;

    d->gradientHash = gradientHash;
    d->metricsHash = metricsHash;
    d->rect = rect;
    d->shape = effectiveShape;

    if ( QskBoxRenderer::isGradientSupported( effectiveShape, effectiveGradient ) )
    {
        if ( material() != qskMaterialColorVertex )
        {
            setMaterial( qskMaterialColorVertex );
            setFlag( OwnsMaterial, false );

            d->gradientType = -1;
        }

        if ( d->geometry.attributeCount() == 1 )
        {
            const QSGGeometry g( QSGGeometry::defaultAttributes_ColoredPoint2D(), 0 );
            memcpy( ( void* ) &d->geometry, ( void* ) &g, sizeof( QSGGeometry ) );
        }
    }
    else
    {
        if ( material() == qskMaterialColorVertex )
        {
            setMaterial( nullptr );
            setFlag( OwnsMaterial, true );

            d->gradientType = -1;
        }

        if ( d->geometry.attributeCount() != 1 )
        {
            const QSGGeometry g( QSGGeometry::defaultAttributes_Point2D(), 0 );
            memcpy( ( void* ) &d->geometry, ( void* ) &g, sizeof( QSGGeometry ) );
        }
    }

    if ( material() == qskMaterialColorVertex )
    {
        /*
            Colors are added to the vertices, while the material does
            not depend on the gradient at all
         */
        if ( dirtyMetrics || dirtyColors )
        {
            qskUpdateColoredPoint2D( rect, effectiveShape,
                effectiveGradient, d->geometry );
            markDirty( QSGNode::DirtyGeometry );
        }
    }
    else
    {
        /*
            Colors are added by the shaders

            Monochrome gradients or QskGradient::Stops are supported by the
            QskBoxRenderer. So we don't need to handle them here.
         */
        if ( dirtyMetrics )
        {
            qskUpdatePoint2D( rect, effectiveShape, d->geometry );
            markDirty( QSGNode::DirtyGeometry );
        }

        // dirtyMetrics: the shader also depends on the target rectangle !
        if ( dirtyColors || dirtyMetrics )
        {
            const auto gradientType = effectiveGradient.type();

            if ( gradientType != d->gradientType )
            {
                setMaterial( QskGradientMaterial::createMaterial( gradientType ) );
                d->gradientType = gradientType;
            }

            auto mat = static_cast< QskGradientMaterial* >( material() );
            if ( mat->updateGradient( rect, effectiveGradient ) )
                markDirty( QSGNode::DirtyMaterial );
        }
    }
}
