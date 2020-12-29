/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSeparatorSkinlet.h"
#include "QskSeparator.h"

#include "QskAspect.h"

QskSeparatorSkinlet::QskSeparatorSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole } );
}

QskSeparatorSkinlet::~QskSeparatorSkinlet() = default;

QRectF QskSeparatorSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto separator = static_cast< const QskSeparator* >( skinnable );

    if ( subControl == QskSeparator::Panel )
    {
        return panelRect( separator, contentsRect );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskSeparatorSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto separator = static_cast< const QskSeparator* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            return updateBoxNode( separator, node, QskSeparator::Panel );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRectF QskSeparatorSkinlet::panelRect(
    const QskSeparator* separator, const QRectF& contentsRect ) const
{
    const qreal extent = separator->extent();

    QRectF r;

    if ( separator->orientation() == Qt::Horizontal )
    {
        r.setWidth( contentsRect.width() );
        r.setHeight( extent );
    }
    else
    {
        r.setHeight( contentsRect.height() );
        r.setWidth( extent );
    }

    r.moveCenter( contentsRect.center() );
    return r;
}

QSizeF QskSeparatorSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto separator = static_cast< const QskSeparator* >( skinnable );

    const qreal extent = separator->extent();

    if ( separator->orientation() == Qt::Horizontal )
        return QSizeF( -1, extent );
    else
        return QSizeF( extent, -1 );
}

#include "moc_QskSeparatorSkinlet.cpp"
