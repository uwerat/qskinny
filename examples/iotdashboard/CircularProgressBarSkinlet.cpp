/******************************************************************************
 * QSkinny - Copyright (C) 2021 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "CircularProgressBarSkinlet.h"
#include "CircularProgressBar.h"

#include <QskArcNode.h>
#include <QskPaintedNode.h>

#include <QEasingCurve>
#include <QPainter>

CircularProgressBarSkinlet::CircularProgressBarSkinlet( QskSkin* skin )
    : QskSkinlet( skin )
{
    setNodeRoles( { GrooveRole, BarRole } );
}

CircularProgressBarSkinlet::~CircularProgressBarSkinlet()
{
}

QRectF CircularProgressBarSkinlet::subControlRect(
    const QskSkinnable*, const QRectF& contentsRect, QskAspect::Subcontrol ) const
{
    return contentsRect;
}

QSGNode* CircularProgressBarSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto bar = static_cast< const CircularProgressBar* >( skinnable );

    switch( nodeRole )
    {
        case GrooveRole: // fall through
        case BarRole:
        {
            return updateBarNode( bar, nodeRole, node );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* CircularProgressBarSkinlet::updateBarNode(
    const CircularProgressBar* bar, quint8 nodeRole, QSGNode* node ) const
{
    auto arcNode = static_cast< QskArcNode* >( node );

    if( arcNode == nullptr )
    {
        arcNode = new QskArcNode();
    }

    // ### for the groove case, we can just call updateArcNode directly,
    // but not for the bar case, because we need to change the angles
    // for the latter case, we can just set the metrics rather than having
    // this method here
    const auto subControl = ( nodeRole == GrooveRole ) ?
                CircularProgressBar::Groove : CircularProgressBar::Bar;

    const QRectF rect = bar->contentsRect(); // ### rather call subcontrolrect

    QskArcMetrics arcMetrics = bar->arcMetricsHint( subControl );
    const int spanAngle = ( nodeRole == GrooveRole ) ?
                5760 : qRound( bar->valueAsRatio() * -5760 );
    arcMetrics.setSpanAngle( spanAngle );

    QQuickWindow* window = bar->window();

    const QskGradient gradient = bar->gradientHint( subControl );

    arcNode->setArcData( rect, arcMetrics, gradient, window );

    return arcNode;
}

#include "moc_CircularProgressBarSkinlet.cpp"
