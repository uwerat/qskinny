/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskMaterial3TextFieldSkinlet.h"
#include "QskMaterial3Skin.h"

#include <QskTextField.h>
#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskFunctions.h>

#include <QFontMetricsF>

using Q = QskTextField;

namespace
{
    const int spacingV = 0; // skin hint !

    QString effectiveHeaderText( const QskTextField* textField )
    {
        if ( !textField->isEditing() && textField->text().isEmpty() )
            return QString();

        return textField->headerText();
    }

    inline bool hasCharacterCount( const QskTextField* textField )
    {
        // magic number hardcoded in qquicktextinput.cpp
        return textField->maxLength() < 32767;
    }

    inline bool hasBottomText( const QskTextField* textField )
    {
        return !textField->footerText().isEmpty() || hasCharacterCount( textField );
    }

    QString maxLengthString( const QskTextField* textField )
    {
        QString s = QString::number( textField->text().length() )
            + " / " + QString::number( textField->maxLength() );
        return s;
    }

    // We need to "cut a hole" in the upper gradient for the label text:
    QskBoxBorderColors outlineColors( const QskBoxBorderColors& colors,
        const QRectF& rect, const QRectF& clipRect )
    {
        auto gradient = colors.gradientAt( Qt::TopEdge );

        const auto margin = 6; // ->skin
        auto s1 = ( clipRect.left() - margin - rect.left() ) / rect.width();
        auto s2 = ( clipRect.right() - rect.left() ) / rect.width();

        s1 = qBound( 0.0, s1, 1.0 );
        s2 = qBound( 0.0, s2, 1.0 );

        // not correct, when gradient is not monochrome !!!

        gradient.setStops( {
            { 0.0, gradient.startColor() },
            { s1, gradient.startColor() },
            { s1, Qt::transparent },
            { s2, Qt::transparent },
            { s2, gradient.endColor() },
            { 1.0, gradient.endColor() }
        } );

        auto borderColors = colors;
        borderColors.setGradientAt( Qt::TopEdge, gradient );

        return borderColors;
    }
}

QskMaterial3TextFieldSkinlet::QskMaterial3TextFieldSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    appendNodeRoles( { SupportingTextRole, CharacterCountRole } );
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

        if( textField->style() == QskTextField::OutlinedStyle )
        {
            const auto h = textField->effectiveFontHeight( Q::Header );
            rect.setTop( rect.top() + 0.5 * h );
        }

        if( hasBottomText( textField ) )
        {
            const auto margins = textField->marginHint( Q::Footer );

            const auto h = textField->effectiveFontHeight( Q::Footer )
                + margins.top() + margins.bottom();

            rect.setHeight( rect.height() - h );
        }

        return rect;
    }

    if ( subControl == Q::Text )
    {
        auto rect = Inherited::subControlRect( skinnable, contentsRect, Q::Text );

        if ( !rect.isEmpty() && ( textField->style() == QskTextField::FilledStyle ) )
        {
            const auto text = effectiveHeaderText( textField );
            if ( !text.isEmpty() )
            {
                const auto h = skinnable->effectiveFontHeight( Q::Header );
                rect.translate( 0.0, 0.5 * ( h + spacingV ) );
            }
        }

        return rect;
    }

    if ( subControl == Q::Header )
    {
        const auto text = effectiveHeaderText( textField );
        if( text.isEmpty() )
            return QRectF();

        const QFontMetrics fm( textField->effectiveFont( Q::Header ) );
        const auto textSize = fm.size( Qt::TextSingleLine | Qt::TextExpandTabs, text );

        qreal x, y;

        if ( textField->style() == QskTextField::FilledStyle )
        {
            const auto r = subControlRect( skinnable, contentsRect, Q::Text );

            x = r.left();
            y = r.top() - spacingV - textSize.height();
        }
        else if ( textField->style() == QskTextField::OutlinedStyle )
        {
            const auto r = subControlRect( skinnable, contentsRect, Q::Panel );

            x = r.left() + skinnable->paddingHint( Q::Panel ).left();
            y = r.top() - 0.5 * textSize.height();
        }

        return QRectF( x, y, textSize.width(), textSize.height() );
    }

    if ( subControl == Q::Footer )
    {
        if( !textField->footerText().isEmpty() )
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

    if ( subControl == Q::CharacterCount )
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

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskMaterial3TextFieldSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto textField = static_cast< const Q* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            if( ( textField->style() == QskTextField::OutlinedStyle ) &&
                !effectiveHeaderText( textField ).isEmpty() )
            {
                auto clipRect = textField->subControlRect( Q::Header );
                if ( !clipRect.isEmpty() )
                {
                    const auto subControl = Q::Panel;

                    const auto panelRect = textField->subControlRect( subControl );

                    auto borderColors = textField->boxBorderColorsHint( subControl );
                    borderColors = outlineColors( borderColors, panelRect, clipRect );

                    return updateBoxNode( skinnable, node,
                        panelRect,
                        skinnable->boxShapeHint( subControl ),
                        skinnable->boxBorderMetricsHint( subControl ),
                        borderColors,
                        skinnable->gradientHint( subControl ) );
                }
            }

            return updateBoxNode( skinnable, node, Q::Panel );
        }

        case CharacterCountRole:
        {
            return updateTextNode( skinnable, node,
                maxLengthString( textField ), Q::CharacterCount );
        }

        case HeaderRole:
        {
            return updateTextNode( skinnable, node,
                effectiveHeaderText( textField ), Q::Header );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSizeF QskMaterial3TextFieldSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    auto hint = Inherited::sizeHint( skinnable, which, constraint );
    
    const auto textField = static_cast< const QskTextField* >( skinnable );

    if( textField->style() != QskTextField::PlainStyle )
        hint.rheight() += textField->effectiveFontHeight( Q::Header ) + spacingV;

    if( hasBottomText( textField ) )
    {
        const auto margins = textField->marginHint( Q::Footer );
        hint.rheight() += textField->effectiveFontHeight( Q::Footer )
            + margins.top() + margins.bottom();
    }

    return hint;
}

QString QskMaterial3TextFieldSkinlet::effectivePlaceholderText(
    const QskTextField* textField ) const
{
    if ( textField->text().isEmpty() &&
        !( textField->isReadOnly() || textField->isEditing() ) )
    {
        auto text = textField->placeholderText();
        if ( text.isEmpty() )
            text = textField->headerText();

        return text;
    }

    return QString();
}

#include "moc_QskMaterial3TextFieldSkinlet.cpp"
