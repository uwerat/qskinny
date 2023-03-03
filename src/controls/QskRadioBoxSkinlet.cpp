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
    qreal lineHeight( const QskRadioBox* radioBox )
    {
        using Q = QskRadioBox;

        auto strutHight = qMax( radioBox->strutSizeHint( Q::Button ).height(),
            radioBox->strutSizeHint( Q::Text ).height() );

        const auto textMargins = radioBox->marginHint( Q::Text );

        auto fontHeight = radioBox->effectiveFontHeight( Q::Text );
        fontHeight += textMargins.top() + textMargins.bottom();

        return qMax( strutHight, fontHeight );
    }

    QRectF lineRect( const QskRadioBox* radioBox, const QRectF& rect, int index )
    {
        using Q = QskRadioBox;

        auto h = lineHeight( radioBox );

        auto y = rect.top();
        if ( index > 0 )
        {
            const auto spacing = radioBox->spacingHint( Q::Panel );
            y += index * ( h + spacing );
        }

        return QRectF( rect.x(), y, rect.width(), h );
    }

    inline qreal maxTextWidth( const QskRadioBox* radioBox )
    {
        qreal w = 0.0;

        const QFontMetrics fm( radioBox->effectiveFont( QskRadioBox::Text ) );

        const auto options = radioBox->options();
        for( const auto& option : options )
            w = std::max( w, qskHorizontalAdvance( fm, option ) );

        return w;
    }

#if 1
    inline qreal lineSpacing( const QskRadioBox* )
    {
        // skinHint TODO ...
        return 10;
    }
#endif
}

QskRadioBoxSkinlet::QskRadioBoxSkinlet( QskSkin* )
{
    setNodeRoles( { PanelRole, ButtonRole, IndicatorRole, TextRole, RippleRole } );
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

        case IndicatorRole:
            return updateSeriesNode( skinnable, Q::Indicator, node );

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
        const auto buttonRect = sampleRect( radioBox, rect, Q::Button, index );
        r.moveCenter( buttonRect.center() );
    }

    return r;
}

QRectF QskRadioBoxSkinlet::buttonRect( const QskRadioBox* radioBox,
    const QRectF& rect, int index ) const
{
    using Q = QskRadioBox;

    auto r = lineRect( radioBox, rect, index );
    r = r.marginsRemoved( radioBox->paddingHint( Q::Panel ) );

    auto alignment = radioBox->alignmentHint( Q::Button, Qt::AlignCenter );

    alignment &= Qt::AlignVertical_Mask;
    alignment |= radioBox->layoutMirroring() ? Qt::AlignRight : Qt::AlignLeft;

    auto size = radioBox->strutSizeHint( Q::Button );
    size = size.grownBy( radioBox->marginHint( Q::Button ) );

    return qskAlignedRectF( r, size.width(), size.height(), alignment );
}

QRectF QskRadioBoxSkinlet::textRect( const QskRadioBox* radioBox,
    const QRectF& rect, int index ) const
{
    using Q = QskRadioBox;

    auto r = lineRect( radioBox, rect, index );
    r = r.marginsRemoved( radioBox->paddingHint( Q::Panel ) );

    const auto buttonRect = sampleRect( radioBox, rect, Q::Button, index );
    const auto spacing = lineSpacing( radioBox );

    if ( !radioBox->layoutMirroring() )
        r.setLeft( buttonRect.right() + spacing );
    else
        r.setRight( buttonRect.left() - spacing );

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
        return buttonRect( radioBox, rect, index);

    if( subControl == Q::Indicator )
    {
        auto r = sampleRect( radioBox, rect, Q::Button, index );
        r = r.marginsRemoved( radioBox->paddingHint( Q::Button ) );

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
            alignment, radioBox->option( index ), subcontrol );
    }

    if ( subcontrol == Q::Button || subcontrol == Q::Indicator )
        return updateBoxNode( radioBox, node, rect, subcontrol );

    return node;
}

QSizeF QskRadioBoxSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    using Q = QskRadioBox;

    const auto radioBox = static_cast< const QskRadioBox* >( skinnable );

    if ( which != Qt::PreferredSize )
        return QSizeF();

    if ( constraint.width() >= 0.0 )
    {
        // heightForWidth would make sense when word wrapping is enabled TODO ...
    }

    QSizeF textSize( maxTextWidth( radioBox ), 0.0 );

    textSize = textSize.expandedTo( skinnable->strutSizeHint( Q::Text ) );
    textSize = textSize.grownBy( skinnable->marginHint( Q::Text ) );

    QSizeF buttonSize = skinnable->strutSizeHint( Q::Button );
    buttonSize = buttonSize.grownBy( skinnable->marginHint( Q::Button ) );

    const auto count = std::max( ( int )radioBox->options().count(), 1 );

    const qreal w = textSize.width() + lineSpacing( radioBox ) + buttonSize.width();
    const qreal h = count * std::max( textSize.height(), buttonSize.height() )
        + ( count - 1 ) * skinnable->spacingHint( Q::Panel );

    QSizeF hint( w, h );
    hint = hint.grownBy( skinnable->paddingHint( Q::Panel ) );
    hint = hint.expandedTo( skinnable->strutSizeHint( Q::Panel ) );

    return hint;
}

#include "moc_QskRadioBoxSkinlet.cpp"
