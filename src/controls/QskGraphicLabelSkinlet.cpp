/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGraphicLabelSkinlet.h"
#include "QskGraphicLabel.h"

#include "QskAspect.h"
#include "QskColorFilter.h"
#include "QskFunctions.h"
#include "QskGraphic.h"
#include "QskTextureNode.h"

QskGraphicLabelSkinlet::QskGraphicLabelSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { GraphicRole } );
}

QskGraphicLabelSkinlet::~QskGraphicLabelSkinlet() = default;

QRectF QskGraphicLabelSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto label = static_cast< const QskGraphicLabel* >( skinnable );

    if ( subControl == QskGraphicLabel::Graphic )
    {
        return graphicRect( label, contentsRect );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskGraphicLabelSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto label = static_cast< const QskGraphicLabel* >( skinnable );

    switch ( nodeRole )
    {
        case GraphicRole:
        {
            return updateGraphicNode( label, node );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRect QskGraphicLabelSkinlet::graphicRect(
    const QskGraphicLabel* label, const QRectF& contentsRect ) const
{
    // textures are in integers, to avoid useless recalculations
    // that finally will be rounded anyway, we calculate in integers

    const auto fillMode = label->fillMode();

    const QRect graphicRect = contentsRect.toAlignedRect();

    if ( fillMode == QskGraphicLabel::Stretch )
    {
        return graphicRect;
    }

    QSizeF sz = label->effectiveSourceSize();

    if ( fillMode == QskGraphicLabel::PreserveAspectFit )
    {
        sz.scale( graphicRect.size(), Qt::KeepAspectRatio );
    }
    else if ( fillMode == QskGraphicLabel::PreserveAspectCrop )
    {
        sz.scale( graphicRect.size(), Qt::KeepAspectRatioByExpanding );
    }

    return qskAlignedRect( graphicRect,
        ( int ) sz.width(), ( int ) sz.height(), label->alignment() );
}

QSGNode* QskGraphicLabelSkinlet::updateGraphicNode(
    const QskGraphicLabel* label, QSGNode* node ) const
{
    const auto colorFilter = label->graphicFilter();
    const auto rect = label->subControlRect( QskGraphicLabel::Graphic );

    Qt::Orientations mirrored;
    if ( label->mirror() )
        mirrored = Qt::Horizontal;

    if ( label->fillMode() == QskGraphicLabel::Stretch )
    {
        node = QskSkinlet::updateGraphicNode( label, node,
            label->graphic(), colorFilter, rect, mirrored );
    }
    else
    {
        node = QskSkinlet::updateGraphicNode( label, node,
            label->graphic(), colorFilter, rect, Qt::AlignCenter, mirrored );
    }

    return node;
}

QSizeF QskGraphicLabelSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto label = static_cast< const QskGraphicLabel* >( skinnable );

    auto sz = label->effectiveSourceSize();

    if ( !sz.isEmpty() )
    {
        if ( constraint.width() >= 0.0 )
        {
            sz.setHeight( sz.height() * constraint.width() / sz.width() );
            sz.setWidth( -1.0 );
        }
        else if ( constraint.height() >= 0.0 )
        {
            sz.setWidth( sz.width() * constraint.height() / sz.height() );
            sz.setHeight( -1.0 );
        }
    }

    return sz;
}

#include "moc_QskGraphicLabelSkinlet.cpp"
