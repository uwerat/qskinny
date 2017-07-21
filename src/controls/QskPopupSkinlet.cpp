/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPopupSkinlet.h"
#include "QskPopup.h"
#include "QskRectNode.h"

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
            return updateOverlayNode( popup, node );

        default:
            return Inherited::updateSubNode( skinnable, nodeRole, node );
    }
}

QSGNode* QskPopupSkinlet::updateOverlayNode(
    const QskPopup* popup, QSGNode* node ) const
{
    const QRectF rect = subControlRect( popup, QskPopup::Overlay );
    if ( rect.isEmpty() )
        return nullptr;

    const QskGradient gradient = overlayGradient( popup );
    if ( !gradient.isValid() )
        return nullptr;

    auto rectNode = static_cast< QskRectNode* >( node );
    if ( rectNode == nullptr )
        rectNode = new QskRectNode();

    rectNode->setRect( rect );
    rectNode->setFillGradient( gradient );
    rectNode->update();

    return rectNode;
}

QskGradient QskPopupSkinlet::overlayGradient( const QskPopup* popup ) const
{
    using namespace QskAspect;

    const QColor color1 = popup->color( QskPopup::Overlay | TopEdge );
    const QColor color2 = popup->color( QskPopup::Overlay | BottomEdge );

    if ( color1.isValid() && color2.isValid() )
    {
        if ( color1.alpha() == 0 && color2.alpha() == 0 )
            return QskGradient();

        return QskGradient( QskGradient::Vertical, color1, color2 );
    }

    const QColor color = popup->color( QskPopup::Overlay );
    if ( color.isValid() && color.alpha() > 0 )
        QskGradient( color );

    return QskGradient(); // something invalid
}

#include "moc_QskPopupSkinlet.cpp"
