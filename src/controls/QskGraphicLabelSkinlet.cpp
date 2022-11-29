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

#include <QtMath>

QskGraphicLabelSkinlet::QskGraphicLabelSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, GraphicRole } );
}

QskGraphicLabelSkinlet::~QskGraphicLabelSkinlet() = default;

QRectF QskGraphicLabelSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskGraphicLabel::Panel )
    {
        return contentsRect;
    }
    else if ( subControl == QskGraphicLabel::Graphic )
    {
        auto innerRect = contentsRect;

        const auto label = static_cast< const QskGraphicLabel* >( skinnable );

        if ( label->hasPanel() )
        {
            innerRect = label->subControlContentsRect(
                innerRect, QskGraphicLabel::Panel );
        }

        return graphicRect( label, innerRect );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskGraphicLabelSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto label = static_cast< const QskGraphicLabel* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            if ( !label->hasPanel() )
                return nullptr;

            return updateBoxNode( label, node, QskGraphicLabel::Panel );
        }
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
    using Q = QskGraphicLabel;

    // textures are in integers, to avoid useless recalculations
    // that finally will be rounded anyway, we calculate in integers

    const auto fillMode = label->fillMode();

    const QRect graphicRect = contentsRect.toAlignedRect();

    if ( fillMode == Q::Stretch )
    {
        return graphicRect;
    }

    QSizeF sz = label->effectiveSourceSize();

    if ( fillMode == Q::PreserveAspectFit )
    {
        sz.scale( graphicRect.size(), Qt::KeepAspectRatio );
    }
    else if ( fillMode == Q::PreserveAspectCrop )
    {
        sz.scale( graphicRect.size(), Qt::KeepAspectRatioByExpanding );
    }

    return qskAlignedRect( graphicRect,
        qCeil( sz.width() ), qCeil( sz.height() ), label->alignment() );
}

QSGNode* QskGraphicLabelSkinlet::updateGraphicNode(
    const QskGraphicLabel* label, QSGNode* node ) const
{
    using Q = QskGraphicLabel;

    const auto colorFilter = label->graphicFilter();
    const auto rect = label->subControlRect( Q::Graphic );

    Qt::Orientations mirrored;
    if ( label->mirror() )
        mirrored = Qt::Horizontal;

    if ( label->fillMode() == Q::Stretch )
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
    using Q = QskGraphicLabel;

    if ( which != Qt::PreferredSize )
        return QSizeF();

    const bool hasConstraint =
        ( constraint.width() >= 0.0 ) || ( constraint.height() >= 0.0 );

    const auto label = static_cast< const QskGraphicLabel* >( skinnable );
    const auto sourceSize = label->effectiveSourceSize();

    auto hint = sourceSize;

    if ( hasConstraint && !sourceSize.isEmpty() )
    {
        auto innerConstraint = constraint;

        if ( label->hasPanel() )
        {
            constexpr qreal max = std::numeric_limits< int >::max();

            QRectF r( 0.0, 0.0, max, max );

            if ( constraint.width() >= 0.0 )
                r.setWidth( constraint.width() );
            else
                r.setHeight( constraint.height() );

            innerConstraint = label->subControlContentsRect( r, Q::Panel ).size();
        }

        const qreal aspectRatio = sourceSize.width() / sourceSize.height();

        if ( constraint.width() >= 0.0 )
            hint.setHeight( innerConstraint.width() / aspectRatio );
        else
            hint.setWidth( innerConstraint.height() * aspectRatio );
    }

    hint = hint.expandedTo( label->strutSizeHint( Q::Graphic ) );

    if ( label->hasPanel() )
    {
        hint = label->outerBoxSize( Q::Panel, hint );
        hint = hint.expandedTo( label->strutSizeHint( Q::Panel ) );
    }

    if ( hasConstraint )
        hint = hintWithoutConstraint( hint, constraint );

    return hint;
}

#include "moc_QskGraphicLabelSkinlet.cpp"
