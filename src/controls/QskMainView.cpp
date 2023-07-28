/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskMainView.h"
#include "QskQuick.h"

/*
    This code is a placeholder implementation until we know
    what kind of features we actually want to have
 */
class QskMainView::PrivateData
{
  public:
    QPointer< QskControl > header;
    QPointer< QskControl > body;
    QPointer< QskControl > footer;
};

QskMainView::QskMainView( QQuickItem* parent )
    : Inherited( Qt::Vertical, parent )
    , m_data( new PrivateData )
{
    setAutoAddChildren( false );
    setSpacing( 0 );
    setPanel( true );

    setFlag( QQuickItem::ItemIsFocusScope, true );
    setTabFence( true );
    setFocusPolicy( Qt::StrongFocus );
}

QskMainView::~QskMainView()
{
}

QskControl* QskMainView::header() const
{
    return m_data->header;
}

void QskMainView::setHeader( QskControl* header )
{
    if ( header == m_data->header )
        return;

    delete m_data->header;
    m_data->header = header;

    if( header )
    {
        header->setSection( QskAspect::Header );
        insertItem( 0, header );
    }
}

QskControl* QskMainView::body() const
{
    return m_data->body;
}

void QskMainView::setBody( QskControl* body )
{
    if ( body == m_data->body )
        return;

    delete m_data->body;
    m_data->body = body;

    if( body )
    {
        body->setSection( QskAspect::Body );
        insertItem( 1, body );
    }
}

QskControl* QskMainView::footer() const
{
    return m_data->footer;
}

void QskMainView::setFooter( QskControl* footer )
{
    if ( footer == m_data->footer )
        return;

    delete m_data->footer;
    m_data->footer = footer;

    if( footer )
    {
        footer->setSection( QskAspect::Footer );
        insertItem( 2, footer );
    }
}

void QskMainView::focusInEvent( QFocusEvent* event )
{
    Inherited::focusInEvent( event );

    if ( isFocusScope() && ( scopedFocusItem() == nullptr ) )
    {
        if ( auto focusItem = nextItemInFocusChain( true ) )
        {
            if ( qskIsItemComplete( focusItem )
                 && qskIsAncestorOf( this, focusItem ) )
            {
                focusItem->setFocus( true );
            }
        }
    }
}

#include "moc_QskMainView.cpp"
