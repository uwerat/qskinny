/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskRectangleNode.h"
#include "QskGradient.h"
#include "QskSGNode.h"
#include "QskBoxRenderer.h"
#include "QskGradientMaterial.h"

#include <qglobalstatic.h>
#include <qsgvertexcolormaterial.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
QSK_QT_PRIVATE_END

Q_GLOBAL_STATIC( QSGVertexColorMaterial, qskMaterialColorVertex )

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

class QskRectangleNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    QskRectangleNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_ColoredPoint2D(), 0 )
    {
    }

    QSGGeometry geometry;

    QRectF rect;
    QskHashValue gradientHash = 0;

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
    Q_D( QskRectangleNode );

    if ( rect.isEmpty() || !gradient.isVisible() )
    {
        d->rect = QRectF();
        d->gradientHash = 0;
        QskSGNode::resetGeometry( this );

        return;
    }

    const auto effectiveGradient = qskEffectiveGradient( gradient );

    const auto gradientHash = effectiveGradient.hash( 54228 );

    const bool dirtyGradient = gradientHash != d->gradientHash;
    const bool dirtyRect = rect != d->rect;

    if ( !( dirtyGradient || dirtyRect ) )
        return;

    d->gradientHash = gradientHash;
    d->rect = rect;

    if ( QskBoxRenderer::isGradientSupported( effectiveGradient ) )
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
        if ( dirtyRect || dirtyGradient )
        {
            QskBoxRenderer::renderRect( rect, effectiveGradient, d->geometry );
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
        if ( dirtyRect )
        {
            QskBoxRenderer::renderRect( rect, d->geometry );
            markDirty( QSGNode::DirtyGeometry );
        }

        if ( dirtyGradient )
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
