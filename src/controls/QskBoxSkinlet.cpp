/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxSkinlet.h"
#include "QskBox.h"

QskBoxSkinlet::QskBoxSkinlet( QskSkin* skin ):
    Inherited( skin )
{
    setNodeRoles( { PanelRole } );
}

QskBoxSkinlet::~QskBoxSkinlet()
{
}

QRectF QskBoxSkinlet::subControlRect(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{      
    if ( subControl == QskBox::Panel )
    {
        const auto box = static_cast< const QskBox* >( skinnable );
        return box->contentsRect();
    }
    
    return Inherited::subControlRect( skinnable, subControl );
}   

QSGNode* QskBoxSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    switch( nodeRole )
    {
        case PanelRole:
            return updateBoxNode( skinnable, node, QskBox::Panel );

        default:
            return nullptr;
    }
}

#include "moc_QskBoxSkinlet.cpp"
