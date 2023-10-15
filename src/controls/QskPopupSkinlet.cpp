/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskPopupSkinlet.h"
#include "QskPopup.h"

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
    switch ( nodeRole )
    {
        case OverlayRole:
            return updateBoxNode( skinnable, node, QskPopup::Overlay );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

#include "moc_QskPopupSkinlet.cpp"
