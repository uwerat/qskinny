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

    const auto subControl = ( nodeRole == GrooveRole ) ? CircularProgressBar::Groove
                                                       : CircularProgressBar::Bar;

    const QskGradient gradient = bar->gradientHint( subControl );

    const QGradient::Type type = ( nodeRole == GrooveRole ) ?
        QGradient::RadialGradient : QGradient::ConicalGradient;

    const double width = bar->metric( subControl | QskAspect::Size );
    const double value = ( nodeRole == GrooveRole ) ? bar->maximum() : bar->value();
    const double position = bar->metric( CircularProgressBar::Bar | QskAspect::Position );

    arcNode->setArcData( gradient, type, width, value, bar->origin(), bar->maximum(),
        bar->isIndeterminate(), position );

    QQuickWindow* window = bar->window();
    const QRect rect = bar->contentsRect().toRect();
    arcNode->update( window, QskTextureRenderer::AutoDetect, rect );

    return arcNode;
}

#include "moc_CircularProgressBarSkinlet.cpp"
