/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBox.h"
#include "QskMargins.h"

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

void QskBox::setPadding( qreal padding )
{
    setPadding( QskMargins( padding ) );
}

void QskBox::setPadding( const QMarginsF& padding )
{
    const auto pd = QskMargins().expandedTo( padding );

    if ( setPaddingHint( Panel, pd ) )
        Q_EMIT paddingChanged( pd );
}

void QskBox::resetPadding()
{
    if ( resetPaddingHint( Panel ) )
        Q_EMIT paddingChanged( paddingHint( Panel ) );
}

QMarginsF QskBox::padding() const
{
    return paddingHint( Panel );
}

QRectF QskBox::layoutRectForSize( const QSizeF& size ) const
{
    if ( m_hasPanel )
        return subControlContentsRect( size, Panel );

    return Inherited::layoutRectForSize( size );
}

#include "moc_QskBox.cpp"
