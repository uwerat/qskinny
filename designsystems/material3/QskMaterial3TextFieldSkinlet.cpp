/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskMaterial3TextFieldSkinlet.h"
#include "QskMaterial3Skin.h"

#include <QskTextField.h>
#include <QskBoxBorderColors.h>
#include <QskBoxHints.h>
#include <QskFunctions.h>

#include <QFontMetricsF>

using Q = QskTextField;

static inline bool isOutlined( const QskTextField* textField )
{
    return textField->effectiveVariation() == QskAspect::NoVariation;
}

namespace
{
    const int spacingV = 0; // skin hint !

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
    appendNodeRoles( { CharacterCountRole } );
}

QskMaterial3TextFieldSkinlet::~QskMaterial3TextFieldSkinlet()
{
}

QRectF QskMaterial3TextFieldSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto textField = static_cast< const Q* >( skinnable );

    if ( subControl == Q::Header )
    {
        const auto text = effectiveText( textField, Q::Header );
        if( text.isEmpty() )
            return QRectF();

        const QFontMetrics fm( textField->effectiveFont( Q::Header ) );
        const auto textSize = fm.size( Qt::TextSingleLine | Qt::TextExpandTabs, text );

        if ( isOutlined( textField ) )
        {
            const auto r = subControlRect( skinnable, contentsRect, Q::TextPanel );

            const auto x = r.left() + skinnable->paddingHint( Q::TextPanel ).left();
            const auto y = r.top() - 0.5 * textSize.height();

            return QRectF( x, y, textSize.width(), textSize.height() );
        }
        else
        {
            const auto r = subControlRect( skinnable, contentsRect, Q::Text );

            return QRectF( r.x(), r.top() - textSize.height(),
                textSize.width(), textSize.height() );
        }
    }

    if ( subControl == Q::TextPanel )
    {
        auto rect = skinnable->subControlRect( contentsRect, Q::Panel );

        if ( isOutlined( textField ) )
        {
            const QFontMetrics fm( textField->effectiveFont( Q::Header ) );
            rect.setTop( rect.top() + 0.5 * fm.height() );
        }

        const auto h = skinnable->strutSizeHint( Q::TextPanel ).height();
        rect.setHeight( h );

        return rect;
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
        case TextPanelRole:
        {
            const auto rect = textField->subControlRect( Q::TextPanel );

            auto hints = textField->boxHints( Q::TextPanel );

            if ( isOutlined( textField ) )
            {
                const auto clipRect = textField->subControlRect( Q::Header );
                if ( !clipRect.isEmpty() )
                {
                    hints.borderColors = outlineColors(
                        hints.borderColors, rect, clipRect );
                }
            }

            return updateBoxNode( skinnable, node, rect, hints );
        }

        case CharacterCountRole:
        {
            return updateTextNode( skinnable, node,
                maxLengthString( textField ), Q::CharacterCount );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSizeF QskMaterial3TextFieldSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    Q_UNUSED( constraint ); // TODO ...

    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto textField = static_cast< const QskTextField* >( skinnable );

    auto hint = textField->unwrappedTextSize();
    hint = hint.expandedTo( skinnable->strutSizeHint( Q::TextPanel ) );

    if ( isOutlined( textField ) )
    {
        const QFontMetrics fm( textField->effectiveFont( Q::Header ) );
        hint.rheight() += 0.5 * fm.height();
    }

    hint.rheight() += effectiveFooterHeight( textField );
    hint = hint.expandedTo( skinnable->strutSizeHint( Q::Panel ) );

    return hint;
}

QString QskMaterial3TextFieldSkinlet::effectiveText(
    const QskTextField* textField, QskAspect::Subcontrol subControl ) const
{
    const bool showHeaderAsPlaceholder =
        textField->text().isEmpty() && !textField->isEditing();

    if ( subControl == Q::Header )
    {
        if ( showHeaderAsPlaceholder )
            return QString();
    }
    else if ( subControl == Q::Placeholder )
    {
        if ( showHeaderAsPlaceholder )
        {
            auto text = Inherited::effectiveText( textField, Q::Placeholder );
            if ( text.isEmpty() )
                text = Inherited::effectiveText( textField, Q::Header );

            return text;
        }

        return QString();
    }

    return Inherited::effectiveText( textField, subControl );
}

qreal QskMaterial3TextFieldSkinlet::effectiveFooterHeight(
    const QskTextField* textField ) const
{
    if ( hasCharacterCount( textField ) )
    {
        const auto h = textField->effectiveFontHeight( Q::Footer );
        const auto margins = textField->marginHint( Q::Footer );

        return h + margins.top() + margins.bottom();
    }

    return Inherited::effectiveFooterHeight( textField );
}

#include "moc_QskMaterial3TextFieldSkinlet.cpp"
