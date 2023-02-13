/******************************************************************************
 * Copyright (C) 2022 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "StorageBarSkinlet.h"
#include "StorageBar.h"

using S = StorageBar;

StorageBarSkinlet::StorageBarSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { Pictures, Music, Videos, Documents, Others, Free } );
}

QRectF StorageBarSkinlet::subControlRect( const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl ) const
{
    const auto* const bar = static_cast< const S* >( skinnable );

    auto x = contentsRect.x();
    const auto y = contentsRect.y();
    const auto w = contentsRect.width();
    const auto h = contentsRect.height();

    // segement widths
    const auto p = w * bar->pictures();
    const auto m = w * bar->music();
    const auto v = w * bar->videos();
    const auto d = w * bar->documents();
    const auto o = w * bar->others();
    const auto f = w * bar->free();

    if ( subControl == S::Pictures )
    {
        return { x, y, p, h };
    }
    x += p;

    if ( subControl == S::Music )
    {
        return { x, y, m, h };
    }
    x += m;

    if ( subControl == S::Videos )
    {
        return { x, y, v, h };
    }
    x += v;

    if ( subControl == S::Documents )
    {
        return { x, y, d, h };
    }
    x += d;

    if ( subControl == S::Others )
    {
        return { x, y, o, h };
    }
    x += o;

    if ( subControl == S::Free )
    {
        return { x, y, f, h };
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

namespace
{
    inline QSGNode* updateSegmentBoxNode(
        const S* const skinnable, const QskAspect::Subcontrol& subcontrol, QSGNode* const node )
    {
        return QskSkinlet::updateBoxNode( skinnable, node, skinnable->subControlRect( subcontrol ),
            skinnable->gradientHint( subcontrol ), subcontrol );
    }
}

QSGNode* StorageBarSkinlet::updateSubNode(
    const QskSkinnable* const skinnable, const quint8 nodeRole, QSGNode* const node ) const
{
    const auto* const bar = static_cast< const S* >( skinnable );

    switch ( nodeRole )
    {
        case Pictures:
            return updateSegmentBoxNode( bar, S::Pictures, node );
        case Music:
            return updateSegmentBoxNode( bar, S::Music, node );
        case Videos:
            return updateSegmentBoxNode( bar, S::Videos, node );
        case Documents:
            return updateSegmentBoxNode( bar, S::Documents, node );
        case Others:
            return updateSegmentBoxNode( bar, S::Others, node );
        case Free:
            return updateSegmentBoxNode( bar, S::Free, node );
        default:
            return Inherited::updateSubNode( skinnable, nodeRole, node );
    }
}

#include "moc_StorageBarSkinlet.cpp"
