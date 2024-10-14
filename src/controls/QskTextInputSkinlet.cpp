/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextInputSkinlet.h"
#include "QskTextInput.h"

#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxShapeMetrics.h"
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

    // We need to "cut a hole" in the upper gradient for the label text:
    QskBoxBorderColors outlineColors( const QskTextInput* input )
    {
        auto borderColors = input->boxBorderColorsHint( Q::Panel );
        auto topGradient = borderColors.gradientAt( Qt::TopEdge );

        const auto panelRect = input->subControlRect( Q::Panel );

        const auto margins = input->marginHint( Q::LabelText );
        const auto iconMargins = input->marginHint( Q::LeadingIcon );

        const auto x1 = iconMargins.left() - margins.left();
        const auto r1 = x1 / panelRect.width();

        const auto w = qskHorizontalAdvance( input->effectiveFont( Q::LabelText ), input->labelText() );

        const auto x2 = x1 + w + margins.right();
        const auto r2 = x2 / panelRect.width();

        topGradient.setStops( {
            { 0.0, topGradient.startColor() },
            { r1, topGradient.startColor() },
            { r1, Qt::transparent },
            { r2, Qt::transparent },
            { r2, topGradient.startColor() },
            { 1.0, topGradient.startColor() }
        } );

        borderColors.setGradientAt( Qt::TopEdge, topGradient );

        return borderColors;
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

        if( input->emphasis() == Q::LowEmphasis )
        {
            const auto fontHeight = input->effectiveFontHeight( Q::LabelText | Q::Focused );
            rect.setY( fontHeight / 2 );
        }


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

        if( !input->inputText().isEmpty()
             || input->hasSkinState( Q::Focused )
             || input->hasSkinState( Q::Editing ) )
        {
            const auto margins = input->marginHint( subControl );
            auto rect = inputRect;
            const QFontMetricsF fm( input->effectiveFont( subControl ) );

            if( input->emphasis() == Q::LowEmphasis )
            {
                const auto iconMargins = input->marginHint( Q::LeadingIcon );
                rect.setX( iconMargins.left() );
                rect.setY( 0 );
            }
            else
            {
                rect.setY( contentsRect.y() + margins.top() );
            }

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

        const auto trailingIconRect = input->subControlRect( Q::TrailingIcon );

        if( trailingIconRect.isValid() )
        {
            rect.setRight( trailingIconRect.left() );
        }
        else
        {
            rect.setRight( contentsRect.right() );
        }

        rect = rect.marginsRemoved( margins );

        return rect;
    }
    else if ( subControl == Q::HintText )
    {
        if( !input->hasSkinState( Q::TextPopulated )
            && ( input->hasSkinState( Q::Focused ) || input->hasSkinState( Q::Editing ) ) )
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

            if( input->emphasis() == Q::LowEmphasis
                 && ( input->hasSkinState( Q::TextPopulated )
                    || input->hasSkinState( Q::Focused )
                    || input->hasSkinState( Q::Editing ) ) )
            {
                const auto shape = skinnable->boxShapeHint( Q::Panel );
                const auto borderMetrics = skinnable->boxBorderMetricsHint( Q::Panel );
                const auto borderColors = outlineColors( input );
                const auto gradient = input->gradientHint( Q::Panel );

                return updateBoxNode( skinnable, node, input->subControlRect( Q::Panel ),
                    shape, borderMetrics, borderColors, gradient );
            }
            else
            {
                return updateBoxNode( skinnable, node, Q::Panel );
            }
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
