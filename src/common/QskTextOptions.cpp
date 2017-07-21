/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTextOptions.h"
#include <QTextDocument>
#include <QDebug>
#include <limits>

QskTextOptions::QskTextOptions():
    m_maximumLineCount( std::numeric_limits< int >::max() ),
    m_fontSizeMode( QskTextOptions::FixedSize ),
    m_wrapMode( QskTextOptions::NoWrap ),
    m_format( PlainText ), // AutoText ???
    m_elideMode( Qt::ElideNone )
{
}

void QskTextOptions::setFormat( TextFormat format )
{
    m_format = format;
}

QskTextOptions::TextFormat QskTextOptions::format() const
{
    return m_format;
}

void QskTextOptions::setElideMode( Qt::TextElideMode elideMode )
{
    m_elideMode = elideMode;
}

Qt::TextElideMode QskTextOptions::elideMode() const
{
    return m_elideMode;
}

void QskTextOptions::setWrapMode( WrapMode wrapMode )
{
    m_wrapMode = wrapMode;
}

QskTextOptions::WrapMode QskTextOptions::wrapMode() const
{
    return m_wrapMode;
}

void QskTextOptions::setFontSizeMode( FontSizeMode fontSizeMode )
{
    m_fontSizeMode = fontSizeMode;
}   
    
QskTextOptions::FontSizeMode QskTextOptions::fontSizeMode() const
{   
    return m_fontSizeMode;
}

void QskTextOptions::setMaximumLineCount( int lineCount )
{
    m_maximumLineCount = lineCount;
}

int QskTextOptions::maximumLineCount() const
{
    return m_maximumLineCount;
}

int QskTextOptions::textFlags() const
{
    int flags = 0;

    switch( m_wrapMode )
    {
        case QTextOption::NoWrap:
        {
            flags |= Qt::TextSingleLine;
            break;
        }
        case QskTextOptions::WordWrap:
        {
            flags |= Qt::TextWordWrap;
            break;
        }
        case QskTextOptions::WrapAnywhere:
        {
            flags |= Qt::TextWrapAnywhere;
            break;
        }
        case QskTextOptions::Wrap:
        {
            // ???
            flags |= Qt::TextWrapAnywhere;
            flags |= Qt::TextWordWrap;
            break;
        }
    }

    return flags;
}

bool QskTextOptions::operator==( const QskTextOptions& other ) const
{
    return ( m_format == other.m_format )
           && ( m_elideMode == other.m_elideMode )
           && ( m_wrapMode == other.m_wrapMode )
           && ( m_fontSizeMode == other.m_fontSizeMode )
           && ( m_maximumLineCount == other.m_maximumLineCount );
}

bool QskTextOptions::isRichText( const QString& text ) const
{
    if ( text.isEmpty() )
        return false;

    if ( m_format == QskTextOptions::AutoText )
        return Qt::mightBeRichText( text );

    return m_format == QskTextOptions::RichText;
}

uint qHash( const QskTextOptions &options, uint seed ) noexcept
{
    uint hash;

    hash = qHash( options.maximumLineCount(), seed );
    hash = qHash( options.fontSizeMode(), hash );
    hash = qHash( options.wrapMode(), hash );
    hash = qHash( options.format(), hash );
    hash = qHash( options.elideMode(), hash );

    return hash;
}

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<( QDebug debug, const QskTextOptions& options )
{
    QDebugStateSaver saver( debug );
    debug.nospace();
    debug << "TextOptions" << '(';
    debug << options.format() << "," << options.elideMode()
          << options.fontSizeMode() << options.wrapMode()
          << "," << options.maximumLineCount();
    debug << ')';
    return debug;
}

#endif

#include "moc_QskTextOptions.cpp"
