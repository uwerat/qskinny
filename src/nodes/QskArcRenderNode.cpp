/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcRenderNode.h"
#include "QskGradient.h"
#include "QskArcRenderer.h"
#include "QskArcMetrics.h"
#include "QskGradient.h"
#include "QskSGNode.h"
#include "QskFillNodePrivate.h"

class QskArcRenderNodePrivate final : public QskFillNodePrivate
{
  public:
    inline void resetNode( QskArcRenderNode* node )
    {
        hash = 0;
        node->resetGeometry();
    }

    inline bool updateHash( const QRectF& rect, const QskArcMetrics& metrics,
        bool radial, qreal borderWidth, const QColor& borderColor,
        const QskGradient& gradient )
    {
        QskHashValue value = 3496;

        value = qHashBits( &rect, sizeof( QRectF ), value );
        value = metrics.hash( value );
        value = qHash( radial, value );
        value = qHash( borderWidth, value );
        value = qHashBits( &borderColor, sizeof( borderColor ), value );

        if ( gradient.isVisible() )
            value = gradient.hash( value );

        if ( hash != value )
        {
            hash = value;
            return true;
        }

        return false;
    }

  private:
    QskHashValue hash = 0;
};

QskArcRenderNode::QskArcRenderNode()
    : QskFillNode( *new QskArcRenderNodePrivate )
{
}

QskArcRenderNode::~QskArcRenderNode()
{
}

void QskArcRenderNode::updateFilling( const QRectF& rect,
    const QskArcMetrics& metrics, const QskGradient& gradient )
{
    updateFilling( rect, metrics, false, 0.0, gradient );
}

void QskArcRenderNode::updateFilling( const QRectF& rect,
    const QskArcMetrics& arcMetrics, bool radial,
    qreal borderWidth, const QskGradient& gradient )
{
    Q_D( QskArcRenderNode );

    const auto metrics = arcMetrics.toAbsolute( rect.size() );
    const auto borderMax = 0.5 * metrics.thickness();

    const bool hasFill = gradient.isVisible() && ( borderWidth < borderMax );
    const bool hasBorder = false;

    if ( rect.isEmpty() || metrics.isNull() || !( hasFill || hasBorder ) )
    {
        d->resetNode( this );
        return;
    }

    borderWidth = qMin( borderWidth, borderMax );

    if ( !d->updateHash( rect, metrics, radial, borderWidth, QColor(), gradient ) )
    {
        d->resetNode( this );
        return;
    }

#if 1
    const bool coloredGeometry = hasHint( PreferColoredGeometry )
        && QskArcRenderer::isGradientSupported( rect, metrics, gradient );
    Q_ASSERT( coloredGeometry ); // TODO ...
#endif

    auto& geometry = *this->geometry();

    setColoring( QskFillNode::Polychrome );

    QskArcRenderer::setColoredFillLines( rect, metrics, radial,
        borderWidth, gradient, geometry );

    markDirty( QSGNode::DirtyGeometry );
}

void QskArcRenderNode::updateBorder( const QRectF& rect,
    const QskArcMetrics& metrics, bool radial,
    qreal borderWidth, const QColor& borderColor )
{
    updateArc( rect, metrics, radial, borderWidth, borderColor, QskGradient() );
}

void QskArcRenderNode::updateArc(
    const QRectF& rect, const QskArcMetrics& arcMetrics, bool radial,
    qreal borderWidth, const QColor& borderColor, const QskGradient& gradient )
{
    Q_D( QskArcRenderNode );

    const auto metrics = arcMetrics.toAbsolute( rect.size() );
    const auto borderMax = 0.5 * metrics.thickness();

    const bool hasFill = gradient.isVisible() && ( borderWidth < borderMax );
    const bool hasBorder = ( borderWidth > 0.0 )
                && borderColor.isValid() && ( borderColor.alpha() > 0 );

    if ( rect.isEmpty() || metrics.isNull() || !( hasFill || hasBorder ) )
    {
        d->resetNode( this );
        return;
    }

    borderWidth = qMin( borderWidth, borderMax );

    if ( !d->updateHash( rect, metrics, radial, borderWidth, borderColor, gradient ) )
    {
        d->resetNode( this );
        return;
    }

#if 1
    const bool coloredGeometry = hasHint( PreferColoredGeometry )
        && QskArcRenderer::isGradientSupported( rect, metrics, gradient );
    Q_ASSERT( coloredGeometry ); // TODO ...
#endif

    auto& geometry = *this->geometry();

    setColoring( QskFillNode::Polychrome );

    QskArcRenderer::setColoredBorderAndFillLines( rect, metrics, radial,
        borderWidth, borderColor, gradient, geometry );

    markDirty( QSGNode::DirtyGeometry );
}
