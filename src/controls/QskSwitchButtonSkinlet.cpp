/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSwitchButton.h"
#include "QskSwitchButtonSkinlet.h"
#include "QskSGNode.h"

QskSwitchButtonSkinlet::QskSwitchButtonSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { GrooveRole, HandleRole } );
}

QskSwitchButtonSkinlet::~QskSwitchButtonSkinlet()
{
}

QRectF QskSwitchButtonSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl) const
{
    using Q = QskSwitchButton;

    const auto switchButton = static_cast< const Q* >( skinnable );

    if ( subControl == Q::Handle )
    {
        const auto diameter = 2 * skinnable->metric( Q::Handle | QskAspect::Size );
        const auto grooveSize = skinnable->strutSizeHint( Q::Groove );

        auto position = skinnable->metric( Q::Handle | QskAspect::Position );
        if( switchButton->isInverted() )
            position = 1.0 - position;

        auto rect = QRectF( 0, 0, diameter, diameter );

        if( switchButton->orientation() == Qt::Vertical )
        {
            if( diameter < grooveSize.height() )
                rect.moveLeft( ( grooveSize.height() - diameter ) / 2 );

            rect.moveTop( ( grooveSize.height() - diameter ) / 2
                + position * ( grooveSize.width() - diameter
                - ( grooveSize.height() - diameter ) ) );
        }
        else
        {
            if( switchButton->layoutMirroring() )
                position = 1.0 - position;

            if( diameter < grooveSize.height() )
                rect.moveTop( ( grooveSize.height() - diameter ) / 2 );

            rect.moveLeft( ( grooveSize.height() - diameter ) / 2
                + position * ( grooveSize.width() - diameter
                - ( grooveSize.height() - diameter ) ) );
        }

        return rect;
    }

    if ( subControl == Q::Groove )
    {
        auto diameter = 2 * skinnable->metric( Q::Handle | QskAspect::Size );
        const auto grooveSize = skinnable->strutSizeHint( Q::Groove );

        auto result = contentsRect;
        result.setSize( grooveSize );

        if( switchButton->orientation() == Qt::Vertical )
        {
            if( grooveSize.height() < diameter )
                result.moveLeft( ( diameter - result.height() ) / 2 );

            return result.transposed();
        }
        else
        {
            if( grooveSize.height() < diameter )
                result.moveTop( ( diameter - result.height() ) / 2 );

            return result;
        }
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSizeF QskSwitchButtonSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint, const QSizeF&) const
{
    using Q = QskSwitchButton;

    const auto switchButton = static_cast< const Q* >( skinnable );
    const auto diameter = 2 * skinnable->metric( Q::Handle | QskAspect::Size );

    auto hint = skinnable->strutSizeHint( Q::Groove );
    hint = hint.expandedTo( QSizeF( diameter, diameter ) );

    if( switchButton->orientation() == Qt::Vertical )
        hint.transpose();

    return hint;
}

QSGNode* QskSwitchButtonSkinlet::updateSubNode( const QskSkinnable* skinnable,
    quint8 nodeRole, QSGNode* node) const
{
    using Q = QskSwitchButton;

    switch ( nodeRole )
    {
        case HandleRole:
            return updateBoxNode( skinnable, node, Q::Handle );

        case GrooveRole:
            return updateBoxNode( skinnable, node, Q::Groove );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

#include "moc_QskSwitchButtonSkinlet.cpp"
