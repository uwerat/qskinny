/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTabViewSkinlet.h"
#include "QskTabView.h"
#include "QskTabBar.h"

QskTabViewSkinlet::QskTabViewSkinlet( QskSkin* skin ):
    Inherited( skin )
{
    setNodeRoles( { PageRole } );
}

QskTabViewSkinlet::~QskTabViewSkinlet() = default;

QRectF QskTabViewSkinlet::subControlRect(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    const auto tabView = static_cast< const QskTabView* >( skinnable );

    if ( subControl == QskTabView::Page )
        return pageRect( tabView );

    if ( subControl == QskTabView::TabBar )
        return tabBarRect( tabView );

    return Inherited::subControlRect( skinnable, subControl );
}

QRectF QskTabViewSkinlet::pageRect( const QskTabView* tabView ) const
{
    const QRectF barRect = subControlRect( tabView, QskTabView::TabBar );

    QRectF r = tabView->contentsRect();

    if ( tabView->orientation() == Qt::Vertical )
    {
        r.setTop( barRect.bottom() );
    }
    else
    {
        if ( tabView->layoutMirroring() )
            r.setRight( r.right() - barRect.left() );
        else
            r.setLeft( barRect.right() );
    }

    return r;
}

QRectF QskTabViewSkinlet::tabBarRect( const QskTabView* tabView ) const
{
    QRectF r = tabView->layoutRect();
    const QSizeF hint = tabView->tabBar()->sizeHint();

    if ( tabView->orientation() == Qt::Vertical )
    {
        r.setHeight( hint.height() );
    }
    else
    {
        r.setWidth( hint.width() );

        if ( tabView->layoutMirroring() )
            r.moveLeft( r.right() - r.width() );
    }

    return r;
}

QSGNode* QskTabViewSkinlet::updateSubNode( const QskSkinnable* skinnable,
    quint8 nodeRole, QSGNode* node ) const
{
    const auto tabView = static_cast< const QskTabView* >( skinnable );

    switch( nodeRole )
    {
        case PageRole:
            return updatePageNode( tabView, node );

        default:
            return nullptr;
    }
}

QSGNode* QskTabViewSkinlet::updatePageNode(
    const QskTabView* tabView, QSGNode* node ) const
{
    return updateBoxNode( tabView, node, QskTabView::Page );
}

#include "moc_QskTabViewSkinlet.cpp"
