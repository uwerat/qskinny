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
    setNodeRoles( { PanelRole, TextPanelRole,
        IconRole, ButtonPanelRole, ButtonRole,
        PlaceholderRole, HeaderRole, FooterRole } );
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

        if( !skinnable->symbolHint( Q::Icon ).isEmpty() )
        {
            const auto r = subControlRect( skinnable, contentsRect, Q::Icon );
            if ( !r.isEmpty() )
                rect.setLeft( r.right() );
        }

        if( !skinnable->symbolHint( Q::Button ).isEmpty() )
        {
            const auto r = subControlRect( skinnable, contentsRect, Q::Button );
            if( !r.isEmpty() )
                rect.setRight( r.left() );
        }

        const auto h = skinnable->effectiveFontHeight( Q::Text );
        rect.setTop( rect.center().y() - 0.5 * h );
        rect.setHeight( h );
        rect = rect.marginsAdded( skinnable->marginHint( Q::Text ) );

        return rect;
    }

    if ( subControl == Q::Placeholder )
    {
        const auto textField = static_cast< const QskTextField* >( skinnable );
        if( textField->text().isEmpty() )
            return subControlRect( skinnable, contentsRect, Q::Text );

        return QRectF();
    }

    if ( subControl == Q::Icon )
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

    if ( subControl == Q::ButtonPanel )
    {
        const auto textField = static_cast< const QskTextField* >( skinnable );
        if ( textField->buttonStates() & Q::Hovered )
        {
            const auto r = subControlRect( skinnable, contentsRect, Q::Button );

            QRectF rect( QPointF(), skinnable->strutSizeHint( subControl ) );
            rect.moveCenter( r.center() );

            return rect;
        }

        return QRectF();
    }

    if ( subControl == Q::Button )
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
        case TextPanelRole:
        {
            return updateBoxNode( skinnable, node, Q::TextPanel );
        }
        case PlaceholderRole:
        {
            const auto text = effectivePlaceholderText( textField );
            if ( text.isEmpty() )
                return nullptr;

            const auto subControl = Q::Placeholder;

            QskSkinHintStatus status;

            auto options = skinnable->textOptionsHint( subControl, &status );
            if ( !status.isValid() )
                options.setElideMode( Qt::ElideRight );

            return updateTextNode( skinnable, node,
                textField->subControlRect( subControl ),
                textField->alignmentHint( subControl, Qt::AlignLeft ),
                options, text, subControl );
        }

        case HeaderRole:
        {
            return updateTextNode( skinnable, node,
                textField->headerText(), Q::Header );
        }

        case FooterRole:
        {
           return updateTextNode( skinnable, node,
                textField->footerText(), Q::Footer );
        }

        case IconRole:
            return updateSymbolNode( skinnable, node, Q::Icon );

        case ButtonPanelRole:
            return updateBoxNode( skinnable, node, Q::ButtonPanel );

        case ButtonRole:
            return updateSymbolNode( skinnable, node, Q::Button );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSizeF QskTextFieldSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    Q_UNUSED( constraint ); // TODO ...

    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto input = static_cast< const QskAbstractTextInput* >( skinnable );

    auto hint = input->unwrappedTextSize();
    hint = hint.grownBy( skinnable->marginHint( Q::Text ) );

    hint = input->outerBoxSize( Q::TextPanel, hint );
    hint = hint.expandedTo( input->strutSizeHint( Q::TextPanel ) );

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
