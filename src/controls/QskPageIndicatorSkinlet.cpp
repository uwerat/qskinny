/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPageIndicatorSkinlet.h"
#include "QskPageIndicator.h"
#include "QskBoxNode.h"

QskPageIndicatorSkinlet::QskPageIndicatorSkinlet( QskSkin* skin ):
    QskSkinlet(skin)
{
    setNodeRoles( { PanelRole, BulletsRole } );
}

QskPageIndicatorSkinlet::~QskPageIndicatorSkinlet()
{
}

QRectF QskPageIndicatorSkinlet::subControlRect(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    const auto indicator = static_cast< const QskPageIndicator* >( skinnable );

    if ( subControl == QskPageIndicator::Panel )
    {
        return indicator->contentsRect();
    }

    return Inherited::subControlRect( skinnable, subControl );
}

QRectF QskPageIndicatorSkinlet::bulletRect(
    const QskPageIndicator* indicator, const QRectF& rect, int index ) const
{
#if 1
    // ignoring, that width/height might differ !!!
    const qreal szNormal = indicator->bulletSize( QskPageIndicator::Bullet ).width();
    const qreal szHighlighted = indicator->bulletSize( QskPageIndicator::Highlighted ).width();
#endif

    // scale bullet size if we are in between a transition:
    qreal indexDiff = qAbs( indicator->currentIndex() - index );
    if (indexDiff > ( indicator->count() - 1 ) )
        indexDiff = ( indicator->count() - indicator->currentIndex() ); // wrapping

    const qreal sz =  ( indexDiff < 1 ) ?
        ( 1 - indexDiff ) * szHighlighted + indexDiff * szNormal : szNormal;

    const qreal spacing = indicator->metric( QskPageIndicator::Panel | QskAspect::Spacing );
    const bool horizontal = ( indicator->orientation() == Qt::Horizontal );

    qreal w, h;
    if ( horizontal )
    {
        w = ( indicator->count() - 1 ) * ( szNormal + spacing ) + szHighlighted;
        h = rect.height();
    }
    else
    {
        w = rect.width();
        h = ( indicator->count() - 1 ) * ( szNormal + spacing ) + szHighlighted;
    }
    QRectF r( 0, 0, w, h );
    r.moveCenter( rect.center() );

    // x-adjust (in horizontal mode) when not scrolling:
    const qreal s = ( index > indicator->currentIndex() ) ? szHighlighted : szNormal;

    // adapt x-adjust while scrolling:
    qreal s2;
    // scrolling from or to this bullet:
    if ( indexDiff < 1 && index >= indicator->currentIndex() )
    {
        s2 = szNormal + qAbs( szHighlighted - szNormal ) * indexDiff;
    }
    // wrapping case:
    else if ( ( indicator->currentIndex() > ( indicator->count() - 1 ) &&
                index > ( indicator->currentIndex() - indicator->count() + 1 ) ) )
    {
        qreal wrappingDiff = indexDiff;
        while ( wrappingDiff > 1 )
            wrappingDiff -= 1;
        s2 = szNormal + qAbs( szHighlighted - szNormal ) * wrappingDiff;
    }
    // unrelated bullet:
    else
    {
        s2 = s;
    }

    qreal x = r.left() + s2 + spacing + ( index - 1 ) * ( szNormal + spacing );

    qreal yAdjust;
    if ( indicator->currentIndex() == index )
    {
        yAdjust = qMax( 0.0, szNormal - szHighlighted ) / 2;
    }
    else
    {
        yAdjust = qMax( 0.0, szHighlighted - szNormal ) / 2;
    }
    if ( indexDiff < 1 )
        yAdjust *= indexDiff;
    const qreal y = ( horizontal ? r.top() : r.left() ) + yAdjust;

    QRectF ret = horizontal ? QRectF( x, y, sz, sz ) : QRectF( y, x, sz, sz );
    return ret;
}

QSGNode* QskPageIndicatorSkinlet::updateSubNode( const QskSkinnable* skinnable,
    quint8 nodeRole, QSGNode* node ) const
{
    const auto indicator = static_cast< const QskPageIndicator* >( skinnable );

    switch( nodeRole )
    {
        case PanelRole:
            return updatePanelNode( indicator, node );

        case BulletsRole:
            return updateBulletsNode( indicator, node );

        default:
            return nullptr;
    }
}

QSGNode* QskPageIndicatorSkinlet::updatePanelNode(
    const QskPageIndicator* indicator, QSGNode* node ) const
{
    return updateBoxNode( indicator, node, QskPageIndicator::Panel );
}

QSGNode* QskPageIndicatorSkinlet::updateBulletsNode(
    const QskPageIndicator* indicator, QSGNode* node) const
{
    if ( indicator->count() == 0 )
        return nullptr;

    if ( node == nullptr )
        node = new QSGNode();

    QRectF rect = subControlRect( indicator, QskPageIndicator::Panel );
    rect = indicator->innerBox( QskPageIndicator::Panel, rect );

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
        {
            bulletNode = new QskBoxNode();
            node->appendChildNode( bulletNode );
        }

        updateBoxNode( indicator, bulletNode, bulletRect( indicator, rect, i ),
            ( i == currentBullet ) ? Q::Highlighted : Q::Bullet );
    }

    // if count has decreased we need to remove superfluous nodes
    while ( bulletNode->nextSibling() != nullptr )
        node->removeChildNode( bulletNode->nextSibling() );

    return node;
}

#include "moc_QskPageIndicatorSkinlet.cpp"
