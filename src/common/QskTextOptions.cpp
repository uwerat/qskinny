/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTextOptions.h"
#include <qtextdocument.h>

static void qskRegisterTextOptions()
{
    qRegisterMetaType< QskTextOptions >();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QMetaType::registerEqualsComparator< QskTextOptions >();
#endif
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterTextOptions )

int QskTextOptions::textFlags() const noexcept
{
    int flags = 0;

    switch ( m_wrapMode )
    {
        case QskTextOptions::NoWrap:
        {
            // flags |= Qt::TextSingleLine;
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

QskTextOptions::TextFormat QskTextOptions::effectiveFormat( const QString& text ) const
{
    if ( text.isEmpty() )
        return PlainText;

    if ( m_format == QskTextOptions::AutoText )
        return Qt::mightBeRichText( text ) ? StyledText : PlainText;
    else
        return m_format;
}

QskHashValue QskTextOptions::hash( QskHashValue seed ) const noexcept
{
    auto hash = qHash( m_maximumLineCount, seed );
    hash = qHash( m_fontSizeMode, hash );
    hash = qHash( m_wrapMode, hash );
    hash = qHash( m_format, hash );
    hash = qHash( m_elideMode, hash );

    return hash;
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

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
