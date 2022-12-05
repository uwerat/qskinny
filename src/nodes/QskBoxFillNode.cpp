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
#include "QskSGNode.h"

#include <qsgflatcolormaterial.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
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

class QskBoxFillNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    QskBoxFillNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
    {
    }

    QskHashValue metricsHash = 0;
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

    if ( rect.isEmpty() || !gradient.isVisible() )
    {
        d->rect = QRectF();
        d->metricsHash = 0;
        QskSGNode::resetGeometry( this );

        return;
    }

    const auto metricsHash = qskMetricsHash( shapeMetrics, borderMetrics );
    const bool dirtyGeometry = ( metricsHash != d->metricsHash ) || ( rect == d->rect );

    d->metricsHash = metricsHash;
    d->rect = rect;

    if ( dirtyGeometry )
    {
        QskBoxRenderer::renderFill( rect, shapeMetrics, borderMetrics, d->geometry );
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
        const auto effectiveGradient = qskEffectiveGradient( gradient );
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
