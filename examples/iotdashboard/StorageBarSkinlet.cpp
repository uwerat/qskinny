/******************************************************************************
 * Copyright (C) 2022 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "StorageBarSkinlet.h"
#include "StorageBar.h"

using S = StorageBar;

StorageBarSkinlet::StorageBarSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { Pictures, Music, Videos, Documents, Others, Free } );
}

QRectF StorageBarSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
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

QSGNode* StorageBarSkinlet::updateSubNode(
    const QskSkinnable* const skinnable, const quint8 nodeRole, QSGNode* const node ) const
{
    switch ( nodeRole )
    {
        case Pictures:
            return updateBoxNode( skinnable, node, S::Pictures );

        case Music:
            return updateBoxNode( skinnable, node, S::Music );

        case Videos:
            return updateBoxNode( skinnable, node, S::Videos );

        case Documents:
            return updateBoxNode( skinnable, node, S::Documents );

        case Others:
            return updateBoxNode( skinnable, node, S::Others );

        case Free:
            return updateBoxNode( skinnable, node, S::Free );

        default:
            return Inherited::updateSubNode( skinnable, nodeRole, node );
    }
}

#include "moc_StorageBarSkinlet.cpp"
