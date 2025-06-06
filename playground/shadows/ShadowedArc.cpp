/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "ShadowedArc.h"
#include "ArcShadowNode.h"

#include <QskSkinlet.h>
#include <QskArcNode.h>
#include <QskArcHints.h>
#include <QskShadowMetrics.h>
#include <QskSGNode.h>
#include <QskRgbValue.h>

QSK_SUBCONTROL( ShadowedArc, Arc )

namespace
{
    class Skinlet : public QskSkinlet
    {
        using Inherited = QskSkinlet;

      public:
        enum NodeRoles { ShadowRole, ArcRole };

        Skinlet( QskSkin* skin = nullptr );

        QRectF subControlRect( const QskSkinnable*,
            const QRectF&, QskAspect::Subcontrol ) const override;

        QSGNode* updateSubNode( const QskSkinnable*,
            quint8 nodeRole, QSGNode* ) const override;

      private:
        QSGNode* updateArcNode( const ShadowedArc*, QSGNode* node ) const;
        QSGNode* updateShadowNode( const ShadowedArc*, QSGNode* node ) const;
    };

    Skinlet::Skinlet( QskSkin* skin )
        : QskSkinlet( skin )
    {
        setNodeRoles( { ShadowRole, ArcRole } );
    }

    QRectF Skinlet::subControlRect( const QskSkinnable* skinnable,
        const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
    {
        if ( subControl == ShadowedArc::Arc )
            return contentsRect;

        return Inherited::subControlRect( skinnable, contentsRect, subControl );
    }

    QSGNode* Skinlet::updateSubNode(
        const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
    {
        auto arc = static_cast< const ShadowedArc* >( skinnable );

        switch( nodeRole )
        {
            case ShadowRole:
                return updateShadowNode( arc, node );
                break;

            case ArcRole:
                return updateArcNode( arc, node );
        }

        return Inherited::updateSubNode( skinnable, nodeRole, node );
    }

    QSGNode* Skinlet::updateShadowNode( const ShadowedArc* arc, QSGNode* node ) const
    {
        using Q = ShadowedArc;

        const auto rect = arc->subControlRect( Q::Arc );
        if ( rect.isEmpty() )
            return nullptr;

        const auto color = arc->shadowColorHint( Q::Arc );
        if ( !QskRgb::isVisible( color ) )
            return nullptr;

        auto metricsArc = arc->arcMetricsHint( Q::Arc );
        metricsArc = metricsArc.toAbsolute( rect.size() );

        auto metrics = arc->shadowMetricsHint( Q::Arc );
        metrics = metrics.toAbsolute( rect.size() );

        const auto shadowRect = metrics.shadowRect( rect );
        const auto spreadRadius = metrics.spreadRadius() + 0.5 * metricsArc.thickness();

        auto shadowNode = QskSGNode::ensureNode< ArcShadowNode >( node );
        shadowNode->setShadowData( shadowRect, spreadRadius, metrics.blurRadius(),
            metricsArc.startAngle(), metricsArc.spanAngle(), color );

        return shadowNode;;
    }

    QSGNode* Skinlet::updateArcNode( const ShadowedArc* arc, QSGNode* node ) const
    {
        using Q = ShadowedArc;

        const auto rect = arc->subControlRect( Q::Arc );
        if ( rect.isEmpty() )
            return nullptr;

        auto arcNode = QskSGNode::ensureNode< QskArcNode >( node );
        arcNode->setArcData( rect, arc->arcHints( Q::Arc ) );

        return arcNode;
    }
}

ShadowedArc::ShadowedArc( QQuickItem* parent )
    : Inherited( parent )
{
    auto skinlet = new Skinlet();
    skinlet->setOwnedBySkinnable( true );

    setSkinlet( skinlet );

    // initial settings

    setArcMetrics( { 0.0, 360.0, 1.0, Qt::RelativeSize } );

    setFillGradient( Qt::darkRed );

    setBorderWidth( 0 );
    setBorderColor( Qt::gray );

    setShadowColor( Qt::black );
    setShadowMetrics( { 0, 0, QPointF( 0, 0 ), Qt::AbsoluteSize } );
}

ShadowedArc::~ShadowedArc()
{
}

void ShadowedArc::setThickness( qreal thickness )
{
    auto metrics = arcMetrics();
    metrics.setThickness( thickness );

    setArcMetrics( metrics );
}

qreal ShadowedArc::thickness() const
{
    return arcMetrics().thickness();
}

void ShadowedArc::setBorderWidth( qreal width )
{
    width = std::max( width, 0.0 );
    setMetric( Arc | QskAspect::Border, width );
}

qreal ShadowedArc::borderWidth() const
{
    return metric( Arc | QskAspect::Border );
}

void ShadowedArc::setStartAngle( qreal degrees )
{
    auto metrics = arcMetrics();
    metrics.setStartAngle( degrees );

    setArcMetrics( metrics );
}

qreal ShadowedArc::startAngle() const
{
    return arcMetrics().startAngle();
}

void ShadowedArc::setSpanAngle( qreal degrees )
{
    auto metrics = arcMetrics();
    metrics.setSpanAngle( degrees );

    setArcMetrics( metrics );
}

qreal ShadowedArc::spanAngle() const
{
    return arcMetrics().spanAngle();
}

void ShadowedArc::setOffsetX( qreal dx )
{
    auto metrics = shadowMetrics();
    metrics.setOffsetX( dx );

    setShadowMetrics( metrics );
}

qreal ShadowedArc::offsetX() const
{
    return shadowMetrics().offset().x();
}

void ShadowedArc::setOffsetY( qreal dy )
{
    auto metrics = shadowMetrics();
    metrics.setOffsetY( dy );

    setShadowMetrics( metrics );
}

qreal ShadowedArc::offsetY() const
{
    return shadowMetrics().offset().y();
}

void ShadowedArc::setSpreadRadius( qreal radius )
{
    auto metrics = shadowMetrics();
    metrics.setSpreadRadius( radius );

    setShadowMetrics( metrics );
}

qreal ShadowedArc::spreadRadius() const
{
    return shadowMetrics().spreadRadius();
}

void ShadowedArc::setBlurRadius( qreal radius )
{
    auto metrics = shadowMetrics();
    metrics.setBlurRadius( radius );

    setShadowMetrics( metrics );
}

qreal ShadowedArc::blurRadius() const
{
    return shadowMetrics().blurRadius();
}

void ShadowedArc::setFillGradient( const QskGradient& gradient )
{
    setGradientHint( Arc, gradient );
}

QskGradient ShadowedArc::fillGradient() const
{
    return gradientHint( Arc );
}

void ShadowedArc::setShadowColor( const QColor& color )
{
    setShadowColorHint( Arc, color );
}

QColor ShadowedArc::shadowColor() const
{
    return shadowColorHint( Arc );
}

void ShadowedArc::setBorderColor( const QColor& color )
{
    setColor( Arc | QskAspect::Border, color );
}

QColor ShadowedArc::borderColor() const
{
    return color( Arc | QskAspect::Border );
}

QskShadowMetrics ShadowedArc::shadowMetrics() const
{
    return shadowMetricsHint( Arc );
}

void ShadowedArc::setShadowMetrics( const QskShadowMetrics& metrics )
{
    setShadowMetricsHint( Arc, metrics );
}

QskArcMetrics ShadowedArc::arcMetrics() const
{
    return arcMetricsHint( Arc );
}

void ShadowedArc::setArcMetrics( const QskArcMetrics& metrics )
{
    setArcMetricsHint( Arc, metrics );
}

#include "moc_ShadowedArc.cpp"
