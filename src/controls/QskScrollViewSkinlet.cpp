/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskScrollViewSkinlet.h"
#include "QskScrollView.h"

#include "QskAspect.h"
#include "QskQuick.h"
#include "QskSGNode.h"
#include "QskSkinStateChanger.h"

#include <qsgnode.h>

static void qskAlignedHandle( qreal start, qreal end,
    qreal scrollBarLength, qreal minHandleLength,
    qreal& handleStart, qreal& handleEnd )
{
    // no qBound: scrollBarLength might be < 4.0
    minHandleLength = qMax( 4.0, minHandleLength );
    minHandleLength = qMin( minHandleLength, scrollBarLength );

    handleStart = start * scrollBarLength;
    handleEnd = end * scrollBarLength;

    const qreal handleLength = handleEnd - handleStart;
    if ( handleLength < minHandleLength )
    {
        const qreal extra = minHandleLength - handleLength;

        handleStart -= extra * handleStart / scrollBarLength;
        handleEnd += extra * ( ( scrollBarLength - handleEnd ) / scrollBarLength );
    }
    else if ( handleLength > scrollBarLength )
    {
        const qreal extra = scrollBarLength - handleLength;

        handleStart -= extra * handleStart / scrollBarLength;
        handleEnd += extra * ( ( scrollBarLength - handleEnd ) / scrollBarLength );
    }
}

static qreal qskScrollBarExtent(
    const QskScrollView* scrollView, Qt::Orientation orientation )
{
    const auto subControl = ( orientation == Qt::Horizontal )
        ? QskScrollView::HorizontalScrollBar : QskScrollView::VerticalScrollBar;

    QskSkinStateChanger stateChanger( scrollView );
    stateChanger.setStates( scrollView->scrollBarStates( subControl ) );

    return scrollView->metric( subControl | QskAspect::Size );
}

QskScrollViewSkinlet::QskScrollViewSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, ViewportRole, ContentsRootRole,
        HorizontalScrollBarRole, HorizontalScrollHandleRole,
        VerticalScrollBarRole, VerticalScrollHandleRole } );
}

QskScrollViewSkinlet::~QskScrollViewSkinlet() = default;

QRectF QskScrollViewSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    using Q = QskScrollView;

    const auto scrollView = static_cast< const QskScrollView* >( skinnable );

    if ( subControl == Q::Panel )
        return contentsRect;

    if ( subControl == Q::Viewport )
        return viewportRect( scrollView, contentsRect );

    if ( subControl == Q::HorizontalScrollBar )
        return scrollBarRect( scrollView, contentsRect, Qt::Horizontal );

    if ( subControl == Q::HorizontalScrollHandle )
        return scrollHandleRect( scrollView, contentsRect, Qt::Horizontal );

    if ( subControl == Q::VerticalScrollBar )
        return scrollBarRect( scrollView, contentsRect, Qt::Vertical );

    if ( subControl == Q::VerticalScrollHandle )
        return scrollHandleRect( scrollView, contentsRect, Qt::Vertical );

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskScrollViewSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    using Q = QskScrollView;

    const auto scrollView = static_cast< const QskScrollView* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
            return updateBoxNode( skinnable, node, Q::Panel );

        case ViewportRole:
            return updateBoxNode( skinnable, node, Q::Viewport );

        case HorizontalScrollHandleRole:
            return updateScrollBarNode( scrollView, Q::HorizontalScrollHandle, node );

        case VerticalScrollHandleRole:
            return updateScrollBarNode( scrollView, Q::VerticalScrollHandle, node );

        case HorizontalScrollBarRole:
            return updateScrollBarNode( scrollView, Q::HorizontalScrollBar, node );

        case VerticalScrollBarRole:
            return updateScrollBarNode( scrollView, Q::VerticalScrollBar, node );

        case ContentsRootRole:
            return updateContentsRootNode( scrollView, node );

    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskScrollViewSkinlet::updateScrollBarNode( const QskScrollView* scrollView,
    QskAspect::Subcontrol subControl, QSGNode* node ) const
{
    const auto rect = subControlRect( scrollView,
        scrollView->contentsRect(), subControl );
    
    QskSkinStateChanger stateChanger( scrollView );
    stateChanger.setStates( scrollView->scrollBarStates( subControl ) );

    return updateBoxNode( scrollView, node, rect, subControl );
}

QSGNode* QskScrollViewSkinlet::updateContentsRootNode(
    const QskScrollView* scrollView, QSGNode* node ) const
{
    auto* clipNode = updateBoxClipNode( scrollView, node, QskScrollView::Viewport );
    if ( clipNode == nullptr )
        return nullptr;

    auto oldContentsNode = QskSGNode::findChildNode( clipNode, ContentsRootRole );
    auto contentsNode = updateContentsNode( scrollView, oldContentsNode );

    if ( contentsNode )
    {
        /*
            Not all classes derived from QskScrollView create their content node
            by updateContentsNode(). F.e QskScrollArea takes the itemNode from
            another item and puts it below the clip node.
            For those situations we need to set a node role, that we can decide
            which child of the clip node needs to be replaced.
         */
        QskSGNode::setNodeRole( contentsNode, ContentsRootRole );

        if ( contentsNode->parent() != clipNode )
            clipNode->appendChildNode( contentsNode );
    }

    if ( oldContentsNode && oldContentsNode != contentsNode )
    {
        clipNode->removeChildNode( oldContentsNode );

        if ( oldContentsNode->flags() & QSGNode::OwnedByParent )
            delete oldContentsNode;
    }

    return clipNode;
}

QSGNode* QskScrollViewSkinlet::updateContentsNode(
    const QskScrollView*, QSGNode* ) const
{
    return nullptr;
}

QRectF QskScrollViewSkinlet::viewportRect(
    const QskScrollView* scrollView, const QRectF& contentsRect ) const
{
    using Q = QskScrollView;

    const auto orientation = scrollView->scrollableOrientations();

    auto vr = subControlRect( scrollView, contentsRect, Q::Panel );
    const qreal spacing = scrollView->spacingHint( Q::Panel );

    if ( orientation & Qt::Vertical )
    {
        const auto r = subControlRect(
            scrollView, contentsRect, Q::VerticalScrollBar );

        if ( r.width() > 0.0 )
            vr.setWidth( vr.width() - r.width() - spacing );
    }

    if ( orientation & Qt::Horizontal )
    {
        const auto r = subControlRect(
            scrollView, contentsRect, Q::HorizontalScrollBar );

        if ( r.height() >= 0.0 )
            vr.setHeight( vr.height() - r.height() - spacing );
    }

    if ( vr.width() < 0.0 )
        vr.setWidth( 0.0 );

    if ( vr.height() < 0.0 )
        vr.setHeight( 0.0 );

    return vr;
}

QRectF QskScrollViewSkinlet::scrollHandleRect( const QskScrollView* scrollView,
    const QRectF& contentsRect, Qt::Orientation orientation ) const
{
    using Q = QskScrollView;

    const auto scrollOrientations = scrollView->scrollableOrientations();
    if ( !( orientation & scrollOrientations ) )
        return QRectF();

    const auto pos = scrollView->scrollPos();

    const auto vRect = subControlRect(
        scrollView, contentsRect, QskScrollView::Viewport );

    const auto scrollableSize = scrollView->scrollableSize();

    QRectF handleRect;

    if ( orientation == Qt::Vertical )
    {
        const auto sbRect = subControlRect( scrollView, contentsRect, Q::VerticalScrollBar );

        QskSkinStateChanger stateChanger( scrollView );
        stateChanger.setStates( scrollView->scrollBarStates( Q::VerticalScrollBar ) );

        const auto padding = scrollView->paddingHint( Q::VerticalScrollBar );
        const auto strut = scrollView->strutSizeHint( Q::VerticalScrollHandle );

        const qreal y1 = pos.y() / scrollableSize.height();
        const qreal y2 = ( pos.y() + vRect.height() ) / scrollableSize.height();

        qreal top, bottom;
        qskAlignedHandle( y1, y2, sbRect.height(), strut.height(), top, bottom );

        handleRect = sbRect;
        handleRect.setTop( sbRect.y() + top );
        handleRect.setBottom( sbRect.y() + bottom );
        handleRect.adjust( padding.left(), 0, -padding.right(), 0 );
    }
    else
    {
        const auto sbRect = subControlRect( scrollView, contentsRect, Q::HorizontalScrollBar );

        QskSkinStateChanger stateChanger( scrollView );
        stateChanger.setStates( scrollView->scrollBarStates( Q::HorizontalScrollBar ) );

        const auto padding = scrollView->paddingHint( Q::HorizontalScrollBar );

        const qreal x1 = pos.x() / scrollableSize.width();
        const qreal x2 = ( pos.x() + vRect.width() ) / scrollableSize.width();

        const auto strut = scrollView->strutSizeHint( Q::HorizontalScrollHandle );

        qreal left, right;
        qskAlignedHandle( x1, x2, sbRect.width(), strut.width(), left, right );

        handleRect = sbRect;
        handleRect.setLeft( sbRect.x() + left );
        handleRect.setRight( sbRect.x() + right );
        handleRect.adjust( 0, padding.top(), 0, -padding.bottom() );
    }

    return handleRect;
}

QRectF QskScrollViewSkinlet::scrollBarRect( const QskScrollView* scrollView,
    const QRectF& contentsRect, Qt::Orientation orientation ) const
{
    using Q = QskScrollView;

    const auto scrollOrientations = scrollView->scrollableOrientations();
    if ( !( orientation & scrollOrientations ) )
        return QRectF();

    auto r = subControlRect( scrollView, contentsRect, Q::Panel );

    if ( orientation == Qt::Horizontal )
    {
        const qreal h = qskScrollBarExtent( scrollView, Qt::Horizontal );
        r.setTop( r.bottom() - h );

        if ( scrollOrientations & Qt::Vertical )
        {
            const qreal w = qskScrollBarExtent( scrollView, Qt::Vertical );
            r.setRight( r.right() - w );
        }
    }
    else
    {
        const qreal w = qskScrollBarExtent( scrollView, Qt::Vertical );
        r.setLeft( r.right() - w );

        if ( scrollOrientations & Qt::Horizontal )
        {
            const qreal h = qskScrollBarExtent( scrollView, Qt::Horizontal );
            r.setBottom( r.bottom() - h );
        }
    }

    return r;
}

#include "moc_QskScrollViewSkinlet.cpp"
