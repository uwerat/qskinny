/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSwitchButtonSkinlet.h"
#include "QskSwitchButton.h"

using Q = QskSwitchButton;

static inline qreal qskEffectivePosition( const QskSwitchButton* switchButton )
{
    auto pos = switchButton->positionHint( QskSwitchButton::Handle );
    pos = qBound( 0.0, pos, 1.0 );

    if( switchButton->isInverted() )
        pos = 1.0 - pos;

    if ( switchButton->orientation() == Qt::Horizontal )
    {
        if( switchButton->layoutMirroring() )
            pos = 1.0 - pos;
    }

    return pos;
}

static QSizeF qskIconSize( const QskSwitchButton* button )
{
    if( button->iconMode() == Q::NoIcon
         || ( button->iconMode() == Q::ShowIconWhenSelected && !button->isChecked() ) )
    {
        return {};
    }
    else
    {
        return button->strutSizeHint( Q::Icon );
    }
}

QskSwitchButtonSkinlet::QskSwitchButtonSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { GrooveRole, HandleRole, IconRole } );
}

QskSwitchButtonSkinlet::~QskSwitchButtonSkinlet()
{
}

QRectF QskSwitchButtonSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto button = static_cast< const Q* >( skinnable );

    if ( subControl == Q::Groove )
    {
        return grooveRect( button, contentsRect );
    }

    if ( subControl == Q::Handle )
    {
        return handleRect( button, contentsRect );
    }

    if ( subControl == Q::Icon )
    {
        return iconRect( button, contentsRect );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSizeF QskSwitchButtonSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto grooveHint = skinnable->strutSizeHint( QskSwitchButton::Groove );
    const auto handleHint = skinnable->strutSizeHint( QskSwitchButton::Handle );

    auto hint = grooveHint;
    hint = hint.expandedTo( handleHint );

    return hint;
}

QSGNode* QskSwitchButtonSkinlet::updateSubNode( const QskSkinnable* skinnable,
    quint8 nodeRole, QSGNode* node ) const
{
    switch ( nodeRole )
    {
        case HandleRole:
            return updateBoxNode( skinnable, node, Q::Handle );

        case IconRole:
            return updateSymbolNode( skinnable, node, Q::Icon );

        case GrooveRole:
            return updateBoxNode( skinnable, node, Q::Groove );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRectF QskSwitchButtonSkinlet::grooveRect(
    const QskSwitchButton* button, const QRectF& contentsRect ) const
{
    auto size = button->strutSizeHint( Q::Groove );

    if ( button->orientation() == Qt::Vertical )
    {
        if ( size.height() < 0.0 )
        {
            const auto handleSize = button->strutSizeHint( Q::Handle );
            size.setHeight( 2 * handleSize.height() );
        }
    }
    else
    {
        if ( size.width() < 0.0 )
        {
            const auto handleSize = button->strutSizeHint( Q::Handle );
            size.setWidth( 2 * handleSize.width() );
        }
    }

    size = size.expandedTo( QSize( 0.0, 0.0 ) );

    QRectF r;
    r.setSize( size );
    r.moveCenter( contentsRect.center() );

    return r;
}

QRectF QskSwitchButtonSkinlet::handleRect(
    const QskSwitchButton* button, const QRectF& contentsRect ) const
{
    const auto grooveRect = subControlRect( button, contentsRect, Q::Groove );
    const auto pos = qskEffectivePosition( button );

    auto size = button->strutSizeHint( Q::Handle );

    qreal cx, cy;

    if( button->orientation() == Qt::Vertical )
    {
        const qreal y0 = grooveRect.y() + 0.5 * size.height();
        const qreal h = grooveRect.height() - size.height();

        cx = grooveRect.x() + 0.5 * grooveRect.width();
        cy = y0 + pos * h;
    }
    else
    {
        const qreal x0 = grooveRect.x() + 0.5 * size.width();
        const qreal w = grooveRect.width() - size.width();

        cx = x0 + pos * w;
        cy = grooveRect.y() + 0.5 * grooveRect.height();
    }

    auto iconSize = qskIconSize( button );

    if( !iconSize.isNull() )
    {
        auto padding = button->paddingHint( Q::Icon );

        // need to compensate for the margins,
        // which might differ between states:
        auto margins = button->marginHint( Q::Handle );

        iconSize = iconSize.grownBy( padding ).grownBy( margins );
        size = size.expandedTo( iconSize );
    }

    QRectF r;
    r.setSize( size );
    r.moveCenter( QPointF( cx, cy ) );

    return r;
}

QRectF QskSwitchButtonSkinlet::iconRect( const QskSwitchButton* button, const QRectF& contentsRect ) const
{
    QRectF rect;
    rect.setSize( qskIconSize( button ) );
    const auto hr = handleRect( button, contentsRect );
    rect.moveCenter( hr.center() );
    return rect;
}


#include "moc_QskSwitchButtonSkinlet.cpp"
