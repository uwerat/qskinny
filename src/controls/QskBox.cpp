/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBox.h"

QSK_SUBCONTROL( QskBox, Panel )

QskBox::QskBox( QQuickItem* parent ):
    Inherited( parent )
{
}

QskBox::~QskBox()
{
}

QRectF QskBox::layoutRect() const
{   
    return innerBox( Panel, subControlRect( Panel ) );
}

QSizeF QskBox::contentsSizeHint() const
{
    QSizeF size( -1, -1 );

    if ( autoLayoutChildren() )
    {
        const QSizeF hint = Inherited::contentsSizeHint();
        
        if ( hint.width() > 0 )
            size.setWidth( hint.width() );
            
        if ( hint.height() > 0 )
            size.setHeight( hint.height() );
    }       

    const QSizeF minSize(
        metric( Panel | QskAspect::MinimumWidth ),
        metric( Panel | QskAspect::MinimumHeight ) );

    return outerBoxSize( Panel, size ).expandedTo( minSize );
}   

qreal QskBox::heightForWidth( qreal width ) const
{
    qreal height = -1;

    if ( autoLayoutChildren() )
    {
        const auto children = childItems();
        if ( !children.isEmpty() )
        {
            const auto margins = this->margins();
            width -= margins.left() + margins.right();

            const auto padding = innerPadding( Panel, QSizeF( width, width ) );
            width -= padding.left() + padding.right();

            for ( auto child : children )
            {
                if ( auto control = qobject_cast< const QskControl* >( child ) )
                {
                    if ( !control->isTransparentForPositioner() )
                        height = qMax( height, control->heightForWidth( width ) );
                }
            }

            if ( height >= 0 )
            {
                height += padding.top() + padding.bottom();
                height += margins.top() + margins.bottom();
            }
        }
    }

    return height;
}

qreal QskBox::widthForHeight( qreal height ) const
{
    qreal width = -1;

    if ( autoLayoutChildren() )
    {
        const auto children = childItems();
        if ( !children.isEmpty() )
        {
            const auto margins = this->margins();
            height -= margins.top() + margins.bottom();

            const auto padding = innerPadding( Panel, QSizeF( height, height ) );
            height -= padding.top() + padding.bottom();

            for ( auto child : children )
            {
                if ( auto control = qobject_cast< const QskControl* >( child ) )
                {
                    if ( !control->isTransparentForPositioner() )
                        width = qMax( width, control->widthForHeight( height ) );
                }
            }

            if ( width >= 0 )
            {
                width += padding.left() + padding.right();
                width += margins.left() + margins.right();
            }
        }
    }

    return width;
}


#include "moc_QskBox.cpp"
