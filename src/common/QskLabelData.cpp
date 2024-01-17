/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskLabelData.h"
#include "QskGraphicProvider.h"

#include <qvariant.h>
#include <qhashfunctions.h>

static void qskRegisterLabelData()
{
    qRegisterMetaType< QskLabelData >();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QMetaType::registerEqualsComparator< QskLabelData >();
#endif
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterLabelData )

QskLabelData::QskLabelData( const char* text )
    : m_text( text )
{
}

QskLabelData::QskLabelData( const QString& text )
    : m_text( text )
{
}

QskLabelData::QskLabelData( const QskIcon& icon )
    : m_icon( icon )
{
}

QskLabelData::QskLabelData( const QString& text, const QskIcon& icon )
    : m_text( text )
    , m_icon( icon )
{
}

bool QskLabelData::operator==( const QskLabelData& other ) const noexcept
{
    return ( m_text == other.m_text )
        && ( m_icon == other.m_icon );
}

void QskLabelData::setText( const QString& text )
{
    m_text = text;
}

void QskLabelData::setIconSource( const QUrl& source )
{
    m_icon.setSource( source );
}

void QskLabelData::setIcon( const QskIcon& icon )
{
    m_icon = icon;
}

QskHashValue QskLabelData::hash( QskHashValue seed ) const
{
    const auto hash = qHash( m_text, seed );
    return m_icon.hash( hash );
}

QVector< QskLabelData > qskCreateLabelData( const QStringList& list )
{
    QVector< QskLabelData > labelData;
    labelData.reserve( list.size() );

    for ( const auto& text : list )
        labelData += QskLabelData( text );

    return labelData;
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskLabelData& labelData )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    const auto icon = labelData.icon();
    const auto text = labelData.text();

    debug << "Label" << "( ";
    if ( !text.isEmpty() )
    {
        debug << text;
        if ( !icon.isNull() )
            debug << ", ";
    }

    if ( !icon.source().isEmpty() )
    {
        debug << icon.source();
    }
    else if ( !icon.maybeGraphic().isNull() )
    {
        debug << "I:" << icon.maybeGraphic().hash( 0 );
    }

    debug << " )";

    return debug;
}

#endif

#include "moc_QskLabelData.cpp"
