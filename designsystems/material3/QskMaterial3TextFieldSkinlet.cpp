/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskMaterial3TextFieldSkinlet.h"
#include "QskTextField.h"

#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxShapeMetrics.h"
#include "QskFunctions.h"

#include <QFontMetricsF>

using Q = QskTextField;

namespace
{
    inline bool hasCharacterCount( const QskTextField* textField )
    {
        // magic number hardcoded in qquicktextinput.cpp
        return textField->maxLength() < 32767;
    }

    QString maxLengthString( const QskTextField* textField )
    {
        QString s = QString::number( textField->text().length() )
            + " / " + QString::number( textField->maxLength() );
        return s;
    }

    // We need to "cut a hole" in the upper gradient for the label text:
    QskBoxBorderColors outlineColors( const QskTextField* textField )
    {
        auto borderColors = textField->boxBorderColorsHint( Q::Panel );

        if( textField->labelText().isEmpty() )
        {
            return borderColors;
        }

        auto topGradient = borderColors.gradientAt( Qt::TopEdge );

        const auto panelRect = textField->subControlRect( Q::Panel );

        const auto margins = textField->marginHint( Q::LabelText );
        const auto iconMargins = textField->marginHint( Q::LeadingIcon );

        const auto x1 = iconMargins.left() - margins.left();
        auto r1 = x1 / panelRect.width();
        r1 = qBound( 0.0, r1, 1.0 );

        const auto w = qskHorizontalAdvance(
            textField->effectiveFont( Q::LabelText ), textField->labelText() );

        const auto x2 = x1 + w + margins.right();
        auto r2 = x2 / panelRect.width();
        r2 = qBound( 0.0, r2, 1.0 );

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

QskMaterial3TextFieldSkinlet::QskMaterial3TextFieldSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    appendNodeRoles( {
        LeadingIconRole,
        SupportingTextRole,
        CharacterCountRole,
        TrailingIconRippleRole,
        TrailingIconRole,
    } );
}

QskMaterial3TextFieldSkinlet::~QskMaterial3TextFieldSkinlet()
{
}

QRectF QskMaterial3TextFieldSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto textField = static_cast< const Q* >( skinnable );

    if ( subControl == Q::Panel )
    {
        auto rect = contentsRect;

        if( textField->emphasis() == Q::LowEmphasis )
        {
            const auto h = textField->effectiveFontHeight( Q::LabelText | Q::Focused );
            rect.setY( h / 2 );
        }

        rect.setHeight( textField->strutSizeHint( subControl ).height() );

        return rect;
    }
    else if ( subControl == Q::LabelText )
    {
        if( !textField->text().isEmpty()
             || textField->hasSkinState( Q::Focused )
             || textField->hasSkinState( Q::Editing ) )
        {
            auto rect = textField->subControlRect( Q::Text );

            if( textField->emphasis() == Q::LowEmphasis )
            {
                const auto iconMargins = textField->marginHint( Q::LeadingIcon );
                rect.setX( iconMargins.left() );
                rect.setY( 0 );
            }
            else
            {
                const auto margins = textField->marginHint( subControl );
                rect.setY( contentsRect.y() + margins.top() );
            }

            rect.setHeight( textField->effectiveFontHeight( subControl ) );

            return rect;
        }

        return textField->subControlRect( Q::Text );
    }
    else if ( subControl == Q::LeadingIcon )
    {
        if( !textField->symbolHint( subControl ).isEmpty() )
        {
            const auto margins = textField->marginHint( subControl );
            const auto panelRect = textField->subControlRect( Q::Panel );
            auto rect = panelRect.marginsRemoved( margins );

            const auto size = textField->strutSizeHint( subControl );
            rect.setSize( size );
            rect.moveCenter( { rect.center().x(), panelRect.center().y() } );

            return rect;
        }

        return QRectF();
    }
    else if ( subControl == Q::PlaceholderText )
    {
        if ( skinnable->skinStates() & ( Q::Focused | Q::Editing ) )
        {
            return Inherited::subControlRect( skinnable,
                contentsRect, subControl );
        }

        return QRectF();
    }
    else if ( subControl == Q::SupportingText )
    {
        if( !textField->supportingText().isEmpty() )
        {
            auto rect = contentsRect;

            const auto margins = textField->marginHint( subControl );
            const auto h = textField->effectiveFontHeight( subControl )
                + margins.top() + margins.bottom();

            rect.setTop( rect.bottom() - h );
            rect.setLeft( rect.left() + margins.left() );

            return rect;
        }

        return QRectF();
    }
    else if ( subControl == Q::CharacterCount )
    {
        if( hasCharacterCount( textField ) )
        {
            auto rect = contentsRect;

            const auto margins = textField->marginHint( subControl );
            const auto h = textField->effectiveFontHeight( subControl )
                + margins.top() + margins.bottom();

            rect.setTop( rect.bottom() - h );

            const QFontMetricsF fm( textField->effectiveFont( subControl ) );
            const auto w = qskHorizontalAdvance( fm, maxLengthString( textField ) );
            rect.setRight( rect.right() - margins.right() );
            rect.setLeft( rect.right() - ( margins.left() + w + margins.right() ) );

            return rect;
        }

        return QRectF();
    }
    else if ( subControl == Q::TrailingIconRipple )
    {
        const auto cursorPos = textField->effectiveSkinHint(
            Q::TrailingIconRipple | Q::Hovered | QskAspect::Metric | QskAspect::Position ).toPointF();

        if ( !cursorPos.isNull() )
        {
            const auto r = textField->subControlRect( Q::TrailingIcon );

            if( !cursorPos.isNull() && r.contains( cursorPos ) )
            {
                QRectF rect( QPointF(), textField->strutSizeHint( subControl ) );
                rect.moveCenter( r.center() );

                return rect;
            }
        }

        return QRectF();
    }
    else if ( subControl == Q::TrailingIcon )
    {
        if( !textField->symbolHint( subControl ).isEmpty() )
        {
            const auto margins = textField->marginHint( subControl );
            const auto panelRect = textField->subControlRect( Q::Panel );
            auto rect = panelRect.marginsRemoved( margins );

            const auto size = textField->strutSizeHint( subControl );
            rect.setHeight( size.height() );
            rect.moveCenter( { rect.center().x(), panelRect.center().y() } );
            rect.setLeft( rect.right() - size.width() );

            return rect;
        }

        return QRectF();
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSizeF QskMaterial3TextFieldSkinlet::adjustSizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& oldHint ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    auto hint = oldHint;

    const auto textField = static_cast< const Q* >( skinnable );

    if( textField->emphasis() == Q::LowEmphasis )
    {
        const auto fontHeight = textField->effectiveFontHeight( Q::LabelText | Q::Focused );
        hint.rheight() += fontHeight / 2;
    }

    if( !textField->supportingText().isEmpty() || hasCharacterCount( textField ) )
    {
        const auto margins = textField->marginHint( Q::SupportingText );
        hint.rheight() += textField->effectiveFontHeight( Q::SupportingText )
            + margins.top() + margins.bottom();
    }

    return hint;
}

QSGNode* QskMaterial3TextFieldSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto textField = static_cast< const Q* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            if ( !textField->hasPanel() )
                return nullptr;

            if( textField->emphasis() == Q::LowEmphasis
                 && ( textField->hasSkinState( Q::TextPopulated )
                    || textField->hasSkinState( Q::Focused )
                    || textField->hasSkinState( Q::Editing ) ) )
            {
                const auto shape = skinnable->boxShapeHint( Q::Panel );
                const auto borderMetrics = skinnable->boxBorderMetricsHint( Q::Panel );
                const auto borderColors = outlineColors( textField );
                const auto gradient = textField->gradientHint( Q::Panel );

                return updateBoxNode( skinnable, node, textField->subControlRect( Q::Panel ),
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

        case SupportingTextRole:
        {
            return updateTextNode( skinnable, node,
                textField->supportingText(), Q::SupportingText );
        }

        case CharacterCountRole:
        {
            return updateTextNode( skinnable, node,
                maxLengthString( textField ), Q::CharacterCount );
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

#include "moc_QskMaterial3TextFieldSkinlet.cpp"
