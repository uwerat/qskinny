/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "CircularProgressBarSkinlet.h"
#include "CircularProgressBar.h"

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
    switch( nodeRole )
    {
        case GrooveRole:
        {
            return updateArcNode( skinnable, node, CircularProgressBar::Groove );
        }
        case BarRole:
        {
            const auto bar = static_cast< const CircularProgressBar* >( skinnable );

            const qreal startAngle = 90.0;
            const qreal spanAngle = 360.0 * bar->valueAsRatio();

            return updateArcNode( skinnable, node, startAngle, -spanAngle,
                CircularProgressBar::Fill );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

#include "moc_CircularProgressBarSkinlet.cpp"
