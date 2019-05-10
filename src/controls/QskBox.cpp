/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBox.h"

QSK_SUBCONTROL( QskBox, Panel )

QskBox::QskBox( QQuickItem* parent )
    : QskBox( true, parent )
{
}

QskBox::QskBox( bool hasPanel, QQuickItem* parent )
    : Inherited( parent )
    , m_hasPanel( hasPanel )
{
}

QskBox::~QskBox()
{
}

void QskBox::setPanel( bool on )
{
    if ( on != m_hasPanel )
    {
        m_hasPanel = on;

        resetImplicitSize();
        polish();
        update();
    }
}

bool QskBox::hasPanel() const
{
    return m_hasPanel;
}

QRectF QskBox::layoutRectForSize( const QSizeF& size ) const
{
    if ( !m_hasPanel )
        return Inherited::layoutRectForSize( size );

    return innerBox( Panel, subControlRect( size, Panel ) );
}

QSizeF QskBox::contentsSizeHint() const
{
    if ( !m_hasPanel )
        return Inherited::contentsSizeHint();

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
