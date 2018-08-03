/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBox.h"

QSK_SUBCONTROL( QskBox, Panel )

QskBox::QskBox( QQuickItem* parent )
    : Inherited( parent )
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

#include "moc_QskBox.cpp"
