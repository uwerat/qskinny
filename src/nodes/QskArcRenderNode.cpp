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

    QskHashValue hash = 0;
};

QskArcRenderNode::QskArcRenderNode()
    : QskFillNode( *new QskArcRenderNodePrivate )
{
}

QskArcRenderNode::~QskArcRenderNode()
{
}

void QskArcRenderNode::updateNode( const QRectF& rect,
    const QskArcMetrics& metrics, const QskGradient& gradient )
{
    updateNode( rect, metrics, false, 0.0, QColor(), gradient );
}

void QskArcRenderNode::updateNode( const QRectF& rect,
    const QskArcMetrics& metrics, qreal borderWidth, const QColor& borderColor )
{
    updateNode( rect, metrics, false, borderWidth, borderColor, QskGradient() );
}

void QskArcRenderNode::updateNode(
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

    QskHashValue hash = 3496;

    hash = qHashBits( &rect, sizeof( QRectF ), hash );
    hash = qHash( borderWidth, hash );
    hash = qHashBits( &borderColor, sizeof( borderColor ), hash );
    hash = metrics.hash( hash );
    hash = gradient.hash( hash );
    hash = qHash( radial, hash );

    if ( hash == d->hash )
        return;

    d->hash = hash;

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
