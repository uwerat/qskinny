/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextFieldSkinlet.h"
#include "QskTextField.h"

using Q = QskTextField;

QSK_SYSTEM_STATE( QskTextFieldSkinlet, Selected, QskAspect::FirstUserState >> 1 )

QskTextFieldSkinlet::QskTextFieldSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, PlaceholderTextRole, } );
}

QskTextFieldSkinlet::~QskTextFieldSkinlet()
{
}

QRectF QskTextFieldSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == Q::Panel )
    {
        return contentsRect;
    }
    else if ( subControl == Q::Text )
    {
        return skinnable->subControlContentsRect( contentsRect, Q::Panel );
    }
    else if ( subControl == Q::PlaceholderText )
    {
        const auto textField = static_cast< const QskTextField* >( skinnable );
        if( textField->text().isEmpty() )
            return subControlRect( skinnable, contentsRect, Q::Text );

        return QRectF();
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskTextFieldSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto textField = static_cast< const QskTextField* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            if ( !textField->hasPanel() )
                return nullptr;

            return updateBoxNode( skinnable, node, Q::Panel );
        }
        case PlaceholderTextRole:
        {
            return updateTextNode( skinnable, node,
                textField->placeholderText(), Q::PlaceholderText );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

#include "moc_QskTextFieldSkinlet.cpp"
