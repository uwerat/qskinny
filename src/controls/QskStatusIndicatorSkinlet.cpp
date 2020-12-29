/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskStatusIndicatorSkinlet.h"
#include "QskStatusIndicator.h"

#include "QskAspect.h"
#include "QskColorFilter.h"
#include "QskFunctions.h"
#include "QskGraphic.h"

QskStatusIndicatorSkinlet::QskStatusIndicatorSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { GraphicRole } );
}

QskStatusIndicatorSkinlet::~QskStatusIndicatorSkinlet() = default;

QRectF QskStatusIndicatorSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto label = static_cast< const QskStatusIndicator* >( skinnable );

    if ( subControl == QskStatusIndicator::Graphic )
    {
        return graphicRect( label, contentsRect );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskStatusIndicatorSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto indicator = static_cast< const QskStatusIndicator* >( skinnable );

    switch ( nodeRole )
    {
        case GraphicRole:
        {
            return updateGraphicNode( indicator, node );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRect QskStatusIndicatorSkinlet::graphicRect(
    const QskStatusIndicator* indicator, const QRectF& contentsRect ) const
{
    // maybe we should have a common base class for
    // QskStatusIndicator and QskGraphicLabel

    // PreserveAspectFit/PreserveAspectCrop

    const QRect graphicRect = contentsRect.toAlignedRect();

    QSizeF sz = indicator->graphic( indicator->status() ).defaultSize();
    sz.scale( graphicRect.size(), Qt::KeepAspectRatio );

    return qskAlignedRect( graphicRect,
        ( int ) sz.width(), ( int ) sz.height(), Qt::AlignCenter );
}

QSGNode* QskStatusIndicatorSkinlet::updateGraphicNode(
    const QskStatusIndicator* indicator, QSGNode* node ) const
{
    const auto subControl = QskStatusIndicator::Graphic;

    const auto rect = subControlRect( indicator, indicator->contentsRect(), subControl );
    const auto alignment = indicator->alignmentHint( subControl, Qt::AlignCenter );
    const int status = indicator->status();

    node = QskSkinlet::updateGraphicNode( indicator, node,
        indicator->graphic( status ), indicator->graphicFilter( status ),
        rect, alignment );

    return node;
}

QSizeF QskStatusIndicatorSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto indicator = static_cast< const QskStatusIndicator* >( skinnable );

    QSizeF sz;

    for ( const auto status : indicator->statusList() )
    {
        const auto graphic = indicator->graphic( status );

        if ( !graphic.isEmpty() )
        {
            auto hint = graphic.defaultSize();

            if ( !hint.isEmpty() )
            {
                if ( constraint.width() >= 0.0 )
                {
                    hint.setHeight( sz.height() * constraint.width() / sz.width() );
                }
                else if ( constraint.height() >= 0.0 )
                {
                    hint.setWidth( sz.width() * constraint.height() / sz.height() );
                }
            }

            sz = sz.expandedTo( hint );
        }
    }

    return sz;
}

#include "moc_QskStatusIndicatorSkinlet.cpp"
