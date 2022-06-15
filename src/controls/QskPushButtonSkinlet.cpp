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

static inline Qt::Orientation qskOrientation( const QskPushButton* button )
{
    // For the moment we only handle the orientation TODO ...

    const auto direction = button->flagHint(
        QskPushButton::Panel | QskAspect::Direction, Qsk::LeftToRight );

    if ( direction == Qsk::LeftToRight || direction == Qsk::RightToLeft )
        return Qt::Horizontal;
    else
        return Qt::Vertical;
}

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
    using Q = QskPushButton;

    auto r = button->subControlContentsRect( contentsRect, Q::Panel );

    if ( button->hasGraphic() )
    {
        if ( qskOrientation( button ) == Qt::Horizontal )
        {
            const auto graphicsRect = subControlRect( button, contentsRect, Q::Graphic );
            const auto spacing = button->spacingHint( Q::Panel );

            r.setX( r.x() + graphicsRect.width() + spacing );
        }
        else
        {
            const qreal h = button->effectiveFontHeight( Q::Text );
            if ( h < r.height() )
                r.setTop( r.bottom() - h );
        }
    }

    return r;
}

QRectF QskPushButtonSkinlet::graphicRect(
    const QskPushButton* button, const QRectF& contentsRect ) const
{
    using Q = QskPushButton;

    auto r = button->subControlContentsRect( contentsRect, Q::Panel );

    const auto orientation = qskOrientation( button );

    if ( !button->text().isEmpty() && orientation == Qt::Vertical )
    {
        const auto textRect = subControlRect( button, contentsRect, Q::Text );
        qreal h = textRect.height() + button->spacingHint( Q::Panel );

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

    const auto sz = button->graphic().defaultSize();

    if ( !( r.isEmpty() || sz.isEmpty() ) )
    {
        // inner rectangle according to the aspect ratio

        const double scaleFactor =
            qMin( r.width() / sz.width(), r.height() / sz.height() );

        const int w = qFloor( scaleFactor * sz.width() );
        const int h = qFloor( scaleFactor * sz.height() );
        int x, y;

        if ( orientation == Qt::Horizontal )
        {
            x = r.left();
            y = r.top();
        }
        else
        {
            // early aligning to avoid pointless operations, that finally will
            // have no effect, when drawing to an integer based paint device

            x = qFloor( r.center().x() - 0.5 * w );
            y = qFloor( r.center().y() - 0.5 * h );
        }

        r = QRectF( x, y, w, h );

        const auto padding = button->paddingHint( Q::Graphic );
        r = r.marginsRemoved( padding );
    }

    return r;
}

QSGNode* QskPushButtonSkinlet::updateTextNode(
    const QskPushButton* button, QSGNode* node ) const
{
    using Q = QskPushButton;

    const auto rect = button->subControlRect( Q::Text ).toAlignedRect();

    const auto textHeight = button->effectiveFontHeight( Q::Text );
    if ( !button->clip() && ( rect.height() < textHeight ) )
        return nullptr;

    const auto alignment = button->alignmentHint( Q::Text, Qt::AlignCenter );

    return QskSkinlet::updateTextNode( button, node, rect, alignment,
        button->text(), button->textOptions(), Q::Text );
}

QSizeF QskPushButtonSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    using Q = QskPushButton;

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

        const auto padding = button->paddingHint( Q::Graphic );
        const auto orientation = qskOrientation( button );

        if( orientation == Qt::Horizontal )
        {
            size.rwidth() += padding.left() + w + padding.right();
            size.rheight() = qMax( size.height(), h );
        }
        else
        {
            size.rheight() += padding.top() + h + padding.bottom();
            size.rwidth() = qMax( size.width(), w );
        }
    }

    size = size.expandedTo( button->strutSizeHint( Q::Panel ) );
    size = button->outerBoxSize( Q::Panel, size );

    return size;
}

#include "moc_QskPushButtonSkinlet.cpp"
