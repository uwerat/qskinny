/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskFocusIndicatorSkinlet.h"
#include "QskFocusIndicator.h"

QskFocusIndicatorSkinlet::QskFocusIndicatorSkinlet( QskSkin* skin ):
    Inherited( skin )
{
    setNodeRoles( { FrameRole } );
}

QskFocusIndicatorSkinlet::~QskFocusIndicatorSkinlet() = default;

QRectF QskFocusIndicatorSkinlet::subControlRect(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{       
    const auto indicator = static_cast< const QskFocusIndicator* >( skinnable );
    
    if ( subControl == QskFocusIndicator::Panel )
    {
        return indicator->rect();
    }
    
    return Inherited::subControlRect( skinnable, subControl );
}       

QSGNode* QskFocusIndicatorSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto indicator = static_cast< const QskFocusIndicator* >( skinnable );

    switch( nodeRole )
    {
        case FrameRole:
        {
            return updateBoxNode( indicator, node, QskFocusIndicator::Panel );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

#include "moc_QskFocusIndicatorSkinlet.cpp"
