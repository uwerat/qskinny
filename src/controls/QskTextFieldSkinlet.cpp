/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextFieldSkinlet.h"
#include "QskTextField.h"

using Q = QskTextField;

QSK_SYSTEM_STATE( QskTextFieldSkinlet, Selected, QskAspect::FirstSystemState << 3 )

static QString qskEffectivePlaceholderText( const QskTextField* textField )
{
    if ( textField->text().isEmpty() && 
        !( textField->isReadOnly() || textField->isEditing() ) )
    {
        return textField->placeholderText();
    }

    return QString();
}

QskTextFieldSkinlet::QskTextFieldSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, LabelTextRole, PlaceholderTextRole, } );
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
        auto rect = skinnable->subControlContentsRect( contentsRect, Q::Panel );

        if( !skinnable->symbolHint( Q::LeadingIcon ).isEmpty() )
        {
            const auto r = subControlRect( skinnable, contentsRect, Q::LeadingIcon );
            if ( !r.isEmpty() )
                rect.setLeft( r.right() );
        }

        if( !skinnable->symbolHint( Q::TrailingIcon ).isEmpty() )
        {
            const auto r = subControlRect( skinnable, contentsRect, Q::TrailingIcon );
            if( !r.isEmpty() )
                rect.setRight( r.left() );
        }

        return rect.marginsRemoved( skinnable->marginHint( Q::Text ) );
    }
    else if ( subControl == Q::PlaceholderText )
    {
        const auto textField = static_cast< const QskTextField* >( skinnable );
        if( textField->text().isEmpty() )
            return subControlRect( skinnable, contentsRect, Q::Text );

        return QRectF();
    }
    else if ( subControl == Q::LeadingIcon )
    {           
        if( !skinnable->symbolHint( subControl ).isEmpty() )
        {   
            const auto panelRect = skinnable->subControlContentsRect(
                contentsRect, Q::Panel );

            auto rect = panelRect;
        
            rect.setSize( skinnable->strutSizeHint( subControl ) );
            rect.moveCenter( { rect.center().x(), panelRect.center().y() } );
    
            return rect; 
        }
        
        return QRectF();
    }       
    else if ( subControl == Q::TrailingIcon )
    {       
        if( !skinnable->symbolHint( subControl ).isEmpty() )
        {       
            const auto panelRect = skinnable->subControlContentsRect(
                contentsRect, Q::Panel );
            
            auto rect = panelRect;
            
            const auto size = skinnable->strutSizeHint( subControl ); 
            rect.setHeight( size.height() );
            rect.moveCenter( { rect.center().x(), panelRect.center().y() } );
            rect.setLeft( rect.right() - size.width() );
            
            return rect;
        }
        
        return QRectF();
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSizeF QskTextFieldSkinlet::adjustSizeHint(
    const QskSkinnable*, Qt::SizeHint, const QSizeF& hint ) const
{
    return hint;
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
        case LabelTextRole:
        {
            return updateTextNode( skinnable, node,
                textField->labelText(), Q::LabelText );
        }
        case LeadingIconRole:
        {
            return updateSymbolNode( skinnable, node, Q::LeadingIcon );
        }

        case TrailingIconRole:
        {
            return updateSymbolNode( skinnable, node, Q::TrailingIcon );
        }

        case PlaceholderTextRole:
        {
            const auto text = qskEffectivePlaceholderText( textField );
            return updateTextNode( skinnable, node, text, Q::PlaceholderText );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

#include "moc_QskTextFieldSkinlet.cpp"
