/******************************************************************************
 * QSkinny - Copyright (C) 2022 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMainView.h"

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

#include "moc_QskMainView.cpp"
