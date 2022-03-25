/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGraphicProviderMap.h"
#include "QskGraphicProvider.h"

#include <qhash.h>
#include <qpointer.h>
#include <qstring.h>

static inline QString qskKey( const QString& providerId )
{
    return providerId.toLower();
}

class QskGraphicProviderMap::PrivateData
{
  public:
    QHash< QString, QPointer< QskGraphicProvider > > hashTab;
};

QskGraphicProviderMap::QskGraphicProviderMap()
    : m_data( new PrivateData() )
{
}

QskGraphicProviderMap::~QskGraphicProviderMap()
{
    const auto& hashTab = m_data->hashTab;
    for ( auto it = hashTab.constBegin(); it != hashTab.constEnd(); ++it )
        delete it.value();
}

void QskGraphicProviderMap::insert(
    const QString& providerId, QskGraphicProvider* provider )
{
    if ( provider == nullptr )
    {
        remove( providerId );
        return;
    }

    m_data->hashTab.insert( qskKey( providerId ),
        QPointer< QskGraphicProvider >( provider ) );
}

void QskGraphicProviderMap::remove( const QString& providerId )
{
    delete take( providerId );
}

QskGraphicProvider* QskGraphicProviderMap::take( const QString& providerId )
{
    QskGraphicProvider* provider = nullptr;

    const auto it = m_data->hashTab.constFind( qskKey( providerId ) );
    if ( it != m_data->hashTab.constEnd() )
    {
        provider = it.value();
        m_data->hashTab.erase( it );
    }

    return provider;
}

QskGraphicProvider* QskGraphicProviderMap::provider( const QString& providerId ) const
{
    const auto it = m_data->hashTab.constFind( qskKey( providerId ) );
    if ( it == m_data->hashTab.constEnd() )
        return nullptr;

    if ( it.value().isNull() )
    {
        // someone outside has deleted the provider, we can remove the entry
        m_data->hashTab.erase( it );
    }

    return it.value();
}

int QskGraphicProviderMap::size() const
{
    // might not be correct, when providers have been deleted
    // from the outside. TODO ...

    return m_data->hashTab.count();
}
