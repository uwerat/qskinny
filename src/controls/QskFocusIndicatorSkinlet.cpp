/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskFocusIndicatorSkinlet.h"
#include "QskFocusIndicator.h"
#include "QskRectNode.h"
#include "QskAspect.h"

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
        return panelRect( indicator );
    }
    
    return Inherited::subControlRect( skinnable, subControl );
}       

QRectF QskFocusIndicatorSkinlet::panelRect( const QskFocusIndicator* indicator ) const
{   
    return indicator->boundingRect();
}

QSGNode* QskFocusIndicatorSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto indicator = static_cast< const QskFocusIndicator* >( skinnable );

    switch( nodeRole )
    {
        case FrameRole:
            return updateFrameNode( indicator, node );

        default:
            return nullptr;
    }
}

QSGNode* QskFocusIndicatorSkinlet::updateFrameNode(
    const QskFocusIndicator* indicator, QSGNode* node ) const
{
    QskAspect::Subcontrol panel = QskFocusIndicator::Panel;

    const QRectF rect = subControlRect( indicator, panel );
    if ( rect.isEmpty() )
        return nullptr;

    // the current implementation of the box renderer is uncapable
    // of drawing frames without background. TODO ...

    const QColor color = indicator->color( panel | QskAspect::Border );
    const qreal width = indicator->metric( panel | QskAspect::Border );

    if ( width <= 0.0 || color.alpha() == 0 )
        return nullptr;

    auto rectNode = static_cast< QskRectNode* >( node );
    if ( rectNode == nullptr )
        rectNode = new QskRectNode;

    rectNode->setRect( rect );
    rectNode->setBorderWidth( width );

    // QskRectNode needs to be improved for supporting Radius(X/Y)
    rectNode->setRadius( indicator->metric( panel | QskAspect::RadiusX ) );

    rectNode->setBorderColor( color );
    rectNode->update();

    return rectNode;
}

#include "moc_QskFocusIndicatorSkinlet.cpp"
