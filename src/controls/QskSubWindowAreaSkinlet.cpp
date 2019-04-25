/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSubWindowAreaSkinlet.h"
#include "QskSubWindowArea.h"

QskSubWindowAreaSkinlet::QskSubWindowAreaSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole } );
}

QskSubWindowAreaSkinlet::~QskSubWindowAreaSkinlet() = default;

QRectF QskSubWindowAreaSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskSubWindowArea::Panel )
        return contentsRect;

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskSubWindowAreaSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    switch ( nodeRole )
    {
        case PanelRole:
            return updateBoxNode( skinnable, node, QskSubWindowArea::Panel );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

#include "moc_QskSubWindowAreaSkinlet.cpp"
