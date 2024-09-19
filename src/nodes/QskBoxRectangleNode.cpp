/******************************************************************************
 * QSkinny - Copyright (C) The authors
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

class QskBoxRectangleNodePrivate final : public QskFillNodePrivate
{
  public:
    inline void resetNode( QskBoxRectangleNode* node )
    {
        m_metricsHash = m_colorsHash = 0;
        node->resetGeometry();
    }

    bool updateMetrics( const QRectF& rect,
        const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& borderMetrics )
    {
        QskHashValue hash = 13000;

        hash = qHashBits( &rect, sizeof( rect ), hash );
        hash = shape.hash( hash );
        hash = borderMetrics.hash( hash );

        return updateValue( m_metricsHash, hash );
    }

    bool updateColors( const QskBoxBorderColors& borderColors, const QskGradient& gradient )
    {
        QskHashValue hash = 13000;

        if ( borderColors.isVisible() )
            hash = borderColors.hash( hash );

        if ( gradient.isVisible() )
            hash = gradient.hash( hash );

        return updateValue( m_colorsHash, hash );
    }

  private:
    inline bool updateValue( QskHashValue& value, const QskHashValue newValue ) const
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

QskBoxRectangleNode::QskBoxRectangleNode()
    : QskFillNode( *new QskBoxRectangleNodePrivate )
{
}

QskBoxRectangleNode::~QskBoxRectangleNode()
{
}

void QskBoxRectangleNode::updateFilling(
    const QRectF& rect, const QskGradient& gradient )
{
    updateFilling( rect, QskBoxShapeMetrics(), QskBoxBorderMetrics(), gradient );
}

void QskBoxRectangleNode::updateFilling( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskGradient& gradient )
{
    updateFilling( rect, shape, QskBoxBorderMetrics(), gradient );
}

void QskBoxRectangleNode::updateFilling( const QRectF& rect,
    const QskBoxShapeMetrics& shapeMetrics, const QskBoxBorderMetrics& borderMetrics,
    const QskGradient& gradient )
{
    Q_D( QskBoxRectangleNode );

    if ( rect.isEmpty() || !gradient.isVisible() )
    {
        d->resetNode( this );
        return;
    }

    const auto fillGradient = gradient.effectiveGradient();
    const auto shape = shapeMetrics.toAbsolute( rect.size() );

    const bool coloredGeometry = hasHint( PreferColoredGeometry )
        && QskBoxRenderer::isGradientSupported( fillGradient );

    const bool dirtyMetrics = d->updateMetrics( rect, shape, borderMetrics );
    const bool dirtyColors = d->updateColors( QskBoxBorderColors(), fillGradient )
        && ( coloredGeometry == isGeometryColored() );

    if ( dirtyMetrics || dirtyColors )
    {
        if ( coloredGeometry )
        {
            setColoring( QskFillNode::Polychrome );

            QskBoxRenderer::setColoredFillLines( rect, shape,
                borderMetrics, fillGradient, *geometry() );

            markDirty( QSGNode::DirtyGeometry );
        }
        else
        {
            if ( fillGradient.isMonochrome() )
            {
                if ( dirtyColors )
                    setColoring( fillGradient.rgbStart() );
            }
            else
            {
                // dirtyMetrics: the shader also depends on rect !
                setColoring( rect, fillGradient );
            }

            if ( dirtyMetrics )
            {
                QskBoxRenderer::setFillLines(
                    rect, shape, borderMetrics, *geometry() );

                markDirty( QSGNode::DirtyGeometry );
            }
        }
    }
}

void QskBoxRectangleNode::updateBorder( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& borderMetrics,
    const QskBoxBorderColors& borderColors )
{
    Q_D( QskBoxRectangleNode );

    if ( rect.isEmpty() || borderMetrics.isNull() || !borderColors.isVisible() )
    {
        d->resetNode( this );
        return;
    }

    const bool dirtyMetrics = d->updateMetrics( rect, shape, borderMetrics );
    const bool dirtyColors = d->updateColors( borderColors, QskGradient() );

    if ( dirtyMetrics || dirtyColors )
    {
        const auto coloring = QskFillNode::Polychrome;

        if ( coloring == QskFillNode::Polychrome )
            setColoring( coloring );
        else
            setColoring( borderColors.left().rgbStart() );

        QskBoxRenderer::setColoredBorderLines( rect, shape, borderMetrics,
            borderColors, *this->geometry() );

        markDirty( QSGNode::DirtyGeometry );
    }
}

void QskBoxRectangleNode::updateBox( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& borderMetrics,
    const QskBoxBorderColors& borderColors, const QskGradient& gradient )
{
    Q_D( QskBoxRectangleNode );

    if ( rect.isEmpty() )
    {
        d->resetNode( this );
        return;
    }

    const bool hasFill = gradient.isVisible();
    const bool hasBorder = !borderMetrics.isNull() && borderColors.isVisible();

    if ( hasFill && hasBorder )
    {
        const bool dirtyMetrics = d->updateMetrics( rect, shape, borderMetrics );
        const bool dirtyColors = d->updateColors( borderColors, gradient );

        if ( dirtyMetrics || dirtyColors )
        {
            /*
                For monochrome border/fiiling with the same color we might be
                able to do QskFillNode::Monochrome. However this is not implemeted in
                QskBoxRenderer yet. TODO ...
             */
            setColoring( QskFillNode::Polychrome );

            auto fillGradient = gradient.effectiveGradient();
            if ( !QskBoxRenderer::isGradientSupported( fillGradient ) )
            {
                qWarning() << "QskBoxRenderer does not support radial/conic gradients";
                fillGradient.setDirection( QskGradient::Linear );
            }

            QskBoxRenderer::setColoredBorderAndFillLines( rect, shape, borderMetrics,
                borderColors, fillGradient, *geometry() );

            markDirty( QSGNode::DirtyGeometry );
        }
    }
    else if ( hasFill )
    {
        updateFilling( rect, shape, borderMetrics, gradient );
    }
    else if ( hasBorder )
    {
        updateBorder( rect, shape, borderMetrics, borderColors );
    }
    else
    {
        d->resetNode( this );
    }
}

bool QskBoxRectangleNode::isCombinedGeometrySupported( const QskGradient& gradient )
{
    return QskBoxRenderer::isGradientSupported( gradient );
}
