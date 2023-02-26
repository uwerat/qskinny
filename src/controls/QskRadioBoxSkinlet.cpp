/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskRadioBoxSkinlet.h"

#include "QskAspect.h"
#include "QskRadioBox.h"

#include "QskSkinStateChanger.h"
#include "QskStandardSymbol.h"
#include "QskColorFilter.h"
#include "QskGraphic.h"
#include "QskFunctions.h"
#include "QskSkin.h"

#include <qfontmetrics.h>

namespace
{
    QskAspect::States statesForIndex( const QskRadioBox* radioBox, int index )
    {
        using Q = QskRadioBox;

        auto states = radioBox->skinStates();

        if( radioBox->selectedIndex() == index )
            states |= Q::Selected;

        if( radioBox->pressedIndex() == index )
            states |= Q::Pressed;

        if( radioBox->positionHint( Q::Ripple ) == index )
            states |= Q::Focused;

        return states;
    }

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
}

QskRadioBoxSkinlet::QskRadioBoxSkinlet( QskSkin* )
{
    setNodeRoles( { PanelRole, ButtonRole, SymbolRole, TextRole, RippleRole } );
}

QskRadioBoxSkinlet::~QskRadioBoxSkinlet()
{
}

QRectF QskRadioBoxSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subcontrol ) const
{
    auto radioBox = static_cast< const QskRadioBox* >( skinnable );

    if( subcontrol == QskRadioBox::Ripple )
        return rippleRect( radioBox, contentsRect );

    return contentsRect;
}

QSizeF QskRadioBoxSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint, const QSizeF& ) const
{
    using Q = QskRadioBox;

    auto radioBox = static_cast< const QskRadioBox* >( skinnable );

    const auto font = skinnable->effectiveFont( Q::Text );
    const auto textMargins = skinnable->marginHint( Q::Text );
    const auto buttonMargins = skinnable->marginHint( Q::Button );
    const auto symbolMargins = skinnable->marginHint( Q::Symbol );

    qreal maxTextWidth = 0;
    for( const auto& item : radioBox->items() )
        maxTextWidth = std::max( maxTextWidth, qskHorizontalAdvance( font, item ) );

    auto buttonWidth = radioBox->strutSizeHint( Q::Button ).width();
    auto symbolWidth = radioBox->strutSizeHint( Q::Symbol ).width();

    maxTextWidth += textMargins.left() + textMargins.right();
    buttonWidth += buttonMargins.left() + buttonMargins.right();
    symbolWidth += symbolMargins.left() + symbolMargins.right();

    auto spacing = radioBox->spacingHint( Q::Panel );
    return QSizeF( maxTextWidth + qMax( buttonWidth, symbolWidth ),
        ( lineHeight( radioBox ) + spacing ) * radioBox->items().size() - spacing );
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

        case SymbolRole:
            return updateSeriesNode( skinnable, Q::Symbol, node );

        case TextRole:
            return updateSeriesNode( skinnable, Q::Text, node );

        case RippleRole:
        {
            auto radioBox = static_cast< const QskRadioBox* >( skinnable );

            QskSkinStateChanger changer( radioBox );
            auto ripplePosition = radioBox->positionHint( Q::Ripple );
            changer.setStates( statesForIndex( radioBox, ripplePosition ) );

            return updateBoxNode( radioBox, node, Q::Ripple );
        }
    };

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

int QskRadioBoxSkinlet::sampleCount(
    const QskSkinnable* skinnable, QskAspect::Subcontrol ) const
{
    const auto radioBox = static_cast< const QskRadioBox* >( skinnable );
    return radioBox->items().count();
}

QSizeF QskRadioBoxSkinlet::buttonSymbolSize( const QskRadioBox* radioBox ) const
{
    using Q = QskRadioBox;

    auto buttonStrut = radioBox->strutSizeHint( Q::Button );
    auto symbolStrut = radioBox->strutSizeHint( Q::Symbol );

    buttonStrut = buttonStrut.grownBy( radioBox->marginHint( Q::Button ) );
    symbolStrut = symbolStrut.grownBy( radioBox->marginHint( Q::Symbol ) );

    return QSizeF( qMax( buttonStrut.width(), symbolStrut.width() ),
        qMax( buttonStrut.height(), symbolStrut.height() ) );
}

QRectF QskRadioBoxSkinlet::rippleRect(
    const QskRadioBox* radioBox, const QRectF& rect ) const
{
    using Q = QskRadioBox;

    auto ripplePosition = radioBox->positionHint( Q::Ripple );

    if( ripplePosition < 0 )
        return QRectF();

    auto rippleSize = radioBox->strutSizeHint( Q::Ripple );

    auto r = buttonRect( radioBox, Q::Button, rect, ripplePosition );

    r.moveLeft( r.x() - ( rippleSize.width() - r.width() ) / 2 );
    r.moveTop( r.y() - ( rippleSize.height() - r.height() ) / 2 );
    r.setSize( rippleSize );

    return r;
}

QRectF QskRadioBoxSkinlet::buttonRect( const QskRadioBox* radioBox,
    const QskAspect::Subcontrol target, const QRectF& rect, double index ) const
{
    using Q = QskRadioBox;

    if( index < 0 )
        return QRectF();

    auto result = rect;
    result.setSize( radioBox->strutSizeHint( target ) );

    auto spacing = radioBox->spacingHint( Q::Panel );
    result.moveTop( ( lineHeight( radioBox ) + spacing ) * index );

    auto margins = radioBox->marginHint( target );
    auto withMargins = result.size().grownBy( margins );

    auto maxSize = buttonSymbolSize( radioBox );
    auto alignment = radioBox->alignmentHint( target );

    // Vertical positioning
    const auto alignHeight = maxSize.height() - withMargins.height();
    if( alignment.testFlag( Qt::AlignVCenter ) )
        result.moveTop( result.top() + alignHeight / 2 );
    else if( alignment.testFlag( Qt::AlignBottom ) )
        result.moveTop( result.top() + alignHeight );

    result.moveTop( result.top() + margins.top() );

    // Horizontal positioning
    auto alignWidth = 0;
    if( alignment.testFlag( Qt::AlignHCenter ) )
        alignWidth = ( maxSize.width() - withMargins.width() ) / 2;
    else if ( alignment.testFlag( Qt::AlignRight ) )
        alignWidth = maxSize.width() - withMargins.width();

    if( radioBox->layoutMirroring() )
        result.moveRight( rect.width() - ( alignWidth + margins.right() ) );
    else
        result.moveLeft( margins.left() + alignWidth );

    return result;
}

QRectF QskRadioBoxSkinlet::textRect( const QskRadioBox* radioBox,
    const QRectF& rect, int index ) const
{
    using Q = QskRadioBox;

    const auto text = radioBox->items()[ index ];
    if( text.isEmpty() )
        return QRectF();

    QRectF result = rect;
    auto spacing = radioBox->spacingHint( Q::Panel );
    auto lh = lineHeight( radioBox );
    const auto textMargins = radioBox->marginHint( Q::Text );
    const auto font = radioBox->effectiveFont( Q::Text );

    result.moveTop( index * ( lh + spacing )
        + lh - QFontMetricsF( font ).height() + textMargins.top() );

    result.setHeight( lh );
    result.setWidth( qskHorizontalAdvance( font, text ) );

    const auto button = buttonRect( radioBox, Q::Button, rect, index );
    const auto buttonsMargins = radioBox->marginHint( Q::Button );
    const auto buttonWidth = button.marginsAdded( buttonsMargins ).width();

    if( radioBox->layoutMirroring() )
    {
        result.moveLeft( rect.width() - textMargins.right()
            - result.width() - buttonWidth);
    }
    else
    {
        result.moveLeft( buttonWidth + textMargins.left() );
    }

    return result;
}

QRectF QskRadioBoxSkinlet::sampleRect( const QskSkinnable* skinnable,
    const QRectF& rect, QskAspect::Subcontrol subcontrol, int index ) const
{
    using Q = QskRadioBox;

    auto radioBox = static_cast< const QskRadioBox* >( skinnable );

    if( subcontrol == Q::Text )
        return textRect( radioBox, rect, index );

    return buttonRect( radioBox, subcontrol, rect, index);
}

QskAspect::States QskRadioBoxSkinlet::sampleStates( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, int index ) const
{
    auto radioBox = static_cast< const QskRadioBox* >( skinnable );
    auto states = Inherited::sampleStates( skinnable, subControl, index );

    return states | statesForIndex( radioBox, index );
}

QSGNode* QskRadioBoxSkinlet::updateSampleNode( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subcontrol, int index, QSGNode* node ) const
{
    using Q = QskRadioBox;

    auto radioBox = static_cast< const QskRadioBox* >( skinnable );

    auto rect = sampleRect( skinnable, radioBox->contentsRect(), subcontrol, index );

    if( subcontrol == Q::Text )
    {
        return QskSkinlet::updateTextNode( radioBox, node, rect, Qt::AlignLeft,
            radioBox->items()[ index ], subcontrol );
    }
    else if ( subcontrol == Q::Button )
    {
        return QskSkinlet::updateBoxNode( radioBox, node, rect, subcontrol );
    }
    else if( subcontrol == Q::Symbol )
    {
        auto symbol = QskStandardSymbol::NoSymbol;
        auto color = radioBox->color( subcontrol ).rgb();

        if( radioBox->selectedIndex() == index )
        {
            symbol = QskStandardSymbol::Bullet;
            color = radioBox->color( subcontrol | Q::Selected ).rgb();
        }

        auto graphic = radioBox->effectiveSkin()->symbol( symbol );

        /*
            Our default skins do not have the concept of colorRoles
            implemented. Until then we do the recoloring manually here
         */
        QskColorFilter filter;
        filter.addColorSubstitution( Qt::black, color );

        auto colorSub = radioBox->color( subcontrol | statesForIndex( radioBox, index ) );
        filter.addColorSubstitution( Qt::black, colorSub.rgb() );

        QskGraphic::fromGraphic( graphic, filter );

        return updateGraphicNode( radioBox, node, graphic, filter, rect );
    }

    return node;
}

#include "moc_QskRadioBoxSkinlet.cpp"
