/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextInputSkinlet.h"
#include "QskTextInput.h"

#include "QskFunctions.h"

#include <QFontMetricsF>

using Q = QskTextInput;

namespace
{
    QString maxLengthString( const QskTextInput* input )
    {
        QString s = QString::number( input->inputText().length() )
                    + " / " + QString::number( input->maxLength() );
        return s;
    }
}

QskTextInputSkinlet::QskTextInputSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( {
        PanelRole,
        LeadingIconRole,
        LabelTextRole,
        HintTextRole,
        SupportingTextRole,
        CharacterCountRole,
        TrailingIconRippleRole,
        TrailingIconRole,
    } );
}

QskTextInputSkinlet::~QskTextInputSkinlet()
{
}

QRectF QskTextInputSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto input = static_cast< const Q* >( skinnable );

    if ( subControl == Q::Panel )
    {
        auto rect = contentsRect;

        const auto h = input->strutSizeHint( subControl ).height();
        rect.setHeight( h );

        return rect;
    }
    else if ( subControl == Q::LeadingIcon )
    {
        if( input->symbolHint( subControl ).isEmpty() )
        {
            return {};
        }
        else
        {
            const auto margins = input->marginHint( subControl );
            const auto panelRect = input->subControlRect( Q::Panel );
            auto rect = panelRect.marginsRemoved( margins );

            const auto size = input->strutSizeHint( subControl );
            rect.setSize( size );
            rect.moveCenter( { rect.center().x(), panelRect.center().y() } );

            return rect;
        }
    }
    else if ( subControl == Q::LabelText )
    {
        const auto inputRect = input->subControlRect( Q::InputText );

        if( input->hasSkinState( Q::Focused ) || !input->inputText().isEmpty() )
        {
            const auto margins = input->marginHint( subControl );
            auto rect = inputRect;
            rect.setY( contentsRect.y() + margins.top() );
            const QFontMetricsF fm ( input->effectiveFont( subControl ) );
            rect.setHeight( fm.height() );
            return rect;
        }
        else
        {
            return inputRect;
        }
    }
    else if ( subControl == Q::InputText )
    {
        const auto margins = input->marginHint( subControl );

        const auto leadingIconRect = input->subControlRect( Q::LeadingIcon );
        const auto panelRect = input->subControlRect( Q::Panel );
        auto rect = panelRect;
        rect.setLeft( leadingIconRect.right() );
        rect.setRight( contentsRect.right() ); // ### space for trailing icon
        rect = rect.marginsRemoved( margins );

        return rect;
    }
    else if ( subControl == Q::HintText )
    {
        if( input->hasSkinState( Q::Focused ) && input->inputText().isEmpty() ) // ### has TextEmpty state
        {
            return input->subControlRect( Q::InputText );
        }
        else
        {
            return {};
        }
    }
    else if ( subControl == Q::SupportingText )
    {
        if( input->supportingText().isEmpty() )
        {
            return {};
        }
        else
        {
            auto rect = contentsRect;

            const auto margins = input->marginHint( subControl );
            const auto h = margins.top() + input->effectiveFontHeight( subControl ) + margins.bottom();
            rect.setTop( rect.bottom() - h );

            rect.setLeft( rect.left() + margins.left() );

            return rect;
        }
    }
    else if ( subControl == Q::CharacterCount )
    {
        if( input->maxLength() == 32767 ) // magic number hardcoded in qquicktextinput.cpp
        {
            return {};
        }
        else
        {
            auto rect = contentsRect;

            const auto margins = input->marginHint( subControl );
            const auto h = margins.top() + input->effectiveFontHeight( subControl ) + margins.bottom();
            rect.setTop( rect.bottom() - h );

            const QFontMetricsF fm( input->effectiveFont( subControl ) );
            const auto w = qskHorizontalAdvance( fm, maxLengthString( input ) );
            rect.setRight( rect.right() - margins.right() );
            rect.setLeft( rect.right() - ( margins.left() + w + margins.right() ) );

            return rect;
        }
    }
    else if ( subControl == Q::TrailingIconRipple )
    {
        const auto cursorPos = input->effectiveSkinHint(
            Q::TrailingIconRipple | Q::Hovered | QskAspect::Metric | QskAspect::Position ).toPointF();
        const auto trailingIconRect = input->subControlRect( Q::TrailingIcon );

        if( !cursorPos.isNull() && trailingIconRect.contains( cursorPos ) )
        {
            const auto size = input->strutSizeHint( subControl );
            QRectF rect( { 0, 0 }, size );

            rect.moveCenter( trailingIconRect.center() );

            return rect;
        }
        else
        {
            return {};
        }
    }
    else if ( subControl == Q::TrailingIcon )
    {
        if( input->symbolHint( subControl ).isEmpty() )
        {
            return {};
        }
        else
        {
            const auto margins = input->marginHint( subControl );
            const auto panelRect = input->subControlRect( Q::Panel );
            auto rect = panelRect.marginsRemoved( margins );

            const auto size = input->strutSizeHint( subControl );
            rect.setHeight( size.height() );
            rect.moveCenter( { rect.center().x(), panelRect.center().y() } );
            rect.setLeft( rect.right() - size.width() );

            return rect;
        }
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskTextInputSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto input = static_cast< const Q* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            if ( !input->hasPanel() )
                return nullptr;

            return updateBoxNode( skinnable, node, Q::Panel );
        }

        case LeadingIconRole:
        {
            return updateSymbolNode( skinnable, node, Q::LeadingIcon );
        }

        case LabelTextRole:
        {
            return updateTextNode( skinnable, node, input->labelText(), Q::LabelText );
        }

        case HintTextRole:
        {
            return updateTextNode( skinnable, node, input->hintText(), Q::HintText );
        }

        case SupportingTextRole:
        {
            return updateTextNode( skinnable, node, input->supportingText(), Q::SupportingText );
        }

        case CharacterCountRole:
        {
            return updateTextNode( skinnable, node, maxLengthString( input ), Q::CharacterCount );
        }

        case TrailingIconRippleRole:
        {
            return updateBoxNode( skinnable, node, Q::TrailingIconRipple );
        }

        case TrailingIconRole:
        {
            return updateSymbolNode( skinnable, node, Q::TrailingIcon );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

#include "moc_QskTextInputSkinlet.cpp"
