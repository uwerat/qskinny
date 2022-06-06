/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSwitchButtonSkinlet.h"
#include "QskSwitchButton.h"

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

QskSwitchButtonSkinlet::QskSwitchButtonSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { GrooveRole, HandleRole, RippleRole } );
}

QskSwitchButtonSkinlet::~QskSwitchButtonSkinlet()
{
}

QRectF QskSwitchButtonSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    using Q = QskSwitchButton;

    if ( subControl == Q::Handle )
    {
        return handleRect( skinnable, contentsRect );
    }

    if ( subControl == Q::Groove )
    {
        return grooveRect( skinnable, contentsRect );
    }

    if ( subControl == Q::Ripple )
    {
        return rippleRect( skinnable, contentsRect );
    }


    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSizeF QskSwitchButtonSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    auto grooveHint = skinnable->strutSizeHint( QskSwitchButton::Groove );
    auto handleHint = skinnable->strutSizeHint( QskSwitchButton::Handle );
    auto rippleHint = skinnable->strutSizeHint( QskSwitchButton::Ripple );

    auto hint = grooveHint;
    hint = hint.expandedTo( rippleHint );
    hint = hint.expandedTo( handleHint );

    return hint;
}

QSGNode* QskSwitchButtonSkinlet::updateSubNode( const QskSkinnable* skinnable,
    quint8 nodeRole, QSGNode* node ) const
{
    using Q = QskSwitchButton;

    switch ( nodeRole )
    {
        case RippleRole:
            return updateBoxNode( skinnable, node, Q::Ripple );

        case HandleRole:
            return updateBoxNode( skinnable, node, Q::Handle );

        case GrooveRole:
            return updateBoxNode( skinnable, node, Q::Groove );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRectF QskSwitchButtonSkinlet::grooveRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect ) const
{
    using Q = QskSwitchButton;

    const auto switchButton = static_cast< const Q* >( skinnable );

    auto size = skinnable->strutSizeHint( Q::Groove );

    if ( switchButton->orientation() == Qt::Vertical )
    {
        if ( size.height() < 0.0 )
        {
            const auto handleSize = skinnable->strutSizeHint( Q::Handle );
            size.setHeight( 2 * handleSize.height() );
        }
    }
    else
    {
        if ( size.width() < 0.0 )
        {
            const auto handleSize = skinnable->strutSizeHint( Q::Handle );
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
    const QskSkinnable* skinnable, const QRectF& contentsRect ) const
{
    using Q = QskSwitchButton;

    const auto switchButton = static_cast< const Q* >( skinnable );

    const auto grooveRect = subControlRect( skinnable, contentsRect, Q::Groove );
    const auto pos = qskEffectivePosition( switchButton );
    const auto size = skinnable->strutSizeHint( Q::Handle );

    qreal cx, cy;

    if( switchButton->orientation() == Qt::Vertical )
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

    QRectF r;
    r.setSize( size );
    r.moveCenter( QPointF( cx, cy ) );

    return r;
}

QRectF QskSwitchButtonSkinlet::rippleRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect ) const
{
    using Q = QskSwitchButton;

    const auto switchButton = static_cast< const Q* >( skinnable );

    const auto grooveRect = subControlRect( skinnable, contentsRect, Q::Groove );
    const auto pos = qskEffectivePosition( switchButton );
    const auto sizeHandle = skinnable->strutSizeHint( Q::Handle );
    const auto sizeRipple = skinnable->strutSizeHint( Q::Ripple );

    qreal cx, cy;

    if( switchButton->orientation() == Qt::Vertical )
    {
        const qreal y0 = grooveRect.y() + 0.5 * sizeHandle.height();
        const qreal h = grooveRect.height() - sizeHandle.height();

        cx = grooveRect.x() + 0.5 * grooveRect.width();
        cy = y0 + pos * h;
    }
    else
    {
        const qreal x0 = grooveRect.x() + 0.5 * sizeHandle.width();
        const qreal w = grooveRect.width() - sizeHandle.width();

        cx = x0 + pos * w;
        cy = grooveRect.y() + 0.5 * grooveRect.height();
    }

    QRectF r;
    r.setSize( sizeRipple );
    r.moveCenter( QPointF( cx, cy ) );

    return r;
}

#include "moc_QskSwitchButtonSkinlet.cpp"
