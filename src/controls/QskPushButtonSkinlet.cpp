/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPushButtonSkinlet.h"
#include "QskPushButton.h"

#include "QskAnimationHint.h"
#include "QskGraphic.h"
#include "QskTextOptions.h"
#include "QskFunctions.h"
#include "QskSGNode.h"

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
    setNodeRoles( { PanelRole, RippleRole, GraphicRole, TextRole } );
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

    if ( subControl == QskPushButton::Graphic )
    {
        return graphicRect( button, contentsRect );
    }

    if ( subControl == QskPushButton::Panel )
    {
        return contentsRect;
    }

    if ( subControl == QskPushButton::Ripple )
    {
        return rippleRect( button, contentsRect );
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

        case RippleRole:
        {
            return updateRippleNode( button, node );
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
    if ( r.isEmpty() || !button->hasGraphic() )
        return r;

    if ( qskOrientation( button ) == Qt::Horizontal )
    {
        /*
            For horizontal layouts Text depends on Graphic, while
            for vertical Graphic depends on Text. Confusing ...
         */
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

    return r;
}

QRectF QskPushButtonSkinlet::graphicRect(
    const QskPushButton* button, const QRectF& contentsRect ) const
{
    using Q = QskPushButton;

    auto r = button->subControlContentsRect( contentsRect, Q::Panel );
    if ( r.isEmpty() || button->text().isEmpty() )
        return r;

    const auto orientation = qskOrientation( button );
    const auto maxSize = button->strutSizeHint( Q::Graphic );

    const qreal maxW = maxSize.width();
    const qreal maxH = maxSize.height();

    if ( orientation == Qt::Vertical )
    {
        /*
            For horizontal layouts Text depends on Graphic, while
            for vertical Graphic depends on Text. Confusing ...
         */
        const auto textRect = subControlRect( button, contentsRect, Q::Text );
        const auto h = textRect.height() + button->spacingHint( Q::Panel );

        if ( h > r.height() )
            return QRectF();

        r.setBottom( r.bottom() - h );

        if ( maxW >= 0 || maxH >= 0 )
        {
            // limiting the size by maxSize

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
    }
    else
    {
        if ( maxW >= 0 || maxH >= 0 )
        {
            if ( maxW >= 0.0 && maxW < r.width() )
            {
                r.setWidth( maxW );
            }

            if ( maxH >= 0.0 && maxH < r.height() )
            {
                r.setY( r.center().y() - 0.5 * maxH );
                r.setHeight( maxH );
            }
        }
    }

    r = r.marginsRemoved( button->paddingHint( Q::Graphic ) );

    if ( !r.isEmpty() )
    {
        auto sz = button->graphic().defaultSize();
        if ( !sz.isEmpty() )
        {
            sz.scale( r.size(), Qt::KeepAspectRatio );
        
            if ( orientation == Qt::Vertical )
                r = qskAlignedRectF( r, sz, Qt::AlignCenter );
            else
                r = qskAlignedRectF( r, sz, Qt::AlignLeft | Qt::AlignVCenter );
        }
    }

    return r;
}

QRectF QskPushButtonSkinlet::rippleRect(
    const QskPushButton* button, const QRectF& contentsRect ) const
{
    using Q = QskPushButton;

    QRectF rect;

    const auto ratio = button->metric( Q::Ripple | QskAspect::Size );
    if ( ratio > 0.0 )
    {
        const auto pos = button->effectiveSkinHint(
            Q::Ripple | QskAspect::Metric | QskAspect::Position ).toPointF();

        const auto panelRect = subControlRect( button, contentsRect, Q::Panel );

        rect.setSize( 2.0 * panelRect.size() * ratio );
        rect.moveCenter( pos );
    }
    
    return rect;
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

QSGNode* QskPushButtonSkinlet::updateRippleNode(
    const QskPushButton* button, QSGNode* node ) const
{
    using Q = QskPushButton;

    const auto rippleRect = button->subControlRect( Q::Ripple );
    if ( rippleRect.isEmpty() )
        return nullptr;

    auto clipNode = updateBoxClipNode( button, node,
        button->subControlRect( Q::Panel ), Q::Panel );

    if ( clipNode )
    {
        auto boxNode = QskSGNode::findChildNode( clipNode, RippleRole );
        boxNode = updateBoxNode( button, boxNode, rippleRect, Q::Ripple );

        if ( boxNode == nullptr )
            return nullptr;

        QskSGNode::setNodeRole( boxNode, RippleRole );
        if ( boxNode->parent() != clipNode )
            clipNode->appendChildNode( boxNode );
    }
        
    return clipNode;
}

QSizeF QskPushButtonSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    using Q = QskPushButton;

    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto button = static_cast< const QskPushButton* >( skinnable );

    QSizeF size( 0, 0 );

    const QFontMetricsF fm( button->effectiveFont( Q::Text ) );

    if ( !button->text().isEmpty() )
    {
        // in elide mode we might want to ignore the text width ???

        size += fm.size( Qt::TextShowMnemonic, button->text() );
    }

    if ( button->hasGraphic() )
    {
        const auto hint = graphicSizeHint( button );

        const auto padding = button->paddingHint( Q::Graphic );
        const auto orientation = qskOrientation( button );

        if( orientation == Qt::Horizontal )
        {
            size.rwidth() += padding.left() + hint.width() + padding.right();
            size.rheight() = qMax( size.height(), hint.height() );
        }
        else
        {
            size.rheight() += padding.top() + hint.height() + padding.bottom();
            size.rwidth() = qMax( size.width(), hint.width() );
        }
    }

    size = size.expandedTo( button->strutSizeHint( Q::Panel ) );
    size = button->outerBoxSize( Q::Panel, size );

    return size;
}

QSizeF QskPushButtonSkinlet::graphicSizeHint( const QskPushButton* button ) const
{
    using Q = QskPushButton;

    auto size = button->strutSizeHint( Q::Graphic );
    if ( !size.isEmpty() )
        return size;

    const auto& graphic = button->graphic();

    auto w = size.width();
    auto h = size.height();

    if ( w > 0.0 )
    {
        h = graphic.heightForWidth( w );
    }
    else
    {
        if ( h <= 0.0 )
            h = 1.5 * button->effectiveFontHeight( Q::Text );

        w = graphic.widthForHeight( h );
    }

    return QSizeF( w, h );
}

#include "moc_QskPushButtonSkinlet.cpp"
