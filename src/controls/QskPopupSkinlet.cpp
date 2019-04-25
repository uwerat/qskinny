/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
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
