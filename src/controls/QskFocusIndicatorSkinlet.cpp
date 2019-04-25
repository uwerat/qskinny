/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskFocusIndicatorSkinlet.h"
#include "QskFocusIndicator.h"

QskFocusIndicatorSkinlet::QskFocusIndicatorSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { FrameRole } );
}

QskFocusIndicatorSkinlet::~QskFocusIndicatorSkinlet() = default;

QRectF QskFocusIndicatorSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskFocusIndicator::Panel )
    {
        return contentsRect;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskFocusIndicatorSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto indicator = static_cast< const QskFocusIndicator* >( skinnable );

    switch ( nodeRole )
    {
        case FrameRole:
        {
            return updateBoxNode( indicator, node, QskFocusIndicator::Panel );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

#include "moc_QskFocusIndicatorSkinlet.cpp"
