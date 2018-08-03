/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTextInputSkinlet.h"
#include "QskTextInput.h"

QskTextInputSkinlet::QskTextInputSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole } );
}

QskTextInputSkinlet::~QskTextInputSkinlet()
{
}

QRectF QskTextInputSkinlet::subControlRect(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskTextInput::Panel )
    {
        const auto textInput = static_cast< const QskTextInput* >( skinnable );
        return textInput->contentsRect();
    }
    else if ( subControl == QskTextInput::Text )
    {
        return skinnable->innerBox( QskTextInput::Panel,
            subControlRect( skinnable, QskTextInput::Panel ) );
    }

    return Inherited::subControlRect( skinnable, subControl );
}

QSGNode* QskTextInputSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    switch ( nodeRole )
    {
        case PanelRole:
        {
            return updateBoxNode( skinnable, node, QskTextInput::Panel );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

#include "moc_QskTextInputSkinlet.cpp"
