/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskScrollViewSkinlet.h"
#include "QskScrollView.h"
#include "QskAspect.h"
#include "QskFrameNode.h"
#include "QskClipNode.h"
#include "QskFunctions.h"

#include <QTransform>

static void qskAlignedHandle( qreal start, qreal end,
    qreal scrollBarLength, qreal minHandleLength,
    qreal& handleStart, qreal& handleEnd )
{
    minHandleLength = qBound( 4.0, minHandleLength, scrollBarLength );

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

QskScrollViewSkinlet::QskScrollViewSkinlet( QskSkin* skin ):
    Inherited( skin )
{
    setNodeRoles( { ViewportRole, ContentsRootRole,
        HorizontalScrollBarRole, HorizontalScrollHandleRole,
        VerticalScrollBarRole, VerticalScrollHandleRole } );
}

QskScrollViewSkinlet::~QskScrollViewSkinlet() = default;

QRectF QskScrollViewSkinlet::subControlRect(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    const auto scrollView = static_cast< const QskScrollView* >( skinnable );

    if ( subControl == QskScrollView::Panel )
        return scrollView->contentsRect();

    if ( subControl == QskScrollView::Viewport )
        return viewportRect( scrollView );

    if ( subControl == QskScrollView::HorizontalScrollBar )
        return scrollBarRect( scrollView, Qt::Horizontal );

    if ( subControl == QskScrollView::HorizontalScrollHandle )
        return scrollHandleRect( scrollView, Qt::Horizontal );

    if ( subControl == QskScrollView::VerticalScrollBar )
        return scrollBarRect( scrollView, Qt::Vertical );

    if ( subControl == QskScrollView::VerticalScrollHandle )
        return scrollHandleRect( scrollView, Qt::Vertical );

    return Inherited::subControlRect( skinnable, subControl );
}

QSGNode* QskScrollViewSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto scrollView = static_cast< const QskScrollView* >( skinnable );

    switch( nodeRole )
    {
        case ViewportRole:
            return updateViewportNode( scrollView, node );

        case HorizontalScrollHandleRole:
            return updateBoxNode( skinnable, node, QskScrollView::HorizontalScrollHandle );

        case VerticalScrollHandleRole:
            return updateBoxNode( skinnable, node, QskScrollView::VerticalScrollHandle );

        case ContentsRootRole:
            return updateContentsRootNode( scrollView, node );

        case HorizontalScrollBarRole:
        case VerticalScrollBarRole:
            return nullptr;
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskScrollViewSkinlet::updateViewportNode(
    const QskScrollView* scrollView, QSGNode* node ) const
{
    auto frameNode = static_cast< QskFrameNode* >( node );
    if ( frameNode == nullptr )
        frameNode = new QskFrameNode();

    const QColor fill = scrollView->color( QskScrollView::Viewport | QskAspect::Background );

    const QColor dark = fill.darker( 150 );
    const QColor light = fill.lighter( 150 );

    const qreal radiusX = scrollView->metric( QskScrollView::Viewport | QskAspect::RadiusX );
    const qreal border = scrollView->metric( QskScrollView::Viewport | QskAspect::Border );

    frameNode->setColors( dark, fill, light );

    frameNode->setBorderWidth( border );
    frameNode->setShadeFactor( -1.0 );
    frameNode->setRect( subControlRect( scrollView, QskScrollView::Viewport ) );

    frameNode->setRadius( radiusX );

    frameNode->update();

    return frameNode;
}

QSGNode* QskScrollViewSkinlet::updateContentsRootNode(
    const QskScrollView* scrollView, QSGNode* node ) const
{
    const qreal radiusX = scrollView->metric( QskScrollView::Viewport | QskAspect::RadiusX );
    const qreal border = scrollView->metric( QskScrollView::Viewport | QskAspect::Border );

    auto* clipNode = static_cast< QskClipNode* >( node );
    if ( clipNode == nullptr )
        clipNode = new QskClipNode();

    clipNode->setRect( scrollView->viewContentsRect() );
    clipNode->setRadius( radiusX - border );
    clipNode->update();

    QSGNode* oldContentsNode = findNodeByRole( clipNode, ContentsRootRole );
    QSGNode* contentsNode = updateContentsNode( scrollView, oldContentsNode );

    if ( contentsNode )
    {
        /*
            Not all classes derived from QskScrollView create their content node
            by updateContentsNode(). F.e QskScrollArea takes the itemNode from
            another item and puts it below the clip node.
            For those situations we need to set a node role, that we can decide
            which child of the clip node needs to be replaced.
         */
        setNodeRole( contentsNode, ContentsRootRole );

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

QSGNode* QskScrollViewSkinlet::contentsNode( const QskScrollView* scrollView )
{
    QSGNode* node = const_cast< QSGNode* >( QskControl::paintNode( scrollView ) );
    if ( node )
    {
        node = findNodeByRole( node, ContentsRootRole );
        if ( node )
        {
            node = node->firstChild();
            if ( node )
                return node->firstChild();
        }
    }

    return nullptr;
}

QRectF QskScrollViewSkinlet::viewportRect( const QskScrollView* scrollView ) const
{
    const Qt::Orientations orientation = scrollView->scrollableOrientations();

    QRectF vr = subControlRect( scrollView, QskScrollView::Panel );
    const qreal spacing = scrollView->metric( QskScrollView::Panel | QskAspect::Spacing );

    if ( orientation & Qt::Vertical )
    {
        const QRectF r = subControlRect( scrollView, QskScrollView::VerticalScrollBar );
        if ( r.width() > 0.0 )
            vr.setWidth( vr.width() - r.width() - spacing );
    }

    if ( orientation & Qt::Horizontal )
    {
        const QRectF r = subControlRect( scrollView, QskScrollView::HorizontalScrollBar );
        if ( r.height() >= 0.0 )
            vr.setHeight( vr.height() - r.height() - spacing );
    }

    return vr;
}

QRectF QskScrollViewSkinlet::scrollHandleRect(
    const QskScrollView* scrollView, Qt::Orientation orientation ) const
{
    const Qt::Orientations scrollOrientations = scrollView->scrollableOrientations();
    if ( !( orientation & scrollOrientations ) )
        return QRectF();

    const QPointF pos = scrollView->scrollPos();
    const QRectF vRect = subControlRect( scrollView, QskScrollView::Viewport );

    QRectF handleRect;

    if ( orientation == Qt::Vertical )
    {
        const auto subControlBar = QskScrollView::VerticalScrollBar;

        const QRectF sbRect = subControlRect( scrollView, subControlBar );
        const QMarginsF padding = scrollView->marginsHint( subControlBar | QskAspect::Padding );

        const qreal h = scrollView->scrollableSize().height();

        const qreal y1 = pos.y() / h;
        const qreal y2 = ( pos.y() + vRect.height() ) / h;

        const qreal minHandleLength =
            scrollView->metric( QskScrollView::VerticalScrollHandle | QskAspect::MinimumHeight );

        qreal top, bottom;
        qskAlignedHandle( y1, y2, sbRect.height(), minHandleLength, top, bottom );

        handleRect = sbRect;
        handleRect.setTop( sbRect.y() + top );
        handleRect.setBottom( sbRect.y() + bottom );
        handleRect.adjust( padding.left(), 0, -padding.right(), 0 );
    }
    else
    {
        const auto subControlBar = QskScrollView::HorizontalScrollBar;

        const QRectF sbRect = subControlRect( scrollView, subControlBar );
        const auto padding = scrollView->marginsHint( subControlBar | QskAspect::Padding );

        const qreal w = scrollView->scrollableSize().width();

        const qreal x1 = pos.x() / w;
        const qreal x2 = ( pos.x() + vRect.width() ) / w;

        const qreal minHandleLength =
            scrollView->metric( QskScrollView::HorizontalScrollHandle | QskAspect::MinimumWidth );

        qreal left, right;
        qskAlignedHandle( x1, x2, sbRect.width(), minHandleLength, left, right );

        handleRect = sbRect;
        handleRect.setLeft( sbRect.x() + left );
        handleRect.setRight( sbRect.x() + right );
        handleRect.adjust( 0, padding.top(), 0, -padding.bottom() );
    }

    return handleRect;
}

QRectF QskScrollViewSkinlet::scrollBarRect(
    const QskScrollView* scrollView, Qt::Orientation orientation ) const
{
    const Qt::Orientations scrollOrientations = scrollView->scrollableOrientations();
    if ( !( orientation & scrollOrientations ) )
        return QRectF();

    QRectF r = subControlRect( scrollView, QskScrollView::Panel );

    if ( orientation == Qt::Horizontal )
    {
        const qreal h = scrollView->metric(
            QskScrollView::HorizontalScrollBar | QskAspect::Size );

        r.setTop( r.bottom() - h );

        if ( scrollOrientations & Qt::Vertical )
        {
            const qreal w = scrollView->metric(
                QskScrollView::VerticalScrollBar | QskAspect::Size );

            r.setRight( r.right() - w );
        }
    }
    else
    {
        const qreal w = scrollView->metric(
            QskScrollView::VerticalScrollBar | QskAspect::Size );

        r.setLeft( r.right() - w );

        if ( scrollOrientations & Qt::Horizontal )
        {
            const qreal h = scrollView->metric(
                QskScrollView::HorizontalScrollBar | QskAspect::Size );

            r.setBottom( r.bottom() - h );
        }
    }

    return r;
}

#include "moc_QskScrollViewSkinlet.cpp"
