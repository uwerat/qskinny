/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSeparatorSkinlet.h"
#include "QskSeparator.h"
#include "QskAspect.h"

QskSeparatorSkinlet::QskSeparatorSkinlet( QskSkin* skin ):
    Inherited( skin )
{
    setNodeRoles( { PanelRole } );
}

QskSeparatorSkinlet::~QskSeparatorSkinlet() = default;

QRectF QskSeparatorSkinlet::subControlRect(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{      
    const auto separator = static_cast< const QskSeparator* >( skinnable );
    
    if ( subControl == QskSeparator::Panel )
    {
        return panelRect( separator );
    }
    
    return Inherited::subControlRect( skinnable, subControl );
}

QSGNode* QskSeparatorSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto separator = static_cast< const QskSeparator* >( skinnable );

    switch( nodeRole )
    {
        case PanelRole:
        {
            const bool isHorizontal = ( separator->orientation() == Qt::Horizontal );
            const QRectF r = subControlRect( separator, QskSeparator::Panel );

            return updateBoxNode( separator, node, r,
                QskSeparator::Panel, isHorizontal ? 0 : -1 );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRectF QskSeparatorSkinlet::panelRect( const QskSeparator* separator ) const
{
    const QRectF cr = separator->contentsRect();
    const qreal m = separator->metric( QskSeparator::Panel );

    QRectF r;

    if ( separator->orientation() == Qt::Horizontal )
    {
        r.setWidth( cr.width() );
        r.setHeight( m );
    }
    else
    {
        r.setHeight( cr.height() );
        r.setWidth( m );
    }

    r.moveCenter( cr.center() );
    return r;
}

#include "moc_QskSeparatorSkinlet.cpp"
