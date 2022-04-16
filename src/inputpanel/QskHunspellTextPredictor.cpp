/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskHunspellTextPredictor.h"

#include <qlocale.h>
#include <qtimer.h>
#include <qfile.h>
#include <qdir.h>
#include <qdebug.h>

#include <hunspell/hunspell.h>

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )

#include <qtextcodec.h>

namespace
{
    class StringConverter
    {
      public:
        StringConverter( const QByteArray& encoding )
            : m_codec( QTextCodec::codecForName( encoding ) )
        {
        }

        inline QString fromHunspell( const char* text ) const
        {
            if ( m_codec )
                return m_codec->toUnicode( text );

            return QString::fromUtf8( text );
        }

        inline QByteArray toHunspell( const QString& text ) const
        {
            if ( m_codec )
                return m_codec->fromUnicode( text );

            return text.toUtf8();
        }
      private:
        QTextCodec* m_codec;
    };
}

#else

#include <qstringconverter.h>

namespace
{
    class StringConverter
    {
      public:
        StringConverter( const QByteArray& encoding )
            : m_decoder( encoding )
            , m_encoder( encoding )
        {
        }

        inline QString fromHunspell( const char* text ) const
        {
            if ( m_decoder.isValid() )
                return m_decoder.decode( text );

            return QString::fromUtf8( text );
        }

        inline QByteArray toHunspell( const QString& text ) const
        {
            if ( m_encoder.isValid() )
                return m_encoder.encode( text );

            return text.toUtf8();
        }

      private:
        mutable QStringDecoder m_decoder;
        mutable QStringEncoder m_encoder;
    };
}

#endif

class QskHunspellTextPredictor::PrivateData
{
  public:
    Hunhandle* hunspellHandle = nullptr;
    QByteArray hunspellEncoding;
    QStringList candidates;
    QLocale locale;
};

QskHunspellTextPredictor::QskHunspellTextPredictor(
        const QLocale& locale, QObject* object )
    : Inherited( object )
    , m_data( new PrivateData() )
{
    m_data->locale = locale;

    // make sure we call virtual functions:
    QMetaObject::invokeMethod( this,
        &QskHunspellTextPredictor::loadDictionaries, Qt::QueuedConnection );
}

QskHunspellTextPredictor::~QskHunspellTextPredictor()
{
    Hunspell_destroy( m_data->hunspellHandle );
}

void QskHunspellTextPredictor::reset()
{
    if ( !m_data->candidates.isEmpty() )
    {
        m_data->candidates.clear();
        Q_EMIT predictionChanged( QString(), {} );
    }
}

QPair< QString, QString > QskHunspellTextPredictor::affAndDicFile(
    const QString& path, const QLocale& locale )
{
    QString prefix = QStringLiteral( "%1/%2" ).arg( path, locale.name() );
    QString affFile = prefix + QStringLiteral( ".aff" );
    QString dicFile = prefix + QStringLiteral( ".dic" );

    if( QFile::exists( affFile ) && QFile::exists( dicFile ) )
    {
        return qMakePair( affFile, dicFile );
    }
    else
    {
        return {};
    }
}

void QskHunspellTextPredictor::loadDictionaries()
{
    const auto userPaths = QString::fromUtf8( qgetenv( "QSK_HUNSPELL_PATH" ) );

    auto paths = userPaths.split( QDir::listSeparator(), Qt::SkipEmptyParts );

#if !defined( Q_OS_WIN32 )
    paths += QStringLiteral( "/usr/share/hunspell" );
    paths += QStringLiteral( "/usr/share/myspell/dicts" );
#endif

    for( const auto& path : paths )
    {
        auto files = affAndDicFile( path, m_data->locale );

        if( !files.first.isEmpty() && !files.second.isEmpty() )
        {
            m_data->hunspellHandle = Hunspell_create( files.first.toUtf8(), files.second.toUtf8() );
            m_data->hunspellEncoding = Hunspell_get_dic_encoding( m_data->hunspellHandle );
            break;
        }
    }

    if( !m_data->hunspellHandle )
    {
        qWarning() << "could not find Hunspell files for locale" << m_data->locale
                   << "in the following directories:" << paths
                   << ". Consider setting QSK_HUNSPELL_PATH to the directory "
                   << "containing Hunspell .aff and .dic files.";
    }
}

void QskHunspellTextPredictor::request( const QString& text )
{
    if( !m_data->hunspellHandle )
    {
        Q_EMIT predictionChanged( text, {} );
        return;
    }

    StringConverter converter( m_data->hunspellEncoding );

    char** suggestions;

    const int count = Hunspell_suggest( m_data->hunspellHandle,
        &suggestions, converter.toHunspell( text ).constData() );

    QStringList candidates;
    candidates.reserve( count );

    for ( int i = 0; i < count; i++ )
    {
        const auto suggestion = converter.fromHunspell( suggestions[ i ] );

        if ( suggestion.startsWith( text ) )
            candidates.prepend( suggestion );
        else
            candidates.append( suggestion );
    }

    Hunspell_free_list( m_data->hunspellHandle, &suggestions, count );

    m_data->candidates = candidates;
    Q_EMIT predictionChanged( text, m_data->candidates );
}

#include "moc_QskHunspellTextPredictor.cpp"
