/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskRadioBoxSkinlet.h"

#include "QskRadioBox.h"
#include "QskFunctions.h"

#include <qfontmetrics.h>

namespace
{
    QSizeF buttonSizeHint( const QskSkinnable* skinnable,
        const QFontMetricsF& fm, const QString& text )
    {
        using Q = QskRadioBox;

        auto hint = skinnable->strutSizeHint( Q::CheckIndicatorPanel );

        hint.rwidth() += skinnable->spacingHint( Q::Button )
            + qskHorizontalAdvance( fm, text );
        hint.rheight() = qMax( hint.height(), fm.height() );

        hint = hint.grownBy( skinnable->paddingHint( Q::Button ) );
        hint = hint.expandedTo( skinnable->strutSizeHint( Q::Button ) );

        return hint;
    }

    QSizeF buttonSizeHint( const QskRadioBox* radioBox, int index )
    {
        const QFontMetrics fm( radioBox->effectiveFont( QskRadioBox::Text ) );
        return buttonSizeHint( radioBox, fm, radioBox->optionAt( index ) );
    }
}

QskRadioBoxSkinlet::QskRadioBoxSkinlet( QskSkin* )
{
    setNodeRoles( { PanelRole, ButtonRole, CheckPanelRole,
        CheckIndicatorRole, TextRole, RippleRole } );
}

QskRadioBoxSkinlet::~QskRadioBoxSkinlet()
{
}

QRectF QskRadioBoxSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subcontrol ) const
{
    using Q = QskRadioBox;

    auto radioBox = static_cast< const QskRadioBox* >( skinnable );

    if( subcontrol == Q::Ripple )
        return rippleRect( radioBox, contentsRect );

    return contentsRect;
}

QSGNode* QskRadioBoxSkinlet::updateSubNode( const QskSkinnable* skinnable,
    quint8 nodeRole, QSGNode* node ) const
{
    using Q = QskRadioBox;

    switch( nodeRole )
    {
        case PanelRole:
            return updateBoxNode( skinnable, node, Q::Panel );

        case ButtonRole:
            return updateSeriesNode( skinnable, Q::Button, node );

        case CheckPanelRole:
            return updateSeriesNode( skinnable, Q::CheckIndicatorPanel, node );

        case CheckIndicatorRole:
            return updateSeriesNode( skinnable, Q::CheckIndicator, node );

        case TextRole:
            return updateSeriesNode( skinnable, Q::Text, node );

        case RippleRole:
            return updateBoxNode( skinnable, node, Q::Ripple );
    };

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

int QskRadioBoxSkinlet::sampleCount(
    const QskSkinnable* skinnable, QskAspect::Subcontrol ) const
{
    const auto radioBox = static_cast< const QskRadioBox* >( skinnable );
    return radioBox->options().count();
}

QRectF QskRadioBoxSkinlet::rippleRect(
    const QskRadioBox* radioBox, const QRectF& rect ) const
{
    using Q = QskRadioBox;

    auto index = radioBox->positionHint( Q::Ripple );
    if( index < 0 )
        return QRectF();

    QRectF r;
    r.setSize( radioBox->strutSizeHint( Q::Ripple ) );

    if ( !r.isEmpty() )
    {
        const auto checkBoxRect = sampleRect(
            radioBox, rect, Q::CheckIndicatorPanel, index );

        r.moveCenter( checkBoxRect.center() );
    }

    return r;
}

QRectF QskRadioBoxSkinlet::buttonRect(
    const QskRadioBox* radioBox, const QRectF& rect, int index ) const
{
    using Q = QskRadioBox;

    /*
        code only works when all buttons have the same height
        - what might be wron, when lineWrapping is enabled TODO ...
     */

    const auto h = buttonSizeHint( radioBox, index ).height();
    const auto y = index * ( h + radioBox->spacingHint( Q::Panel ) );

    const auto r = radioBox->subControlContentsRect( rect, Q::Panel );
    return QRectF( r.left(), r.top() + y, r.width(), h );
}

QRectF QskRadioBoxSkinlet::checkPanelRect( const QskRadioBox* radioBox,
    const QRectF& rect, int index ) const
{
    using Q = QskRadioBox;

    auto r = sampleRect( radioBox, rect, Q::Button, index );
    r = radioBox->innerBox( Q::Button, r );

    auto alignment = radioBox->alignmentHint( Q::CheckIndicatorPanel, Qt::AlignCenter );

    alignment &= Qt::AlignVertical_Mask;
    alignment |= radioBox->layoutMirroring() ? Qt::AlignRight : Qt::AlignLeft;

    auto size = radioBox->strutSizeHint( Q::CheckIndicatorPanel );
    size = size.grownBy( radioBox->marginHint( Q::CheckIndicatorPanel ) );

    return qskAlignedRectF( r, size.width(), size.height(), alignment );
}

QRectF QskRadioBoxSkinlet::textRect( const QskRadioBox* radioBox,
    const QRectF& rect, int index ) const
{
    using Q = QskRadioBox;

    auto r = sampleRect( radioBox, rect, Q::Button, index );
    r = radioBox->innerBox( Q::Button, r );

    const auto checkPanelRect = sampleRect(
        radioBox, rect, Q::CheckIndicatorPanel, index );

    const auto spacing = radioBox->spacingHint( Q::Button );

    if ( !radioBox->layoutMirroring() )
        r.setLeft( checkPanelRect.right() + spacing );
    else
        r.setRight( checkPanelRect.left() - spacing );

    return r;
}

QRectF QskRadioBoxSkinlet::sampleRect( const QskSkinnable* skinnable,
    const QRectF& rect, QskAspect::Subcontrol subControl, int index ) const
{
    using Q = QskRadioBox;

    auto radioBox = static_cast< const QskRadioBox* >( skinnable );

    if( subControl == Q::Text )
        return textRect( radioBox, rect, index );

    if( subControl == Q::Button )
        return buttonRect( radioBox, rect, index );

    if( subControl == Q::CheckIndicatorPanel )
        return checkPanelRect( radioBox, rect, index );

    if( subControl == Q::CheckIndicator )
    {
        auto r = sampleRect( radioBox, rect, Q::CheckIndicatorPanel, index );
        r = r.marginsRemoved( radioBox->paddingHint( Q::CheckIndicatorPanel ) );

        return r;
    }

    return QRectF();
}

QskAspect::States QskRadioBoxSkinlet::sampleStates(
    const QskSkinnable* skinnable, QskAspect::Subcontrol, int index ) const
{
    using Q = QskRadioBox;

    auto radioBox = static_cast< const QskRadioBox* >( skinnable );

    auto states = radioBox->skinStates();

    if( radioBox->selectedIndex() == index )
        states |= Q::Selected;

    if( radioBox->pressedIndex() == index )
        states |= Q::Pressed;

#if 1
    if( radioBox->positionHint( Q::Ripple ) == index )
        states |= Q::Focused;
    else
        states &= ~Q::Focused;
#endif

    return states;
}

QSGNode* QskRadioBoxSkinlet::updateSampleNode( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subcontrol, int index, QSGNode* node ) const
{
    using Q = QskRadioBox;

    auto radioBox = static_cast< const QskRadioBox* >( skinnable );

    auto rect = sampleRect( skinnable, radioBox->contentsRect(), subcontrol, index );

    if( subcontrol == Q::Text )
    {
        Qt::Alignment alignment = Qt::AlignVCenter;
        alignment |= ( radioBox->layoutMirroring() ? Qt::AlignRight : Qt::AlignLeft );

        alignment = radioBox->alignmentHint( Q::Text, Qt::AlignCenter );
        alignment &= Qt::AlignVertical_Mask;
        alignment |= radioBox->layoutMirroring() ? Qt::AlignRight : Qt::AlignLeft;

        return updateTextNode( radioBox, node, rect,
            alignment, radioBox->optionAt( index ), subcontrol );
    }

    if ( subcontrol == Q::CheckIndicatorPanel || subcontrol == Q::CheckIndicator )
        return updateBoxNode( radioBox, node, rect, subcontrol );

    return node;
}

QSizeF QskRadioBoxSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    using Q = QskRadioBox;

    if ( which != Qt::PreferredSize )
        return QSizeF();

    if ( constraint.width() >= 0.0 )
    {
        // heightForWidth would make sense when word wrapping is enabled TODO ...
    }

    const auto radioBox = static_cast< const QskRadioBox* >( skinnable );

    const QFontMetrics fm( radioBox->effectiveFont( QskRadioBox::Text ) );

    qreal w = 0.0;
    qreal h = 0.0;

    const auto options = radioBox->options();
    for( const auto& option : options )
    {
        const auto buttonSize = buttonSizeHint( radioBox, fm, option );

        w = qMax( w, buttonSize.width() );
        h += buttonSize.height();
    }

    if ( auto count = radioBox->options().count() )
        h += ( count - 1 ) * skinnable->spacingHint( Q::Panel );

    QSizeF hint( w, h );
    hint = hint.grownBy( skinnable->paddingHint( Q::Panel ) );
    hint = hint.expandedTo( skinnable->strutSizeHint( Q::Panel ) );

    return hint;
}

#include "moc_QskRadioBoxSkinlet.cpp"
