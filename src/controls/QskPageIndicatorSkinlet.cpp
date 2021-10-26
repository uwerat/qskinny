/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPageIndicatorSkinlet.h"
#include "QskPageIndicator.h"

#include "QskBoxNode.h"
#include "QskSGNode.h"

QskPageIndicatorSkinlet::QskPageIndicatorSkinlet( QskSkin* skin )
    : QskSkinlet( skin )
{
    setNodeRoles( { PanelRole, BulletsRole } );
}

QskPageIndicatorSkinlet::~QskPageIndicatorSkinlet()
{
}

QRectF QskPageIndicatorSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskPageIndicator::Panel )
    {
        return contentsRect;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskPageIndicatorSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto indicator = static_cast< const QskPageIndicator* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            return updateBoxNode( indicator, node, QskPageIndicator::Panel );
        }

        case BulletsRole:
        {
            return updateBulletsNode( indicator, node );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRectF QskPageIndicatorSkinlet::bulletRect(
    const QskPageIndicator* indicator, const QRectF& rect, int index ) const
{
    using Q = QskPageIndicator;

    const auto szNormal = indicator->strutSizeHint( Q::Bullet );
    const auto szHighlighted = indicator->strutSizeHint( Q::Highlighted );

    const qreal wNormal = szNormal.width();
    const qreal wHighlighted = szHighlighted.width();

    const qreal hNormal = szNormal.height();
    const qreal hHighlighted = szHighlighted.height();

    const auto currentIndex = indicator->currentIndex();

    // scale bullet size if we are in between a transition:
    qreal indexDiff = qAbs( currentIndex - index );
    if ( indexDiff > ( indicator->count() - 1 ) )
        indexDiff = ( indicator->count() - currentIndex ); // wrapping

    const qreal w0 = ( indexDiff < 1 ) ?
        ( 1 - indexDiff ) * wHighlighted + indexDiff * wNormal : wNormal;

    const qreal h0 = ( indexDiff < 1 ) ?
        ( 1 - indexDiff ) * hHighlighted + indexDiff * hNormal : hNormal;

    const qreal spacing = indicator->spacingHint( Q::Panel );
    const bool horizontal = ( indicator->orientation() == Qt::Horizontal );

    qreal w, h;
    if ( horizontal )
    {
        w = ( indicator->count() - 1 ) * ( wNormal + spacing ) + wHighlighted;
        h = rect.height();
    }
    else
    {
        w = rect.width();
        h = ( indicator->count() - 1 ) * ( hNormal + spacing ) + hHighlighted;
    }

    QRectF r( 0, 0, w, h );
    r.moveCenter( rect.center() );

    qreal x2, y2;

    {
        const qreal w = ( index > currentIndex ) ? wHighlighted : wNormal;
        const qreal h = ( index > currentIndex ) ? hHighlighted : hNormal;

        if ( indexDiff < 1 && index >= currentIndex )
        {
            // scrolling from or to this bullet:
            x2 = wNormal + qAbs( wHighlighted - wNormal ) * indexDiff;
            y2 = hNormal + qAbs( hHighlighted - hNormal ) * indexDiff;
        }
        else if ( ( currentIndex > ( indicator->count() - 1 ) &&
            index > ( currentIndex - indicator->count() + 1 ) ) )
        {
            // wrapping case:
            qreal wrappingDiff = indexDiff;
            while ( wrappingDiff > 1 )
                wrappingDiff -= 1;

            x2 = wNormal + qAbs( wHighlighted - wNormal ) * wrappingDiff;
            y2 = hNormal + qAbs( hHighlighted - hNormal ) * wrappingDiff;
        }
        else
        {
            x2 = w;
            y2 = h;
        }
    }

    const qreal x = r.left() + x2 + spacing + ( index - 1 ) * ( wNormal + spacing );
    const qreal y = r.top() + y2 + spacing + ( index - 1 ) * ( hNormal + spacing );

    qreal adjust = ( currentIndex == index )
        ? ( wNormal - wHighlighted ) : ( wHighlighted - wNormal );
    adjust = 0.5 * qMax( 0.0, adjust );

    if ( indexDiff < 1 )
        adjust *= indexDiff;

    QRectF bulletRect( 0.0, 0.0, w0, h0 );

    if ( horizontal )
        bulletRect.moveTo( x, r.top() + adjust );
    else
        bulletRect.moveTo( r.left() + adjust, y );

    return bulletRect;
}

QSGNode* QskPageIndicatorSkinlet::updateBulletsNode(
    const QskPageIndicator* indicator, QSGNode* node ) const
{
    if ( indicator->count() == 0 )
        return nullptr;

    if ( node == nullptr )
        node = new QSGNode();

    const auto rect = indicator->subControlContentsRect( QskPageIndicator::Panel );

    // index of the highlighted bullet
    int currentBullet = qRound( indicator->currentIndex() );
    if ( currentBullet >= indicator->count() )
        currentBullet = 0;

    auto bulletNode = node->firstChild();
    for ( int i = 0; i < indicator->count(); i++ )
    {
        using Q = QskPageIndicator;

        if ( i > 0 )
            bulletNode = bulletNode->nextSibling();

        if ( bulletNode == nullptr )
            bulletNode = new QskBoxNode();

        updateBoxNode( indicator, bulletNode, bulletRect( indicator, rect, i ),
            ( i == currentBullet ) ? Q::Highlighted : Q::Bullet );

        if ( bulletNode->parent() != node )
            node->appendChildNode( bulletNode );
    }

    // if count has decreased we need to remove superfluous nodes
    QskSGNode::removeAllChildNodesAfter( node, bulletNode );

    return node;
}

QSizeF QskPageIndicatorSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    using Q = QskPageIndicator;

    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto indicator = static_cast< const QskPageIndicator* >( skinnable );

    const auto bulletSize = indicator->strutSizeHint( Q::Bullet );

    const auto maxSize = bulletSize.expandedTo(
        indicator->strutSizeHint( Q::Highlighted ) );

    const qreal spacing = indicator->spacingHint( Q::Panel );

    const int n = indicator->count();

    qreal w = 0;
    qreal h = 0;

    if ( indicator->orientation() == Qt::Horizontal )
    {
        if ( n > 0 )
        {
            w += maxSize.width();

            if ( n > 1 )
                w += ( n - 1 ) * ( bulletSize.width() + spacing );
        }

        h = maxSize.height();
    }
    else
    {
        if ( n > 0 )
        {
            h += maxSize.height();

            if ( n > 1 )
                h += ( n - 1 ) * ( bulletSize.height() + spacing );
        }

        w = maxSize.width();
    }

    const auto hint = indicator->outerBoxSize( Q::Panel, QSizeF( w, h ) );
    return hint.expandedTo( indicator->strutSizeHint( Q::Panel ) );
}

#include "moc_QskPageIndicatorSkinlet.cpp"
