/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPopupSkinlet.h"
#include "QskPopup.h"
#include "QskBoxNode.h"
#include "QskBoxOptions.h"

QskPopupSkinlet::QskPopupSkinlet( QskSkin* skin ):
    Inherited( skin )
{
    appendNodeRoles( { OverlayRole } );
}

QskPopupSkinlet::~QskPopupSkinlet() = default;

QRectF QskPopupSkinlet::subControlRect(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    const auto popup = static_cast< const QskPopup* >( skinnable );

    if ( subControl == QskPopup::Overlay )
        return popup->overlayRect();

    return Inherited::subControlRect( skinnable, subControl );
}

QSGNode* QskPopupSkinlet::updateSubNode( const QskSkinnable* skinnable,
    quint8 nodeRole, QSGNode* node ) const
{
    const auto popup = static_cast< const QskPopup* >( skinnable );

    switch( nodeRole )
    {
        case OverlayRole:
        {
            return updateOverlayNode( popup, node );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskPopupSkinlet::updateOverlayNode(
    const QskPopup* popup, QSGNode* node ) const
{
    const QRectF rect = subControlRect( popup, QskPopup::Overlay );
    if ( rect.isEmpty() )
        return nullptr;

    const QskGradient gradient = popup->gradientHint( QskPopup::Overlay );
    if ( !gradient.isValid() )
        return nullptr;

    auto rectNode = static_cast< QskBoxNode* >( node );
    if ( rectNode == nullptr )
        rectNode = new QskBoxNode();

    QskBoxOptions options;
    options.fillGradient = gradient;
    rectNode->setBoxData( rect, options );

    return rectNode;
}

#include "moc_QskPopupSkinlet.cpp"
