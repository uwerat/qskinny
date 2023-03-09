/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskIcon.h"
#include "QskGraphicProvider.h"

#include <qvariant.h>
#include <qhashfunctions.h>

static void qskRegisterIcon()
{
    qRegisterMetaType< QskIcon >();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QMetaType::registerEqualsComparator< QskIcon >();
#endif
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterIcon )

QskIcon::QskIcon()
    : m_data( new Data() )
{
}

QskIcon::QskIcon( const QUrl& source )
    : m_source( source )
    , m_data( new Data() )
{
}

QskIcon::QskIcon( const QskGraphic& graphic )
    : m_data( new Data() )
{
    if ( !graphic.isNull() )
        m_data->graphic = new QskGraphic( graphic );
}

bool QskIcon::operator==( const QskIcon& other ) const noexcept
{
    if ( m_data == other.m_data )
        return true; // copy

    if ( !m_source.isEmpty() || !other.m_source.isEmpty() )
        return m_source == other.m_source;

    if ( m_data->graphic && other.m_data->graphic )
        return m_data->graphic == other.m_data->graphic;

    return false;
}

void QskIcon::setSource( const QUrl& source )
{
    if ( source != m_source )
    {
        m_source = source;

#if 1
        m_data.detach(); // needed ???
#endif
        m_data.reset();
    }
}

void QskIcon::setGraphic( const QskGraphic& graphic )
{
    m_source.clear();
    m_data.detach();

    if ( m_data->graphic )
        *m_data->graphic = graphic;
    else
        m_data->graphic = new QskGraphic( graphic );
}

QskGraphic QskIcon::graphic() const
{
    if ( m_data->graphic == nullptr && !m_source.isEmpty() )
    {
        /*
            not detaching: lazy loading should affect all copies

            note: even when loadGraphic returns a null graphic we
            initialize m_iconData to avoid, that loading will
            be repeated again and again.
         */

        m_data->graphic = new QskGraphic( Qsk::loadGraphic( m_source ) );
    }

    return m_data->graphic ? *m_data->graphic : QskGraphic();
}

QskHashValue QskIcon::hash( QskHashValue seed ) const
{
    if ( !m_source.isEmpty() )
        return qHash( m_source, seed );

    return maybeGraphic().hash( seed );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskIcon& icon )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "Icon" << "( ";
    if ( !icon.source().isEmpty() )
    {
        debug << icon.source();
    }
    else
    {
        const auto graphic = icon.maybeGraphic();
        if ( !graphic.isNull() )
            debug << "I:" << graphic.hash( 0 );
    }

    debug << " )";

    return debug;
}

#endif

#include "moc_QskIcon.cpp"
