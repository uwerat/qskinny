#include "QskHunspellTextPredictor.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QTextCodec>
#include <QTimer>
#include <QVector>

#include <hunspell/hunspell.h>

class QskHunspellTextPredictor::PrivateData
{
  public:
    Hunhandle* hunspellHandle = nullptr;
    QByteArray hunspellEncoding;
    QStringList candidates;
    QLocale locale;
};

QskHunspellTextPredictor::QskHunspellTextPredictor( const QLocale &locale, QObject* object )
    : Inherited( object )
    , m_data( new PrivateData() )
{
    m_data->locale = locale;

    // make sure we call virtual functions:
    QTimer::singleShot( 0, this, &QskHunspellTextPredictor::loadDictionaries );
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

QPair< QString, QString > QskHunspellTextPredictor::affAndDicFile( const QString& path, const QLocale& locale )
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
    QString userPaths = QString::fromUtf8( qgetenv( "QSK_HUNSPELL_PATH" ) );

#if defined(Q_OS_WIN32)
    QChar separator( ';' );
    QStringList defaultPaths;
#else
    QChar separator( ':' );
    QStringList defaultPaths = { QStringLiteral( "/usr/share/hunspell" ),
                                 QStringLiteral( "/usr/share/myspell/dicts" ) };
#endif

    QStringList paths = userPaths.split( separator, QString::SkipEmptyParts );
    paths.append( defaultPaths );

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

    char** suggestions;

    QTextCodec *codec = QTextCodec::codecForName( m_data->hunspellEncoding );
    const QByteArray word = codec ? codec->fromUnicode( text ) : text.toUtf8();

    const int count = Hunspell_suggest(
        m_data->hunspellHandle, &suggestions, word.constData() );

    QStringList candidates;
    candidates.reserve( count );

    for ( int i = 0; i < count; i++ )
    {
        const QString suggestion = codec ? codec->toUnicode( suggestions[ i ] )
                                         : QString::fromUtf8( suggestions [ i ] );

        if ( suggestion.startsWith( text ) )
            candidates.prepend( suggestion );
        else
            candidates.append( suggestion );
    }

    Hunspell_free_list( m_data->hunspellHandle, &suggestions, count );

    m_data->candidates = candidates;
    Q_EMIT predictionChanged( text, m_data->candidates );
}
