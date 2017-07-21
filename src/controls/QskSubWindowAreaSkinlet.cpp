/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSubWindowAreaSkinlet.h"
#include "QskSubWindowArea.h"
#include "QskSkin.h"
#include "QskGradient.h"
#include "QskRectNode.h"

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
    {
        return panelRect( area );
    }
    
    return Inherited::subControlRect( skinnable, subControl );
}

QRectF QskSubWindowAreaSkinlet::panelRect( const QskSubWindowArea* area ) const
{
    return area->contentsRect();
}

QSGNode* QskSubWindowAreaSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto area = static_cast< const QskSubWindowArea* >( skinnable );

    switch( nodeRole )
    {
        case PanelRole:
            return updatePanelNode( area, node );

        default:
            return nullptr;
    }
}

QSGNode* QskSubWindowAreaSkinlet::updatePanelNode(
    const QskSubWindowArea* area, QSGNode* node ) const
{
    const QRectF rect = subControlRect( area, QskSubWindowArea::Panel );

    if ( !area->gradient().isValid() || rect.isEmpty() )
        return nullptr;

    QskRectNode* rectNode = static_cast< QskRectNode* >( node );
    if ( rectNode == nullptr )
        rectNode = new QskRectNode();

    rectNode->setRect( rect );
    rectNode->setFillGradient( area->gradient() );
    rectNode->update();

    return rectNode;
}

#include "moc_QskSubWindowAreaSkinlet.cpp"
