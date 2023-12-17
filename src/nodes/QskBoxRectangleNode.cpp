/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskBoxRectangleNode.h"
#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxRenderer.h"
#include "QskBoxShapeMetrics.h"
#include "QskGradient.h"
#include "QskGradientDirection.h"
#include "QskFillNodePrivate.h"

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

class QskBoxRectangleNodePrivate final : public QskFillNodePrivate
{
  public:
    QskHashValue metricsHash = 0;
    QskHashValue colorsHash = 0;
    QRectF rect;
};

QskBoxRectangleNode::QskBoxRectangleNode()
    : QskFillNode( *new QskBoxRectangleNodePrivate )
{
}

QskBoxRectangleNode::~QskBoxRectangleNode()
{
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
        resetGeometry();
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
        resetGeometry();
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

    auto coloring = QskFillNode::Polychrome;

#if 0
    /*
        Always using the same material result in a better batching
        but wastes some memory. when we have a solid color.
        Maybe its worth to introduce a flag to control the behaviour,
        but for the moment we go with performance.
     */

    if ( !( hasFill && hasBorder ) )
    {
        if ( ( hasFill && isFillMonochrome )
            || ( hasBorder && !isBorderMonochrome )
        {
            coloring = QskFillNode::Monochrome;
        }
    }
#endif

    auto& geometry = *this->geometry();

    if ( coloring == QskFillNode::Polychrome )
    {
        setColoring( coloring );

        QskBoxRenderer::renderBox( d->rect, shape, borderMetrics,
            borderColors, fillGradient, geometry );
    }
    else
    {
        if ( hasFill )
        {
            setColoring( fillGradient.rgbStart() );
            QskBoxRenderer::renderFillGeometry(
                d->rect, shape, QskBoxBorderMetrics(), geometry );
        }
        else
        {
            setColoring( borderColors.left().rgbStart() );
            QskBoxRenderer::renderBorderGeometry(
                d->rect, shape, borderMetrics, geometry );
        }
    }

    geometry.markVertexDataDirty();
}
