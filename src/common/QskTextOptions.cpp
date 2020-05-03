/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTextOptions.h"
#include <qtextdocument.h>

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

uint qHash( const QskTextOptions& options, uint seed ) noexcept
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
