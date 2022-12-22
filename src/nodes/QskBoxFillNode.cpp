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

class QskBoxFillNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    QskBoxFillNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
    {
    }

    inline void resetValues()
    {
        rect = QRectF();
        gradientHash = 0;
        metricsHash = 0;
    }

    QRectF rect;

    QskHashValue gradientHash = 0;
    QskHashValue metricsHash = 0;

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

void QskBoxFillNode::updateNode( const QRectF& rect, const QskGradient& gradient )
{
    updateNode( rect, QskBoxShapeMetrics(), QskBoxBorderMetrics(), gradient );
}

void QskBoxFillNode::updateNode(
    const QRectF& rect, const QskBoxShapeMetrics& shapeMetrics,
    const QskBoxBorderMetrics& borderMetrics, const QskGradient& gradient )
{
    Q_D( QskBoxFillNode );

    if ( rect.isEmpty() || !gradient.isVisible() )
    {
        d->resetValues();
        QskSGNode::resetGeometry( this );

        return;
    }

    const auto metricsHash = qskMetricsHash( shapeMetrics, borderMetrics );
    const auto gradientHash = gradient.hash( 22879 );

    const bool dirtyColors = gradientHash != d->gradientHash;
    const bool dirtyMetrics = ( metricsHash != d->metricsHash ) || ( rect != d->rect );

    d->metricsHash = metricsHash;
    d->gradientHash = gradientHash;
    d->rect = rect;

    if ( dirtyMetrics )
    {
        QskBoxRenderer::renderFill( rect, shapeMetrics, borderMetrics, d->geometry );
        markDirty( QSGNode::DirtyGeometry );
    }

    if ( gradient.isMonochrome() )
    {
        if ( dirtyColors )
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
    }
    else
    {
        // dirtyMetrics: the shader also depends on the target rectangle !

        if ( dirtyColors || dirtyMetrics )
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
}
