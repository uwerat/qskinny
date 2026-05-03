/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextLabelSkinlet.h"
#include "QskTextLabel.h"

#include "QskTextOptions.h"
#include "QskTextRenderer.h"
#include "QskFunctions.h"

#include <qmath.h>

QskTextLabelSkinlet::QskTextLabelSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, TextRole } );
}

QskTextLabelSkinlet::~QskTextLabelSkinlet() = default;

QRectF QskTextLabelSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskTextLabel::Panel )
    {
        return contentsRect;
    }
    else if ( subControl == QskTextLabel::Text )
    {
        const auto label = static_cast< const QskTextLabel* >( skinnable );

        if ( label->hasPanel() )
        {
            return label->subControlContentsRect(
                contentsRect, QskTextLabel::Panel );
        }

        return contentsRect;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskTextLabelSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto label = static_cast< const QskTextLabel* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            if ( !label->hasPanel() )
                return nullptr;

            return updateBoxNode( label, node, QskTextLabel::Panel );
        }
        case TextRole:
        {
            return updateTextNode( label, node,
                label->text(), QskTextLabel::Text );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSizeF QskTextLabelSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto label = static_cast< const QskTextLabel* >( skinnable );

    const auto font = label->effectiveFont( QskTextLabel::Text );
    auto textOptions = label->textOptions();
    textOptions.setFormat( label->effectiveTextFormat() );

    const auto text = label->text();

    QSizeF hint;

    const qreal lineHeight = qCeil( qskFontHeight( font ) );

    if ( text.isEmpty() )
    {
        if ( constraint.height() < 0.0 )
            hint.setHeight( lineHeight );
    }
    else if ( constraint.width() >= 0.0 )
    {
        if ( textOptions.effectiveElideMode() != Qt::ElideNone )
        {
            // wrong for text with newlines TODO ...
            hint.setHeight( lineHeight );
        }
        else
        {
            auto height = QskTextRenderer::textHeightForWidth(
                text, font, textOptions, constraint.width() );

            QSizeF size( constraint.width(), height );
            if ( label->hasPanel() )
                size = label->outerBoxSize( QskTextLabel::Panel, size );

            hint.setHeight( size.height() );
        }
    }
    else if ( constraint.height() >= 0.0 )
    {
        auto width = QskTextRenderer::textWidthForHeight(
            text, font, textOptions, constraint.height() );

        QSizeF size( width, constraint.height() );
        if ( label->hasPanel() )
            size = label->outerBoxSize( QskTextLabel::Panel, size );

        hint.setWidth( size.width() );
    }
    else
    {
        hint = QskTextRenderer::textSize( text, font, textOptions );

        if ( label->hasPanel() )
            hint = label->outerBoxSize( QskTextLabel::Panel, hint );
    }

    return hint;
}

#include "moc_QskTextLabelSkinlet.cpp"
