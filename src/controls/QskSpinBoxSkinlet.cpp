/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSpinBoxSkinlet.h"
#include "QskSpinBox.h"

#include <array>

QskSpinBoxSkinlet::QskSpinBoxSkinlet( QskSkin* )
{
    setNodeRoles( { UpPanel, DownPanel, TextPanel,
        UpIndicator, DownIndicator, Text } );
}

QSizeF QskSpinBoxSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& size ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    using Q = QskSpinBox;

    const auto spinbox = static_cast< const QskSpinBox* >( skinnable );

    const auto layout = spinbox->alignmentHint( Q::Panel );
    const auto spacing = spinbox->spacingHint( Q::Panel );

    const auto strutInc = spinbox->strutSizeHint( Q::UpPanel );
    const auto strutDec = spinbox->strutSizeHint( Q::DownPanel );
    const auto strutTxt = spinbox->strutSizeHint( Q::TextPanel );

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
         layout == ( Qt::AlignBottom | Qt::AlignHCenter ) )
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

    if ( subControl == Q::DownIndicator )
        return skinnable->subControlContentsRect( contentsRect, Q::DownPanel );

    if ( subControl == Q::UpIndicator )
        return skinnable->subControlContentsRect( contentsRect, Q::UpPanel );

    if ( subControl == Q::Text )
        return skinnable->subControlContentsRect( contentsRect, Q::TextPanel );

    const auto* const spinbox = static_cast< const QskSpinBox* >( skinnable );

    const auto layout = spinbox->alignmentHint( Q::Panel );
    const auto spacing = spinbox->spacingHint( Q::Panel );

    enum
    {
        Dec = 0,
        Txt = 1,
        Inc = 2,
        Count
    };

    std::array< QRectF, Count > rects = {
        QRectF{ QPointF(), spinbox->strutSizeHint( Q::DownPanel ) },
        QRectF{ QPointF(), spinbox->strutSizeHint( Q::TextPanel ) },
        QRectF{ QPointF(), spinbox->strutSizeHint( Q::UpPanel ) },
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

    if ( subControl == Q::DownPanel )
        return rects[ Dec ];

    if ( subControl == Q::TextPanel )
        return rects[ Txt ];

    if ( subControl == Q::UpPanel )
        return rects[ Inc ];

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskSpinBoxSkinlet::updateSubNode(
    const QskSkinnable* skinnable, const quint8 nodeRole, QSGNode* const node ) const
{
    using Q = QskSpinBox;

    switch( nodeRole )
    {
        case UpPanel:
            return updateBoxNode( skinnable, node, Q::UpPanel );

        case DownPanel:
            return updateBoxNode( skinnable, node, Q::DownPanel );

        case UpIndicator:
        {
            return updateTextNode( skinnable, node,
                QStringLiteral( "+" ), Q::UpIndicator );
        }

        case DownIndicator:
        {
            return updateTextNode( skinnable, node,
                QStringLiteral( "-" ), Q::DownIndicator );
        }

        case TextPanel:
            return updateBoxNode( skinnable, node, Q::TextPanel );

        case Text:
        {
            const auto* const spinbox = static_cast< const QskSpinBox* >( skinnable );

            return updateTextNode( skinnable, node,
                QString::number( spinbox->value() ), Q::Text );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}
