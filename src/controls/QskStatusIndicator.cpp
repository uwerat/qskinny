/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskStatusIndicator.h"
#include "QskColorFilter.h"
#include "QskGraphic.h"
#include "QskGraphicProvider.h"

#include <qdebug.h>

QSK_SUBCONTROL( QskStatusIndicator, Graphic )

namespace
{
    class StatusData
    {
      public:
        StatusData( const QskGraphic& graphic )
            : graphic( graphic )
            , isDirty( false )
        {
        }

        StatusData( const QUrl& url )
            : source( url )
            , isDirty( !url.isEmpty() )
        {
        }

        void ensureGraphic( const QskStatusIndicator* indicator )
        {
            if ( !source.isEmpty() && isDirty )
            {
                graphic = indicator->loadSource( source );
                isDirty = false;
            }
        }

        QUrl source;
        QskGraphic graphic;
        bool isDirty : 1;
    };
}

class QskStatusIndicator::PrivateData
{
  public:
    PrivateData()
        : currentStatus( -1 )
    {
    }

    int currentStatus;
    QMap< int, StatusData > map;
    mutable QList<int> statusList;
};

QskStatusIndicator::QskStatusIndicator( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Expanding );
}

QskStatusIndicator::~QskStatusIndicator()
{
}

QUrl QskStatusIndicator::source( int status ) const
{
    const auto it = m_data->map.constFind( status );
    if ( it != m_data->map.constEnd() )
        return it->source;

    return QUrl();
}

void QskStatusIndicator::setSource( int status, const QUrl& url )
{
    bool hasChanged = false;

    const auto it = m_data->map.find( status );
    if ( it != m_data->map.end() )
    {
        if ( it->source != url )
        {
            it->source = url;
            it->graphic.reset();
            it->isDirty = !url.isEmpty();

            hasChanged = true;
        }
    }
    else
    {
        m_data->map.insert( status, StatusData( url ) );
        m_data->statusList.clear();

        hasChanged = true;
    }

    if ( hasChanged )
    {
        resetImplicitSize();

        if ( status == m_data->currentStatus )
            update();
    }
}

QskGraphic QskStatusIndicator::graphic( int status ) const
{
    const auto it = m_data->map.find( status );
    if ( it != m_data->map.end() )
    {
        it->ensureGraphic( this );
        return it->graphic;
    }

    return QskGraphic();
}

void QskStatusIndicator::setGraphic( int status, const QskGraphic& graphic )
{
    bool hasChanged = false;

    const auto it = m_data->map.find( status );
    if ( it != m_data->map.end() )
    {
        if ( !it->source.isEmpty() || graphic != it->graphic )
        {
            it->source.clear();
            it->isDirty = false;
            it->graphic = graphic;

            hasChanged = true;
        }
    }
    else
    {
        m_data->map.insert( status, StatusData( graphic ) );
        m_data->statusList.clear();
        hasChanged = true;
    }

    if ( hasChanged )
    {
        resetImplicitSize();

        if ( status == m_data->currentStatus )
            update();
    }
}

QskColorFilter QskStatusIndicator::graphicFilter( int status ) const
{
    Q_UNUSED( status )
    return effectiveGraphicFilter( Graphic );
}

void QskStatusIndicator::setGraphicRole( int role )
{
    if ( setGraphicRoleHint( Graphic, role ) )
        Q_EMIT graphicRoleChanged( role );
}

void QskStatusIndicator::resetGraphicRole()
{
    if ( resetGraphicRoleHint( Graphic ) )
        Q_EMIT graphicRoleChanged( graphicRoleHint( Graphic ) );
}

int QskStatusIndicator::graphicRole() const
{
    return graphicRoleHint( Graphic );
}

int QskStatusIndicator::status() const
{
    return m_data->currentStatus;
}

bool QskStatusIndicator::hasStatus( int status ) const
{
    return m_data->map.contains( status );
}

void QskStatusIndicator::setStatus( int status )
{
    if ( status == m_data->currentStatus )
        return;

    const auto it = m_data->map.constFind( status );
    if ( it == m_data->map.constEnd() )
    {
        qWarning() << "QskStatusIndicator: invalid status:" << status;
        return;
    }

    m_data->currentStatus = status;
    Q_EMIT statusChanged( m_data->currentStatus );

    // we should have a mode to decide if we
    // want to keep the hidden graphics in memory

    if ( it->isDirty )
        polish();

    update();
}

QList<int> QskStatusIndicator::statusList() const
{
    /*
        We should be have a QMap< int, QskGraphic >, so that
        users can iterate over all entries without having to
        do extra lookups for each entry. TODO ...
     */
    if ( m_data->statusList.isEmpty() && !m_data->map.isEmpty() )
        m_data->statusList = m_data->map.keys();

    return m_data->statusList;
}

void QskStatusIndicator::changeEvent( QEvent* event )
{
    if ( event->type() == QEvent::StyleChange )
    {
        for ( auto& statusData : m_data->map )
        {
            if ( !statusData.source.isEmpty() )
            {
                statusData.graphic.reset();
                statusData.isDirty = true;
            }
        }
    }

    Inherited::changeEvent( event );
}

void QskStatusIndicator::updateLayout()
{
    const auto it = m_data->map.find( m_data->currentStatus );
    if ( it != m_data->map.end() )
        it->ensureGraphic( this );
}

QskGraphic QskStatusIndicator::loadSource( const QUrl& url ) const
{
    return Qsk::loadGraphic( url );
}

#include "moc_QskStatusIndicator.cpp"
