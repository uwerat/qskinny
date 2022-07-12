/******************************************************************************
 * QSkinny - Copyright (C) 2022 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskApplicationView.h"

class QskApplicationView::PrivateData
{
  public:
    PrivateData()
    {
    }

    QskControl* header = nullptr;
    QskControl* body = nullptr;
    QskControl* footer = nullptr;
};

QskApplicationView::QskApplicationView( QQuickItem* parent )
    : Inherited( Qt::Vertical, parent )
    , m_data( new PrivateData )
{
    setAutoAddChildren( false );
    setSpacing( 0 );
}

QskApplicationView::~QskApplicationView()
{
}

QskControl* QskApplicationView::header() const
{
    return m_data->header;
}

void QskApplicationView::setHeader( QskControl* header )
{
    if( m_data->header )
    {
        removeItem( m_data->header );
    }

    m_data->header = header;

    if( m_data->header )
    {
        m_data->header->setSection( QskAspect::Header );
        insertItem( 0, m_data->header );
    }
}

QskControl* QskApplicationView::body() const
{
    return m_data->body;
}

void QskApplicationView::setBody( QskControl* body )
{
    if( m_data->body )
    {
        removeItem( m_data->body );
    }

    m_data->body = body;

    if( m_data->body )
    {
        m_data->body->setSection( QskAspect::Body );
        insertItem( 1, m_data->body );
    }
}

QskControl* QskApplicationView::footer() const
{
    return m_data->footer;
}

void QskApplicationView::setFooter( QskControl* footer )
{
    if( m_data->footer )
    {
        removeItem( m_data->footer );
    }

    m_data->footer = footer;

    if( m_data->footer )
    {
        m_data->footer->setSection( QskAspect::Footer );
        insertItem( 2, m_data->footer );
    }
}

#include "moc_QskApplicationView.cpp"
