/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSubWindowAreaSkinlet.h"
#include "QskSubWindowArea.h"

QskSubWindowAreaSkinlet::QskSubWindowAreaSkinlet( QskSkin* skin ):
    Inherited( skin )
{
    setNodeRoles( { PanelRole } );
}

QskSubWindowAreaSkinlet::~QskSubWindowAreaSkinlet() = default;

QRectF QskSubWindowAreaSkinlet::subControlRect(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{       
    const auto area = static_cast< const QskSubWindowArea* >( skinnable );
    
    if ( subControl == QskSubWindowArea::Panel )
        return area->contentsRect();
    
    return Inherited::subControlRect( skinnable, subControl );
}

QSGNode* QskSubWindowAreaSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    switch( nodeRole )
    {
        case PanelRole:
            return updateBoxNode( skinnable, node, QskSubWindowArea::Panel );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

#include "moc_QskSubWindowAreaSkinlet.cpp"
