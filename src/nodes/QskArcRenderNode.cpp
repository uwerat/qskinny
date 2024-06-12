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

QSK_QT_PRIVATE_BEGIN
#include <private/qsgnode_p.h>
QSK_QT_PRIVATE_END

#if 1
#include <qsgvertexcolormaterial.h>
#include <qglobalstatic.h>
// deriving from QskFillNode:TODO ...
Q_GLOBAL_STATIC( QSGVertexColorMaterial, qskMaterialColorVertex )
#endif

class QskArcRenderNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    QskArcRenderNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_ColoredPoint2D(), 0 )
    {
    }

    inline void resetValues()
    {
        hash = 0;
    }

    QSGGeometry geometry;
    QskHashValue hash = 0;
};

QskArcRenderNode::QskArcRenderNode()
    : QSGGeometryNode( *new QskArcRenderNodePrivate )
{
    Q_D( QskArcRenderNode );

    setGeometry( &d->geometry );

    setMaterial( qskMaterialColorVertex );
    setFlag( QSGNode::OwnsMaterial, false );
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

    bool visible = !( rect.isEmpty() || metrics.isNull() );
    if ( visible )
    {
        visible = gradient.isVisible();
        if ( !visible )
        {
            visible = ( borderWidth > 0.0 )
                && borderColor.isValid() && ( borderColor.alpha() > 0 );
        }
    }

    if ( !visible )
    {
        d->resetValues();
        QskSGNode::resetGeometry( this );

        return;
    }

    QskHashValue hash = 3496;

    hash = qHashBits( &rect, sizeof( QRectF ), hash );
    hash = qHash( borderWidth, hash );
    hash = qHash( borderColor.rgba(), hash );
    hash = metrics.hash( hash );
    hash = gradient.hash( hash );
    hash = qHash( radial, hash );

    if ( hash != d->hash )
    {
        d->hash = hash;

#if 0
        if ( borderWidth > 0.0 )
        {
            QskArcRenderer::renderBorder(
                rect, metrics, borderWidth, radial, borderColor, *geometry() );
        }
#endif

        QskArcRenderer::renderFillGeometry(
            rect, metrics, radial, borderWidth, gradient, *geometry() );

        markDirty( QSGNode::DirtyGeometry );
        markDirty( QSGNode::DirtyMaterial );
    }
}
