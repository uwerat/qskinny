/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSpinBoxSkinlet.h"
#include "QskSpinBox.h"
#include "QskFunctions.h"
#include "QskSkinStateChanger.h"

#include <qfontmetrics.h>

static inline QskAspect::States qskButtonStates(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl )
{
    using Q = QskSpinBox;

    auto spinBox = static_cast< const QskSpinBox* >( skinnable );

    auto states = spinBox->skinStates();

    if ( spinBox->isEnabled() && !spinBox->isWrapping() )
    {
        if ( subControl == Q::DownIndicator || subControl == Q::DownPanel )
        {
            if ( spinBox->value() <= spinBox->minimum() )
                states |= QskControl::Disabled;
        }
        else if ( subControl == Q::UpIndicator || subControl == Q::UpPanel )
        {
            if ( spinBox->value() >= spinBox->maximum() )
                states |= QskControl::Disabled;
        }
    }

    return states;
}

QskSpinBoxSkinlet::QskSpinBoxSkinlet( QskSkin* )
{
    setNodeRoles( { UpPanel, DownPanel, TextPanel,
        UpIndicator, DownIndicator, Text } );
}

QRectF QskSpinBoxSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    using Q = QskSpinBox;

    QskSkinStateChanger stateChanger( skinnable );
    stateChanger.setStates( qskButtonStates( skinnable, subControl ) );

    if ( subControl == Q::DownIndicator )
        return skinnable->subControlContentsRect( contentsRect, Q::DownPanel );

    if ( subControl == Q::UpIndicator )
        return skinnable->subControlContentsRect( contentsRect, Q::UpPanel );

    if ( subControl == Q::Text )
        return skinnable->subControlContentsRect( contentsRect, Q::TextPanel );

    if ( subControl == Q::DownPanel || subControl == Q::UpPanel )
        return buttonRect( skinnable, contentsRect, subControl );

    if ( subControl == Q::TextPanel )
        return textPanelRect( skinnable, contentsRect );

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskSpinBoxSkinlet::updateSubNode(
    const QskSkinnable* skinnable, const quint8 nodeRole, QSGNode* const node ) const
{
    using Q = QskSpinBox;

    QskSkinStateChanger stateChanger( skinnable );

    switch( nodeRole )
    {
        case UpPanel:
        {
            stateChanger.setStates( qskButtonStates( skinnable, Q::UpPanel ) );
            return updateBoxNode( skinnable, node, Q::UpPanel );
        }

        case DownPanel:
        {
            stateChanger.setStates( qskButtonStates( skinnable, Q::DownPanel ) );
            return updateBoxNode( skinnable, node, Q::DownPanel );
        }

        case UpIndicator:
        {
            stateChanger.setStates( qskButtonStates( skinnable, Q::UpIndicator ) );
            return updateSymbolNode( skinnable, node, Q::UpIndicator );
        }

        case DownIndicator:
        {
            stateChanger.setStates( qskButtonStates( skinnable, Q::DownIndicator ) );
            return updateSymbolNode( skinnable, node, Q::DownIndicator );
        }

        case TextPanel:
        {
            return updateBoxNode( skinnable, node, Q::TextPanel );
        }

        case Text:
        {
            auto spinBox = static_cast< const QskSpinBox* >( skinnable );

            const auto rect = subControlRect( spinBox, spinBox->contentsRect(), Q::Text );

            return updateTextNode( spinBox, node, rect,
                spinBox->textAlignment(), spinBox->text(), Q::Text );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRectF QskSpinBoxSkinlet::textPanelRect(
    const QskSkinnable* skinnable, const QRectF& rect ) const
{
    using Q = QskSpinBox;

    auto spinBox = static_cast< const QskSpinBox* >( skinnable );

    const auto decoration = spinBox->decoration();
    if ( decoration == Q::NoDecoration )
        return rect;

    auto r = rect;

    const auto spacing = spinBox->spacingHint( Q::Panel );

    if ( decoration == Q::UpDownControl )
    {
        const auto w = subControlRect( skinnable, rect, Q::UpPanel ).width();
        if ( w > 0.0 )
            r.setRight( r.right() - spacing - w );
    }
    else
    {
        const auto w1 = subControlRect( skinnable, rect, Q::DownPanel ).width();
        if ( w1 > 0.0 )
            r.setLeft( r.left() + w1 + spacing );

        const auto w2 = subControlRect( skinnable, rect, Q::UpPanel ).width();
        if ( w2 > 0.0 )
            r.setRight( r.right() - w2 - spacing );
    }

    return r;
}

QRectF QskSpinBoxSkinlet::buttonRect( const QskSkinnable* skinnable,
    const QRectF& rect, QskAspect::Subcontrol subControl ) const
{
    using Q = QskSpinBox;

    const auto spinBox = static_cast< const QskSpinBox* >( skinnable );

    if ( const auto decoration = spinBox->decoration() )
    {
        qreal x, y, w, h;

        if ( decoration == Q::UpDownControl )
        {
            const auto hint1 = spinBox->strutSizeHint( Q::UpPanel );
            const auto hint2 = spinBox->strutSizeHint( Q::DownPanel );

            w = std::max( hint1.width(), hint2.width() );
            if ( w <= 0 )
                w = rect.height();

            h = 0.5 * rect.height();

            x = rect.right() - w;
            y = ( subControl == Q::UpPanel ) ? rect.top() : rect.bottom() - h;
        }
        else
        {
            const auto hint = spinBox->strutSizeHint( subControl );

            h = hint.height();
            if ( h <= 0.0 )
                h = rect.height();

            w = hint.width();
            if ( w <= 0.0 )
                w = h;

            x = ( subControl == Q::UpPanel ) ? rect.right() - w : rect.left();
            y = 0.5 * ( rect.height() - h );
        }

        return QRectF( x, y, w, h );
    }

    return QRectF();
}

QSizeF QskSpinBoxSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    using Q = QskSpinBox;

    const auto spinBox = static_cast< const QskSpinBox* >( skinnable );

    QSizeF hint;

    {
        const QFontMetricsF fm( spinBox->effectiveFont( Q::Text ) );

        // 18: QAbstractSpinBox does this
        const auto w1 = qskHorizontalAdvance( fm,
            spinBox->textFromValue( spinBox->minimum() ).left( 18 ) );

        const auto w2 = qskHorizontalAdvance( fm,
            spinBox->textFromValue( spinBox->maximum() ).left( 18 ) );

        hint.setWidth( std::max( w1, w2 ) );
        hint.setHeight( fm.height() );

        hint = hint.grownBy( spinBox->paddingHint( Q::TextPanel ) );
        hint = hint.expandedTo( spinBox->strutSizeHint( Q::TextPanel ) );
    }

    if ( const auto decoration = spinBox->decoration() )
    {
        const auto spacing = spinBox->spacingHint( Q::Panel );
        const auto hintUp = spinBox->strutSizeHint( Q::UpPanel );
        const auto hintDown = spinBox->strutSizeHint( Q::DownPanel );

        if ( decoration == QskSpinBox::UpDownControl )
        {
            qreal w = std::max( hintDown.width(), hintUp.width() );

            qreal h = 0.0;
            if ( hintDown.height() >= 0.0 )
                h += hintDown.height();

            if ( hintUp.height() >= 0.0 )
                h += hintUp.height();

            hint.rwidth() += ( w >= 0.0 ) ? w : hint.height();
            hint.rwidth() += spacing;

            hint.rheight() = std::max( h, hint.height() );
        }
        else
        {
            if ( hintDown.width() > 0.0 )
                hint.rwidth() += hintDown.width() + spacing;

            if ( hintUp.width() > 0.0 )
                hint.rwidth() += hintUp.width() + spacing;

            const auto h = std::max( hintUp.height(), hintDown.height() );
            hint.rheight() = qMax( h, hint.height() );
        }
    }

    hint = hint.expandedTo( spinBox->strutSizeHint( Q::Panel ) );
    return hint;
}

#include "moc_QskSpinBoxSkinlet.cpp"
