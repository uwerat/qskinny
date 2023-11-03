/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskDrawerSkinlet.h"
#include "QskDrawer.h"

QskDrawerSkinlet::QskDrawerSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    appendNodeRoles( { PanelRole } );
}

QskDrawerSkinlet::~QskDrawerSkinlet() = default;

QRectF QskDrawerSkinlet::subControlRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskDrawer::Panel )
        return contentsRect;

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskDrawerSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    if ( nodeRole == PanelRole )
        return updateBoxNode( skinnable, node, QskDrawer::Panel );

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSizeF QskDrawerSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which == Qt::PreferredSize )
        return skinnable->strutSizeHint( QskDrawer::Panel );

    return Inherited::sizeHint( skinnable, which, constraint );
}

#include "moc_QskDrawerSkinlet.cpp"
