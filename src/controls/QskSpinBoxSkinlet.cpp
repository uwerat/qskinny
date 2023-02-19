/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSpinBoxSkinlet.h"
#include "QskSpinBox.h"

#include <array>

namespace
{
    inline QPointF cursorPosSkinHint( const QskSpinBox& spinbox )
    {
        const auto aspect = QskSpinBox::Panel | QskAspect::Metric | QskAspect::Position;
        return spinbox.effectiveSkinHint( aspect ).toPointF();
    }

    enum
    {
        Dec = 0,
        Txt = 1,
        Inc = 2,
        Count
    };
}

QskSpinBoxSkinlet::QskSpinBoxSkinlet( QskSkin* )
{
    setNodeRoles( { IncrementPanel, DecrementPanel, TextPanel,
        IncrementIndicator, DecrementIndicator, Text } );
}

int QskSpinBoxSkinlet::sampleCount(
    const QskSkinnable*, QskAspect::Subcontrol ) const
{
    return 1;
}

QRectF QskSpinBoxSkinlet::sampleRect( const QskSkinnable* skinnable, const QRectF& rect,
    QskAspect::Subcontrol subControl, int index ) const
{
    if ( index == Dec || index == Inc || index == Txt )
    {
        return subControlRect( skinnable, rect, subControl );
    }

    return Inherited::sampleRect( skinnable, rect, subControl, index );
}

QskAspect::States QskSpinBoxSkinlet::sampleStates(
    const QskSkinnable* const skinnable, QskAspect::Subcontrol subControl, int index ) const
{
    using S = QskSpinBox;

    auto states = Inherited::sampleStates( skinnable, subControl, index );

    if ( subControl == S::DecrementPanel || subControl == S::IncrementPanel ||
         subControl == S::TextPanel )
    {
        const auto* const spinbox = static_cast< const S* >( skinnable );
        const auto cursorPos = cursorPosSkinHint( *spinbox );
        const QPointF cursorPosAbs{ qAbs( cursorPos.x() ), qAbs( cursorPos.y() ) };
        const auto subControlRect = spinbox->subControlRect( subControl );

        const auto contain = !cursorPosAbs.isNull() && subControlRect.contains( cursorPosAbs );
        const auto pressed = contain && ( cursorPos.x() < 0 || cursorPos.y() < 0 );
        const auto hovered = contain && !pressed;

        states.setFlag( QskControl::Hovered, hovered );
        states.setFlag( QskSpinBox::Pressed, pressed );
    }

    return states;
}

QSizeF QskSpinBoxSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& size ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    using S = QskSpinBox;

    const auto spinbox = static_cast< const QskSpinBox* >( skinnable );

    const auto layout = spinbox->alignmentHint( S::Panel );
    const auto spacing = spinbox->spacingHint( S::Panel );

    const auto strutInc = spinbox->strutSizeHint( S::IncrementPanel );
    const auto strutDec = spinbox->strutSizeHint( S::DecrementPanel );
    const auto strutTxt = spinbox->strutSizeHint( S::TextPanel );

    if ( layout == Qt::AlignTop || layout == Qt::AlignBottom || layout == Qt::AlignVCenter )
    {
        const auto w = qMax( strutDec.width(), qMax( strutTxt.width(), strutInc.width() ) );
        const auto h = strutDec.height() + strutTxt.height() + strutInc.height();

        return QSizeF( w, h + 2.0 * spacing );
    }

    if ( layout == Qt::AlignLeft || layout == Qt::AlignRight || layout == Qt::AlignHCenter )
    {
        const auto w = strutDec.width() + strutTxt.width() + strutInc.width();
        const auto h = qMax( strutDec.height(), qMax( strutTxt.height(), strutInc.height() ) );

        return QSizeF( w + 2.0 * spacing, h );
    }

    if ( layout == ( Qt::AlignLeft | Qt::AlignVCenter ) ||
         layout == ( Qt::AlignRight | Qt::AlignVCenter ) )
    {
        const auto w = strutTxt.width() + qMax( strutInc.width(), strutDec.width() );
        const auto h =
            qMax( 2.0 * qMax( strutInc.height(), strutDec.height() ), strutTxt.height() );

        return QSizeF( w + spacing, h + spacing );
    }

    if ( layout == ( Qt::AlignTop | Qt::AlignHCenter ) ||
         layout == ( Qt::AlignTop | Qt::AlignHCenter ) )
    {
        const auto w = qMax( strutTxt.width(), strutInc.width() + strutDec.width() );
        const auto h = strutTxt.height() + qMax( strutInc.height(), strutDec.height() );

        return QSizeF( w + spacing, h + spacing );
    }

    return Inherited::sizeHint( skinnable, which, size );
}

QRectF QskSpinBoxSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    using Q = QskSpinBox;

    if ( subControl == Q::DecrementIndicator )
        return skinnable->subControlContentsRect( contentsRect, Q::DecrementPanel );

    if ( subControl == Q::IncrementIndicator )
        return skinnable->subControlContentsRect( contentsRect, Q::IncrementPanel );

    if ( subControl == Q::Text )
        return skinnable->subControlContentsRect( contentsRect, Q::TextPanel );

    const auto* const spinbox = static_cast< const QskSpinBox* >( skinnable );

    const auto layout = spinbox->alignmentHint( Q::Panel );
    const auto spacing = spinbox->spacingHint( Q::Panel );

    std::array< QRectF, Count > rects = {
        QRectF{ QPointF(), spinbox->strutSizeHint( Q::DecrementPanel ) },
        QRectF{ QPointF(), spinbox->strutSizeHint( Q::TextPanel ) },
        QRectF{ QPointF(), spinbox->strutSizeHint( Q::IncrementPanel ) },
    };

    const auto center = contentsRect.center();

    if ( layout == Qt::AlignLeft )
    {
        rects[ Txt ].moveTopLeft( { 0.0, center.y() - rects[ Txt ].height() * 0.5 } );
        rects[ Dec ].moveTopLeft(
            { rects[ Txt ].right() + spacing, center.y() - rects[ Dec ].height() * 0.5 } );
        rects[ Inc ].moveTopLeft(
            { rects[ Dec ].right() + spacing, center.y() - rects[ Inc ].height() * 0.5 } );
    }
    else if ( layout == Qt::AlignRight )
    {
        rects[ Dec ].moveTopLeft( { 0.0, center.y() - rects[ Dec ].height() * 0.5 } );
        rects[ Inc ].moveTopLeft(
            { rects[ Dec ].right() + spacing, center.y() - rects[ Inc ].height() * 0.5 } );
        rects[ Txt ].moveTopLeft(
            { rects[ Inc ].right() + spacing, center.y() - rects[ Txt ].height() * 0.5 } );
    }
    else if ( layout == Qt::AlignTop )
    {
        rects[ Txt ].moveTopLeft( { center.x() - rects[ Txt ].width() * 0.5, 0.0 } );
        rects[ Inc ].moveTopLeft(
            { center.x() - rects[ Inc ].width() * 0.5, rects[ Txt ].bottom() + spacing } );
        rects[ Dec ].moveTopLeft(
            { center.x() - rects[ Dec ].width() * 0.5, rects[ Inc ].bottom() + spacing } );
    }
    else if ( layout == Qt::AlignBottom )
    {
        rects[ Inc ].moveTopLeft( { center.x() - rects[ Inc ].width() * 0.5, 0.0 } );
        rects[ Dec ].moveTopLeft(
            { center.x() - rects[ Dec ].width() * 0.5, rects[ Inc ].bottom() + spacing } );
        rects[ Txt ].moveTopLeft(
            { center.x() - rects[ Txt ].width() * 0.5, rects[ Dec ].bottom() + spacing } );
    }
    else if ( layout == Qt::AlignHCenter )
    {
        rects[ Dec ].moveTopLeft( { 0.0, center.y() - rects[ Dec ].height() * 0.5 } );
        rects[ Txt ].moveTopLeft(
            { rects[ Dec ].right() + spacing, center.y() - rects[ Txt ].height() * 0.5 } );
        rects[ Inc ].moveTopLeft(
            { rects[ Txt ].right() + spacing, center.y() - rects[ Inc ].height() * 0.5 } );
    }
    else if ( layout == Qt::AlignVCenter )
    {
        rects[ Inc ].moveTopLeft( { center.x() - rects[ Inc ].width() * 0.5, 0.0 } );
        rects[ Txt ].moveTopLeft(
            { center.x() - rects[ Txt ].width() * 0.5, rects[ Inc ].bottom() + spacing } );
        rects[ Dec ].moveTopLeft(
            { center.x() - rects[ Dec ].width() * 0.5, rects[ Txt ].bottom() + spacing } );
    }
    else if ( layout == ( Qt::AlignLeft | Qt::AlignVCenter ) )
    {
        rects[ Txt ].moveTopLeft( { 0.0, center.y() - rects[ Txt ].height() * 0.5 } );
        rects[ Inc ].moveTopLeft( { rects[ Txt ].right() + spacing,
            center.y() - spacing * 0.5 - rects[ Inc ].height() } );
        rects[ Dec ].moveTopLeft( { rects[ Txt ].right() + spacing, center.y() + spacing * 0.5 } );
    }
    else if ( layout == ( Qt::AlignRight | Qt::AlignVCenter ) )
    {
        const auto dx = qMax( rects[ Inc ].width(), rects[ Dec ].width() );
        rects[ Inc ].moveTopLeft(
            { dx - rects[ Inc ].width(), center.y() - spacing * 0.5 - rects[ Inc ].height() } );
        rects[ Dec ].moveTopLeft( { dx - rects[ Dec ].width(), center.y() + spacing * 0.5 } );
        rects[ Txt ].moveTopLeft( { dx + spacing, center.y() - rects[ Txt ].height() * 0.5 } );
    }
    else if ( layout == ( Qt::AlignTop | Qt::AlignHCenter ) )
    {
        rects[ Txt ].moveTopLeft( { center.x() - rects[ Txt ].width() * 0.5, 0.0 } );
        rects[ Dec ].moveTopLeft(
            { rects[ Txt ].center().x() - spacing * 0.5 - rects[ Dec ].width(),
                rects[ Txt ].bottom() + spacing } );
        rects[ Inc ].moveTopLeft(
            { rects[ Txt ].center().x() + spacing * 0.5, rects[ Txt ].bottom() + spacing } );
    }
    else if ( layout == ( Qt::AlignBottom | Qt::AlignHCenter ) )
    {
        rects[ Txt ].moveTopLeft(
            { center.x() - rects[ Txt ].width() * 0.5, center.y() - rects[ Txt ].height() * 0.5 } );
        rects[ Dec ].moveTopLeft( { center.x() - spacing * 0.5 - rects[ Dec ].width(),
            rects[ Txt ].top() - spacing - rects[ Dec ].height() } );
        rects[ Inc ].moveTopLeft(
            { center.x() + spacing * 0.5, rects[ Txt ].top() - spacing - rects[ Inc ].height() } );
    }

    if ( subControl == Q::DecrementPanel )
        return rects[ Dec ];

    if ( subControl == Q::TextPanel )
        return rects[ Txt ];

    if ( subControl == Q::IncrementPanel )
        return rects[ Inc ];

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskSpinBoxSkinlet::updateSubNode(
    const QskSkinnable* skinnable, const quint8 nodeRole, QSGNode* const node ) const
{
    using Q = QskSpinBox;

    switch( nodeRole )
    {
        case IncrementPanel:
            return updateSeriesNode( skinnable, Q::IncrementPanel, node );

        case DecrementPanel:
            return updateSeriesNode( skinnable, Q::DecrementPanel, node );

        case IncrementIndicator:
        {
            return updateTextNode( skinnable, node,
                QStringLiteral( "+" ), Q::IncrementIndicator );
        }

        case DecrementIndicator:
        {
            return updateTextNode( skinnable, node,
                QStringLiteral( "-" ), Q::DecrementIndicator );
        }

        case TextPanel:
            return updateSeriesNode( skinnable, Q::TextPanel, node );

        case Text:
        {
            const auto* const spinbox = static_cast< const QskSpinBox* >( skinnable );

            return updateTextNode( skinnable, node,
                QString::number( spinbox->value() ), Q::Text );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskSpinBoxSkinlet::updateSampleNode( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, const int index, QSGNode* const node ) const
{
    using Q = QskSpinBox;
    const auto* const spinbox = static_cast< const QskSpinBox* >( skinnable );

    if ( subControl == Q::DecrementPanel || subControl == Q::IncrementPanel ||
         subControl == Q::TextPanel )
    {
        const auto rect = sampleRect( spinbox, spinbox->contentsRect(), subControl, index );
        return updateBoxNode( skinnable, node, rect, subControl );
    }

    return Inherited::updateSampleNode( skinnable, subControl, index, node );
}
