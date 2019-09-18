/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTabViewSkinlet.h"
#include "QskTabView.h"

#include "QskTabBar.h"

QskTabViewSkinlet::QskTabViewSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PageRole } );
}

QskTabViewSkinlet::~QskTabViewSkinlet() = default;

QRectF QskTabViewSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto tabView = static_cast< const QskTabView* >( skinnable );

    if ( subControl == QskTabView::Page )
    {
        return pageRect( tabView, contentsRect );
    }

    if ( subControl == QskTabView::TabBar )
    {
        return tabBarRect( tabView, contentsRect );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskTabViewSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    switch ( nodeRole )
    {
        case PageRole:
        {
            return updateBoxNode( skinnable, node, QskTabView::Page );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRectF QskTabViewSkinlet::pageRect(
    const QskTabView* tabView, const QRectF& rect ) const
{
    const QRectF barRect = subControlRect( tabView, rect, QskTabView::TabBar );

#if 1
    QRectF r = tabView->layoutRect();
#endif

    switch( tabView->tabPosition() )
    {
        case Qsk::Top:
            r.setTop( barRect.bottom() );
            break;

        case Qsk::Bottom:
            r.setBottom( barRect.top() );
            break;

        case Qsk::Left:
            r.setLeft( barRect.right() );
            break;

        case Qsk::Right:
            r.setRight( barRect.left() );
            break;
    }

    return r;
}

QRectF QskTabViewSkinlet::tabBarRect(
    const QskTabView* tabView, const QRectF& rect ) const
{
    Q_UNUSED( rect )

    const QSizeF barSize = tabView->tabBar()->sizeConstraint();

#if 1
    QRectF r = tabView->layoutRect();
#endif

    switch( tabView->tabPosition() )
    {
        case Qsk::Top:
            r.setHeight( barSize.height() );
            break;

        case Qsk::Bottom:
            r.setTop( r.bottom() - barSize.height() );
            break;

        case Qsk::Left:
            r.setWidth( barSize.width() );
            break;

        case Qsk::Right:
            r.setLeft( r.right() - barSize.width() );
            break;
    }

    return r;
}

#include "moc_QskTabViewSkinlet.cpp"
