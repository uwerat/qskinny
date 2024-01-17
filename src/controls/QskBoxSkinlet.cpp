/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskBoxSkinlet.h"
#include "QskBox.h"

QskBoxSkinlet::QskBoxSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole } );
}

QskBoxSkinlet::~QskBoxSkinlet()
{
}

QRectF QskBoxSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskBox::Panel )
    {
        return contentsRect;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskBoxSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto box = static_cast< const QskBox* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            if ( !box->hasPanel() )
                return nullptr;

            return updateBoxNode( skinnable, node, QskBox::Panel );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSizeF QskBoxSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    const auto box = static_cast< const QskBox* >( skinnable );

    if ( box->hasPanel() && which == Qt::PreferredSize )
        return box->strutSizeHint( QskBox::Panel );

    return Inherited::sizeHint( skinnable, which, constraint );
}

#include "moc_QskBoxSkinlet.cpp"
