/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTextLabelSkinlet.h"
#include "QskTextLabel.h"

#include "QskTextOptions.h"
#include "QskTextRenderer.h"

#include <qfontmetrics.h>
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
            return label->subControlContentsRect( contentsRect, QskTextLabel::Panel );

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
                label->text(), label->textOptions(), QskTextLabel::Text );
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

    const qreal lineHeight = QFontMetricsF( font ).height();

    if ( text.isEmpty() )
    {
        if ( constraint.height() < 0.0 )
            hint.setHeight( qCeil( lineHeight ) );
    }
    else if ( constraint.width() >= 0.0 )
    {
        if ( textOptions.effectiveElideMode() != Qt::ElideNone )
        {
            hint.setHeight( qCeil( lineHeight ) );
        }
        else
        {
            /*
                In case of QskTextOptions::NoWrap we could count
                the line numbers and calculate the height from
                lineHeight. TODO ...
             */
            qreal maxHeight = std::numeric_limits< qreal >::max();
            if ( maxHeight / lineHeight > textOptions.maximumLineCount() )
            {
                // be careful with overflows
                maxHeight = textOptions.maximumLineCount() * lineHeight;
            }

            QSizeF size( constraint.width(), maxHeight );

            size = QskTextRenderer::textSize( text, font, textOptions, size );

            if ( label->hasPanel() )
                size = label->outerBoxSize( QskTextLabel::Panel, size );

            hint.setHeight( qCeil( size.height() ) );
        }
    }
    else if ( constraint.height() >= 0.0 )
    {
        const qreal maxWidth = std::numeric_limits< qreal >::max();

        QSizeF size( maxWidth, constraint.height() );

        size = QskTextRenderer::textSize( text, font, textOptions, size );

        if ( label->hasPanel() )
            size = label->outerBoxSize( QskTextLabel::Panel, size );

        hint.setWidth( qCeil( size.width() ) );
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
