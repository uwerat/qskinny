/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxRectangleNode.h"
#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxRenderer.h"
#include "QskBoxShapeMetrics.h"
#include "QskGradient.h"
#include "QskGradientDirection.h"

#include <qglobalstatic.h>
#include <qsgflatcolormaterial.h>
#include <qsgvertexcolormaterial.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
QSK_QT_PRIVATE_END

Q_GLOBAL_STATIC( QSGVertexColorMaterial, qskMaterialColorVertex )

static inline QskHashValue qskMetricsHash(
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& borderMetrics )
{
    QskHashValue hash = 13000;

    hash = shape.hash( hash );
    return borderMetrics.hash( hash );
}

static inline QskHashValue qskColorsHash(
    const QskBoxBorderColors& borderColors, const QskGradient& fillGradient )
{
    QskHashValue hash = 13000;
    hash = borderColors.hash( hash );
    return fillGradient.hash( hash );
}

#if 1

static inline QskGradient qskEffectiveGradient( const QskGradient& gradient )
{
    auto g = gradient.effectiveGradient();

    if ( g.type() != QskGradient::Linear )
    {
        qWarning() << "QskBoxRectangleNode does not support radial/conic gradients";
        g.setDirection( QskGradient::Linear );
    }

    return g;
}

#endif

class QskBoxRectangleNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    QskBoxRectangleNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_ColoredPoint2D(), 0 )
    {
    }

    QskHashValue metricsHash = 0;
    QskHashValue colorsHash = 0;
    QRectF rect;

    QSGGeometry geometry;
};

QskBoxRectangleNode::QskBoxRectangleNode()
    : QSGGeometryNode( *new QskBoxRectangleNodePrivate )
{
    Q_D( QskBoxRectangleNode );

    setMaterial( qskMaterialColorVertex );
    setGeometry( &d->geometry );
}

QskBoxRectangleNode::~QskBoxRectangleNode()
{
    if ( material() != qskMaterialColorVertex )
        delete material();
}

void QskBoxRectangleNode::updateNode(
    const QRectF& rect, const QskGradient& fillGradient )
{
    updateNode( rect, QskBoxShapeMetrics(), QskBoxBorderMetrics(),
        QskBoxBorderColors(), fillGradient );
}

void QskBoxRectangleNode::updateNode( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskGradient& fillGradient )
{
    updateNode( rect, shape, QskBoxBorderMetrics(),
        QskBoxBorderColors(), fillGradient );
}

void QskBoxRectangleNode::updateNode( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& borderMetrics,
    const QskBoxBorderColors& borderColors, const QskGradient& gradient )
{
    Q_D( QskBoxRectangleNode );

    /*
        We supports certain linear gradients only.
        Everything else has to be done using QskRectangleNode
     */
    const auto fillGradient = qskEffectiveGradient( gradient );

    const auto metricsHash = qskMetricsHash( shape, borderMetrics );
    const auto colorsHash = qskColorsHash( borderColors, fillGradient );

    if ( ( metricsHash == d->metricsHash ) &&
        ( colorsHash == d->colorsHash ) && ( rect == d->rect ) )
    {
        return;
    }

    d->metricsHash = metricsHash;
    d->colorsHash = colorsHash;
    d->rect = rect;

    markDirty( QSGNode::DirtyMaterial );
    markDirty( QSGNode::DirtyGeometry );

    if ( rect.isEmpty() )
    {
        d->geometry.allocate( 0 );
        return;
    }

    bool hasFill = fillGradient.isVisible();

    bool hasBorder = !borderMetrics.isNull();
    if ( hasBorder )
    {
        /*
            Wrong as the border width should have an
            effect - even if not being visible. TODO ...
         */

        hasBorder = borderColors.isVisible();
    }

    if ( !hasBorder && !hasFill )
    {
        d->geometry.allocate( 0 );
        return;
    }

    const bool isFillMonochrome = hasFill ? fillGradient.isMonochrome() : true;
    const bool isBorderMonochrome = hasBorder ? borderColors.isMonochrome() : true;

    if ( hasFill && hasBorder )
    {
        if ( isFillMonochrome && isBorderMonochrome )
        {
            if ( borderColors.left().rgbStart() == fillGradient.rgbStart() )
            {
                // we can draw border and background in one
                hasBorder = false;
            }
        }
    }

#if 0
    /*
        Always using the same material result in a better batching
        but wastes some memory. when we have a solid color.
        Maybe its worth to introduce a flag to control the behaviour,
        but for the moment we go with performance.
     */

    bool maybeFlat = true;

    if ( maybeFlat )
    {
        if ( ( hasFill && hasBorder ) ||
            ( hasFill && !isFillMonochrome ) ||
            ( hasBorder && !isBorderMonochrome ) )
        {
            maybeFlat = false;
        }
    }
#else
    bool maybeFlat = false;
#endif

    if ( !maybeFlat )
    {
        setMonochrome( false );

        QskBoxRenderer::renderBox( d->rect, shape, borderMetrics,
            borderColors, fillGradient, *geometry() );
    }
    else
    {
        // all is done with one color
        setMonochrome( true );

        auto* flatMaterial = static_cast< QSGFlatColorMaterial* >( material() );

        if ( hasFill )
        {
            flatMaterial->setColor( fillGradient.rgbStart() );
            QskBoxRenderer::renderFill( d->rect, shape, QskBoxBorderMetrics(), *geometry() );
        }
        else
        {
            flatMaterial->setColor( borderColors.left().rgbStart() );
            QskBoxRenderer::renderBorder( d->rect, shape, borderMetrics, *geometry() );
        }
    }
}

void QskBoxRectangleNode::setMonochrome( bool on )
{
    const auto material = this->material();

    if ( on == ( material != qskMaterialColorVertex ) )
        return;

    Q_D( QskBoxRectangleNode );

    d->geometry.allocate( 0 );

    if ( on )
    {
        setMaterial( new QSGFlatColorMaterial() );

        const QSGGeometry g( QSGGeometry::defaultAttributes_Point2D(), 0 );
        memcpy( ( void* ) &d->geometry, ( void* ) &g, sizeof( QSGGeometry ) );
    }
    else
    {
        setMaterial( qskMaterialColorVertex );
        delete material;

        const QSGGeometry g( QSGGeometry::defaultAttributes_ColoredPoint2D(), 0 );
        memcpy( ( void* ) &d->geometry, ( void* ) &g, sizeof( QSGGeometry ) );
    }
}
