/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskPopupSkinlet.h"
#include "QskPopup.h"
#include "QskRgbValue.h"

static inline QRgb qskInterpolatedRgb( QRgb rgb, qreal factor )
{
    return QskRgb::toTransparent( rgb, qRound( factor * qAlpha( rgb ) ) );
}

QskPopupSkinlet::QskPopupSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    appendNodeRoles( { OverlayRole } );
}

QskPopupSkinlet::~QskPopupSkinlet() = default;

QRectF QskPopupSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto popup = static_cast< const QskPopup* >( skinnable );

    if ( subControl == QskPopup::Overlay )
        return popup->overlayRect();

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskPopupSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto popup = static_cast< const QskPopup* >( skinnable );

    switch ( nodeRole )
    {
        case OverlayRole:
            return updateOverlayNode( popup, node );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskPopupSkinlet::updateOverlayNode(
    const QskPopup* popup, QSGNode* node ) const
{
    using Q = QskPopup;

    const auto factor = popup->fadingFactor();
    if ( factor <= 0.0 )
        return nullptr;

    const auto rect = popup->subControlRect( Q::Overlay );
    if ( rect.isEmpty() )
        return nullptr;

    auto gradient = popup->gradientHint( Q::Overlay );

    if ( gradient.isVisible() && factor != 1.0 )
    {
        auto stops = gradient.stops();

        for ( auto& stop : stops )
            stop.setRgb( qskInterpolatedRgb( stop.rgb(), factor ) );

        gradient.setStops( stops );
    }

    return updateBoxNode( popup, node, rect, gradient, QskPopup::Overlay );
}

#include "moc_QskPopupSkinlet.cpp"
