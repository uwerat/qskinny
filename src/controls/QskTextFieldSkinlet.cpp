/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextFieldSkinlet.h"
#include "QskTextField.h"

#include <qfontmetrics.h>

using Q = QskTextField;

QskTextFieldSkinlet::QskTextFieldSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, TextPanelRole, LabelTextRole, PlaceholderTextRole, } );
}

QskTextFieldSkinlet::~QskTextFieldSkinlet()
{
}

QRectF QskTextFieldSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == Q::Panel )
        return contentsRect;

    if ( subControl == Q::TextPanel )
        return skinnable->subControlContentsRect( contentsRect, Q::Panel );

    if ( subControl == Q::Text )
    {
        auto rect = skinnable->subControlContentsRect( contentsRect, Q::TextPanel );

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

        const auto h = skinnable->effectiveFontHeight( Q::Text );
        rect.setTop( rect.center().y() - 0.5 * h );
        rect.setHeight( h );
        rect = rect.marginsAdded( skinnable->marginHint( Q::Text ) );

        return rect;
    }

    if ( subControl == Q::PlaceholderText )
    {
        const auto textField = static_cast< const QskTextField* >( skinnable );
        if( textField->text().isEmpty() )
            return subControlRect( skinnable, contentsRect, Q::Text );

        return QRectF();
    }

    if ( subControl == Q::LeadingIcon )
    {
        if( !skinnable->symbolHint( subControl ).isEmpty() )
        {
            const auto panelRect = skinnable->subControlContentsRect(
                contentsRect, Q::TextPanel );

            auto rect = panelRect;

            rect.setSize( skinnable->strutSizeHint( subControl ) );
            rect.moveCenter( { rect.center().x(), panelRect.center().y() } );

            return rect;
        }

        return QRectF();
    }

    if ( subControl == Q::TrailingIcon )
    {
        if( !skinnable->symbolHint( subControl ).isEmpty() )
        {
            const auto panelRect = skinnable->subControlContentsRect(
                contentsRect, Q::TextPanel );

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

QSGNode* QskTextFieldSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto textField = static_cast< const QskTextField* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            return updateBoxNode( skinnable, node, Q::Panel );
        }

        case PlaceholderTextRole:
        {
            const auto text = effectivePlaceholderText( textField );
            if ( text.isEmpty() )
                return nullptr;

            const auto subControl = Q::PlaceholderText;

            QskSkinHintStatus status;

            auto options = skinnable->textOptionsHint( subControl, &status );
            if ( !status.isValid() )
                options.setElideMode( Qt::ElideRight );

            return updateTextNode( skinnable, node,
                textField->subControlRect( subControl ),
                textField->alignmentHint( subControl, Qt::AlignLeft ),
                options, text, subControl );
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
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSizeF QskTextFieldSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    auto hint = Inherited::sizeHint( skinnable, which, constraint );
    hint = skinnable->outerBoxSize( Q::Panel, hint );
    hint = hint.expandedTo( skinnable->strutSizeHint( Q::Panel ) );

    return hint;
}

QString QskTextFieldSkinlet::effectivePlaceholderText(
    const QskTextField* textField ) const
{
    if ( textField->text().isEmpty() &&
        !( textField->isReadOnly() || textField->isEditing() ) )
    {
        return textField->placeholderText();
    }

    return QString();
}

#include "moc_QskTextFieldSkinlet.cpp"
