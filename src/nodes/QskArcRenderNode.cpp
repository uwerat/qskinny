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
#include "QskRgbValue.h"
#include "QskFillNodePrivate.h"

static inline bool qskHasBorder( qreal width, const QColor& color )
{
    return ( width > 0.0 ) && QskRgb::isVisible( color );
}

class QskArcRenderNodePrivate final : public QskFillNodePrivate
{
  public:
    inline void resetNode( QskArcRenderNode* node )
    {
        m_metricsHash = m_colorsHash = 0;
        node->resetGeometry();
    }

    inline bool updateMetrics( const QRectF& rect,
        const QskArcMetrics& metrics, bool radial, qreal borderWidth )
    {
        QskHashValue hash = 13000;

        hash = qHashBits( &rect, sizeof( rect ), hash );
        hash = metrics.hash( hash );
        hash = qHash( radial, hash );
        hash = qHash( borderWidth, hash );

        return updateHash( m_metricsHash, hash );
    }

    inline bool updateColors( const QColor& borderColor, const QskGradient& gradient )
    {
        QskHashValue hash = 13000;

        if ( QskRgb::isVisible( borderColor ) )
            hash = qHashBits( &borderColor, sizeof( borderColor ), hash );

        if ( gradient.isVisible() )
            hash = gradient.hash( hash );

        return updateHash( m_colorsHash, hash );
    }

  private:
    inline bool updateHash( QskHashValue& value, const QskHashValue newValue ) const
    {
        if ( newValue != value )
        {
            value = newValue;
            return true;
        }

        return false;
    }

  public:
    QskHashValue m_metricsHash = 0;
    QskHashValue m_colorsHash = 0;
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

    if ( rect.isEmpty() || arcMetrics.isNull() || !gradient.isVisible() )
    {
        d->resetNode( this );
        return;
    }

    const auto metrics = arcMetrics.toAbsolute( rect.size() );

    if ( borderWidth >= 0.5 * metrics.thickness() )
    {
        d->resetNode( this );
        return;
    }

    bool coloredGeometry = hasHint( PreferColoredGeometry );
    if ( coloredGeometry )
    {
        // not all gradients are supported by the renderer
        coloredGeometry = QskArcRenderer::isGradientSupported( rect, metrics, gradient );
    }
    else
    {
        // QskGradient::Stops is specific for QskArcRenderer
        coloredGeometry = ( gradient.type() == QskGradient::Stops );
    }

    bool dirtyGeometry = d->updateMetrics( rect, metrics, radial, borderWidth );
    bool dirtyMaterial = d->updateColors( QColor(), gradient );

    if ( coloredGeometry != isGeometryColored() )
        dirtyGeometry = dirtyMaterial = true;

    if ( dirtyGeometry || dirtyMaterial )
    {
        if ( coloredGeometry )
        {
            setColoring( QskFillNode::Polychrome );

            QskArcRenderer::setColoredFillLines( rect, metrics, radial,
                borderWidth, gradient, *geometry() );

            markDirty( QSGNode::DirtyGeometry );
        }
        else
        {
            setColoring( rect, gradient );

            if ( dirtyGeometry )
            {
                QskArcRenderer::setFillLines( rect, metrics,
                    radial, borderWidth, *geometry() );

                markDirty( QSGNode::DirtyGeometry );
            }
        }
    }
}

void QskArcRenderNode::updateBorder( const QRectF& rect,
    const QskArcMetrics& arcMetrics, bool radial,
    qreal borderWidth, const QColor& borderColor )
{
    Q_D( QskArcRenderNode );

    if ( rect.isEmpty() || arcMetrics.isNull()
        || !qskHasBorder( borderWidth, borderColor ) )
    {
        d->resetNode( this );
        return;
    }

    const bool coloredGeometry = hasHint( PreferColoredGeometry );

    bool dirtyGeometry = d->updateMetrics( rect, arcMetrics, radial, borderWidth );
    bool dirtyMaterial = d->updateColors( borderColor, QskGradient() );

    if ( coloredGeometry != isGeometryColored() )
        dirtyGeometry = dirtyMaterial = true;

    if ( dirtyGeometry || dirtyMaterial )
    {
        const auto metrics = arcMetrics.toAbsolute( rect.size() );
        borderWidth = qMin( borderWidth, 0.5 * metrics.thickness() );

        if ( coloredGeometry )
        {
            setColoring( QskFillNode::Polychrome );

            QskArcRenderer::setColoredBorderLines( rect, metrics, radial,
                borderWidth, borderColor, *geometry() );

            markDirty( QSGNode::DirtyGeometry );
        }
        else
        {
            setColoring( borderColor );

            if ( dirtyGeometry )
            {
                QskArcRenderer::setBorderLines( rect, metrics, radial,
                    borderWidth, *geometry() );

                markDirty( QSGNode::DirtyGeometry );
            }
        }
    }
}

void QskArcRenderNode::updateArc(
    const QRectF& rect, const QskArcMetrics& arcMetrics, bool radial,
    qreal borderWidth, const QColor& borderColor, const QskGradient& gradient )
{
    Q_D( QskArcRenderNode );

    const auto metrics = arcMetrics.toAbsolute( rect.size() );
    if ( rect.isEmpty() || metrics.isNull() )
    {
        d->resetNode( this );
        return;
    }

    const auto borderMax = 0.5 * metrics.thickness();

    const bool hasFill = gradient.isVisible() && ( borderWidth < borderMax );
    const bool hasBorder = qskHasBorder( borderWidth, borderColor );

    if ( hasBorder && hasFill )
    {
        /*
            When having a monochrome gradient that has the same color as the
            border we might be able to use QskFillNode::Monochrome. For the
            moment this is not implemented
         */
        borderWidth = qMin( borderWidth, borderMax );

        const bool isDirty = d->updateMetrics( rect, arcMetrics, radial, borderWidth )
            || d->updateColors( borderColor, gradient ) || !isGeometryColored();

        if ( isDirty )
        {
            setColoring( QskFillNode::Polychrome );

            QskArcRenderer::setColoredBorderAndFillLines( rect, metrics, radial,
                borderWidth, borderColor, gradient, *geometry() );

            markDirty( QSGNode::DirtyGeometry );
        }
    }
    else if ( hasBorder )
    {
        updateBorder( rect, arcMetrics, radial, borderWidth, borderColor );
    }
    else if ( hasFill )
    {
        updateFilling( rect, arcMetrics, radial, borderWidth, gradient );
    }
    else
    {
        d->resetNode( this );
        return;
    }
}
