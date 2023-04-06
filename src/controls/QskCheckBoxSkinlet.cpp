/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskCheckBoxSkinlet.h"
#include "QskCheckBox.h"
#include "QskTextOptions.h"
#include "QskFunctions.h"

QskCheckBoxSkinlet::QskCheckBoxSkinlet( QskSkin* skin )
    : QskSkinlet( skin )
{
    setNodeRoles( { BoxRole, IndicatorRole, TextRole, RippleRole } );
}

QskCheckBoxSkinlet::~QskCheckBoxSkinlet()
{
}

QRectF QskCheckBoxSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    using Q = QskCheckBox;

    const auto checkBox = static_cast< const QskCheckBox* >( skinnable );

    if ( subControl == Q::Panel )
        return contentsRect;

    if ( subControl == Q::Box )
        return boxRect( checkBox, contentsRect );

    if ( subControl == Q::Indicator )
    {
        const auto boxRect = subControlRect( skinnable, contentsRect, Q::Box );
        return skinnable->innerBox( Q::Box, boxRect );
    }

    if ( subControl == Q::Text )
        return textRect( checkBox, contentsRect );

    if ( subControl == Q::Ripple )
        return rippleRect( checkBox, contentsRect );

    return contentsRect;
}

QRectF QskCheckBoxSkinlet::textRect(
    const QskCheckBox* checkBox, const QRectF& contentsRect ) const
{
    using Q = QskCheckBox;

    const auto boxRect = subControlRect( checkBox, contentsRect, Q::Box );
    const qreal spacing = checkBox->spacingHint( Q::Panel );

    auto r = subControlRect( checkBox, contentsRect, Q::Panel );
    r = checkBox->innerBox( Q::Panel, r );

    if ( checkBox->layoutMirroring() )
        r.setRight( boxRect.left() - spacing );
    else
        r.setLeft( boxRect.right() + spacing  );

    return r;
}

QRectF QskCheckBoxSkinlet::boxRect(
    const QskCheckBox* checkBox, const QRectF& contentsRect ) const
{
    const auto size = checkBox->strutSizeHint( QskCheckBox::Box );

    auto r = checkBox->innerBox( QskCheckBox::Panel, contentsRect );

    if ( checkBox->layoutMirroring() )
        r.setLeft( r.right() - size.width() );
    else
        r.setWidth( size.width() );

    r.setTop( r.top() + 0.5 * ( r.height() - size.height() ) );
    r.setHeight( size.height() );

    return r;
}

QRectF QskCheckBoxSkinlet::rippleRect(
    const QskCheckBox* checkBox, const QRectF& rect ) const
{
    const auto rippleSize = checkBox->strutSizeHint( QskCheckBox::Ripple );
    const auto boxSize = checkBox->strutSizeHint( QskCheckBox::Box );

    const auto w = ( rippleSize.width() - boxSize.width() ) / 2;
    const auto h = ( rippleSize.height() - boxSize.height() ) / 2;

    auto r = boxRect( checkBox, rect );
    r = r.marginsAdded( { w, h, w, h } );

    return r;
}

QSGNode* QskCheckBoxSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    using Q = QskCheckBox;

    auto checkBox = static_cast< const QskCheckBox* >( skinnable );

    switch( nodeRole )
    {
        case PanelRole:
            return updateBoxNode( skinnable, node, Q::Panel );

        case BoxRole:
            return updateBoxNode( skinnable, node, Q::Box );

        case IndicatorRole:
            return updateSymbolNode( checkBox, node, Q::Indicator );

        case TextRole:
            return updateTextNode( checkBox, node );

        case RippleRole:
            return updateBoxNode( checkBox, node, Q::Ripple );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskCheckBoxSkinlet::updateTextNode(
    const QskCheckBox* checkBox, QSGNode* node ) const
{
    using Q = QskCheckBox;

    const auto rect = checkBox->subControlRect( Q::Text );
    const auto alignH = checkBox->layoutMirroring()
        ? Qt::AlignRight : Qt::AlignLeft;

    return QskSkinlet::updateTextNode( checkBox, node,
        rect, alignH | Qt::AlignVCenter, checkBox->text(), Q::Text );
}

QSizeF QskCheckBoxSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    using Q = QskCheckBox;

    if ( which != Qt::PreferredSize )
        return QSizeF();

    auto checkBox = static_cast< const QskCheckBox* >( skinnable );

    auto size = skinnable->strutSizeHint( QskCheckBox::Box );
    size = skinnable->outerBoxSize( Q::Panel, size );

    auto text = checkBox->text();
    if ( !text.isEmpty() )
    {
        qreal extra = skinnable->spacingHint( Q::Panel );

        if ( which == Qt::MinimumSize )
            text = 'W';

        const auto font = skinnable->effectiveFont( Q::Text );
        extra += qskHorizontalAdvance( font, text );

        size.setWidth( size.width() + extra );
    }

    return size.expandedTo( skinnable->strutSizeHint( Q::Panel ) );
}

#include "moc_QskCheckBoxSkinlet.cpp"
