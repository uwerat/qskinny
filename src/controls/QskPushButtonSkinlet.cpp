/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPushButtonSkinlet.h"
#include "QskPushButton.h"

#include "QskGraphic.h"
#include "QskTextOptions.h"

#include <qfontmetrics.h>
#include <qmath.h>

QskPushButtonSkinlet::QskPushButtonSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, GraphicRole, TextRole } );
}

QskPushButtonSkinlet::~QskPushButtonSkinlet() = default;

QRectF QskPushButtonSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto button = static_cast< const QskPushButton* >( skinnable );

    if ( subControl == QskPushButton::Text )
    {
        return textRect( button, contentsRect );
    }
    else if ( subControl == QskPushButton::Graphic )
    {
        return graphicRect( button, contentsRect );
    }
    else if ( subControl == QskPushButton::Panel )
    {
        return contentsRect;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskPushButtonSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto button = static_cast< const QskPushButton* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            return updateBoxNode( button, node, QskPushButton::Panel );
        }

        case TextRole:
        {
            return updateTextNode( button, node );
        }

        case GraphicRole:
        {
            return updateGraphicNode( button, node,
                button->graphic(), QskPushButton::Graphic );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRectF QskPushButtonSkinlet::textRect(
    const QskPushButton* button, const QRectF& contentsRect ) const
{
    // buttonShift -> TODO

    auto r = button->subControlContentsRect( contentsRect, QskPushButton::Panel );

    if ( button->hasGraphic() )
    {
        // in case of having text + graphic we put the text at the bottom

        qreal h = QFontMetricsF( button->effectiveFont( QskPushButton::Text ) ).height();
        if ( h < r.height() )
            r.setTop( r.bottom() - h );
    }

    return r;
}

QRectF QskPushButtonSkinlet::graphicRect(
    const QskPushButton* button, const QRectF& contentsRect ) const
{
    // buttonShift -> TODO

    auto r = button->subControlContentsRect( contentsRect, QskPushButton::Panel );

    if ( !button->text().isEmpty() )
    {
        const auto textRect = subControlRect( button, contentsRect, QskPushButton::Text );
        qreal h = textRect.height() + button->spacingHint( QskPushButton::Panel );

        if ( h < r.height() )
            r.setBottom( r.bottom() - h );
        else
            r.setHeight( 0 );
    }

    const auto maxSize = button->graphicSourceSize();
    if ( maxSize.width() >= 0 || maxSize.height() >= 0 )
    {
        // limiting the size by graphicSize
        const qreal maxW = maxSize.width();
        const qreal maxH = maxSize.height();

        if ( maxW >= 0.0 && maxW < r.width() )
        {
            r.setX( r.center().x() - 0.5 * maxW );
            r.setWidth( maxW );
        }

        if ( maxH >= 0.0 && maxH < r.height() )
        {
            r.setY( r.center().y() - 0.5 * maxH );
            r.setHeight( maxH );
        }
    }

    const QSizeF sz = button->graphic().defaultSize();

    if ( !( r.isEmpty() || sz.isEmpty() ) )
    {
        // inner rectangle according to the aspect ratio

        const double scaleFactor =
            qMin( r.width() / sz.width(), r.height() / sz.height() );

        // early aligning to avoid pointless operations, that finally will
        // have no effect, when drawing to an integer based paint device

        const int w = qFloor( scaleFactor * sz.width() );
        const int h = qFloor( scaleFactor * sz.height() );
        const int x = qFloor( r.center().x() - 0.5 * w );
        const int y = qFloor( r.center().y() - 0.5 * h );

        r = QRectF( x, y, w, h );
    }

    return r;
}

QSGNode* QskPushButtonSkinlet::updateTextNode(
    const QskPushButton* button, QSGNode* node ) const
{
    const auto rect = button->subControlRect( QskPushButton::Text ).toAlignedRect();

    const QFontMetricsF fm( button->effectiveFont( QskPushButton::Text ) );
    if ( !button->clip() && ( rect.height() < fm.height() ) )
        return nullptr;

    const auto alignment = button->alignmentHint( QskPushButton::Text, Qt::AlignCenter );

    return QskSkinlet::updateTextNode( button, node, rect, alignment,
        button->text(), button->textOptions(), QskPushButton::Text );
}

QSizeF QskPushButtonSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto button = static_cast< const QskPushButton* >( skinnable );

    QSizeF size( 0, 0 );

    const QFontMetricsF fm( button->font() );

    if ( !button->text().isEmpty() )
    {
        // in elide mode we might want to ignore the text width ???

        size += fm.size( Qt::TextShowMnemonic, button->text() );
    }

    if ( button->hasGraphic() )
    {
        const auto sz = button->graphicSourceSize();

        qreal w = sz.width();
        qreal h = sz.height();

        if ( w < 0.0 || h < 0.0 )
        {
            const auto graphic = button->graphic();

            if ( !graphic.isEmpty() )
            {

                if ( ( w < 0.0 ) && ( h < 0.0 ) )
                {
                    // strutSizeHint( Graphic ) ???
                    h = 1.5 * fm.height();
                }

                if ( w < 0 )
                {
                    w = graphic.widthForHeight( h );
                }
                else if ( h < 0 )
                {
                    h = graphic.heightForWidth( w );
                }
            }
        }

        const qreal padding = 2.0; // paddingHint( Graphic ) ???

        size.rheight() += 2 * padding + h;
        size.rwidth() = qMax( size.width(), w );
    }

    size = size.expandedTo( button->strutSizeHint( QskPushButton::Panel ) );
    size = button->outerBoxSize( QskPushButton::Panel, size );

    return size;
}

#include "moc_QskPushButtonSkinlet.cpp"
