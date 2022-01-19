#include "QskHunspellTextPredictor.h"
#include <QVector>

#include "hunspell.h"

class QskHunspellTextPredictor::PrivateData
{
  public:
    Hunhandle* hunspellHandle;
    QVector< QString > candidates;
};

QskHunspellTextPredictor::QskHunspellTextPredictor( QObject* object )
    : Inherited( Words, object )
    , m_data( new PrivateData() )
{
#if 1
    // TODO: loading the language specific one depending on the locale

    m_data->hunspellHandle = Hunspell_create(
        "/usr/share/hunspell/en_US.aff",
        "/usr/share/hunspell/en_US.dic" );
#endif
}

QskHunspellTextPredictor::~QskHunspellTextPredictor()
{
    Hunspell_destroy( m_data->hunspellHandle );
}

int QskHunspellTextPredictor::candidateCount() const
{
    return m_data->candidates.count();
}

QString QskHunspellTextPredictor::candidate( int pos ) const
{
    return m_data->candidates[ pos ];
}

void QskHunspellTextPredictor::reset()
{
    if ( !m_data->candidates.isEmpty() )
    {
        m_data->candidates.clear();
        Q_EMIT predictionChanged();
    }
}

void QskHunspellTextPredictor::request( const QString& text )
{
    char** suggestions;
    const QByteArray word = text.toUtf8(); // ### do we need to check the encoding

    const int count = Hunspell_suggest(
        m_data->hunspellHandle, &suggestions, word.constData() );

    QVector< QString > candidates;
    candidates.reserve( count );

    for ( int i = 0; i < count; i++ )
    {
        const QString suggestion = QString::fromUtf8( suggestions[ i ] ); // ### encoding?

        if ( suggestion.startsWith( text ) )
            candidates.prepend( suggestion );
        else
            candidates.append( suggestion );
    }

    Hunspell_free_list( m_data->hunspellHandle, &suggestions, count );

    m_data->candidates = candidates;
    Q_EMIT predictionChanged();
}
