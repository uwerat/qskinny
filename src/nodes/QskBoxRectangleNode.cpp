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

static inline bool qskHasBorder( 
    const QskBoxBorderMetrics& metrics, const QskBoxBorderColors& colors )
{
    return !metrics.isNull() && colors.isVisible();
}

class QskBoxRectangleNodePrivate final : public QskFillNodePrivate
{
  public:
    inline void resetNode( QskBoxRectangleNode* node )
    {
        m_metricsHash = m_colorsHash = 0;
        node->resetGeometry();
    }

    inline bool updateMetrics( const QRectF& rect,
        const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& borderMetrics )
    {
        QskHashValue hash = 13000;

        hash = qHashBits( &rect, sizeof( rect ), hash );
        hash = shape.hash( hash );
        hash = borderMetrics.hash( hash );

        return updateHash( m_metricsHash, hash );
    }

    inline bool updateColors(
        const QskBoxBorderColors& borderColors, const QskGradient& gradient )
    {
        QskHashValue hash = 13000;

        if ( borderColors.isVisible() )
            hash = borderColors.hash( hash );

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

    const auto fillGradient = QskBoxRenderer::effectiveGradient( gradient );
    const auto shape = shapeMetrics.toAbsolute( rect.size() );

    const bool coloredGeometry = hasHint( PreferColoredGeometry )
        && QskBoxRenderer::isGradientSupported( fillGradient );

    bool dirtyGeometry = d->updateMetrics( rect, shape, borderMetrics );
    bool dirtyMaterial = d->updateColors( QskBoxBorderColors(), fillGradient );

    if ( coloredGeometry != isGeometryColored() )
        dirtyGeometry = dirtyMaterial = true;

    if ( dirtyGeometry || dirtyMaterial )
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
            setColoring( rect, fillGradient );

            if ( dirtyGeometry )
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

    if ( rect.isEmpty() || !qskHasBorder( borderMetrics, borderColors ) )
    {
        d->resetNode( this );
        return;
    }

    const bool coloredGeometry = hasHint( PreferColoredGeometry )
        || !borderColors.isMonochrome();

    bool dirtyGeometry = d->updateMetrics( rect, shape, borderMetrics );
    bool dirtyMaterial = d->updateColors( borderColors, QskGradient() );

    if ( coloredGeometry != isGeometryColored() )
        dirtyGeometry = dirtyMaterial = true;

    if ( dirtyGeometry || dirtyMaterial )
    {
        if ( coloredGeometry )
        {
            setColoring( QskFillNode::Polychrome );

            QskBoxRenderer::setColoredBorderLines( rect, shape,
                borderMetrics, borderColors, *geometry() );

            markDirty( QSGNode::DirtyGeometry );
        }
        else
        {
            setColoring( borderColors.left().rgbStart() );

            if ( dirtyGeometry )
            {
                QskBoxRenderer::setBorderLines( rect, shape,
                    borderMetrics, *geometry() );

                markDirty( QSGNode::DirtyGeometry );
            }
        }
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
    const bool hasBorder = qskHasBorder( borderMetrics, borderColors );

    if ( hasFill && hasBorder )
    {
        const bool isDirty = d->updateMetrics( rect, shape, borderMetrics )
            || d->updateColors( borderColors, gradient ) || !isGeometryColored();

        if ( isDirty )
        {
            /*
                For monochrome border/filling with the same color we might be
                able to do QskFillNode::Monochrome. However this is not implemeted in
                QskBoxRenderer yet. TODO ...
             */
            setColoring( QskFillNode::Polychrome );

            auto fillGradient = QskBoxRenderer::effectiveGradient( gradient );
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
