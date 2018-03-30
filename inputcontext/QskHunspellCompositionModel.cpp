#include "QskHunspellCompositionModel.h"
#include <QVector>

#include "hunspell.h"

class QskHunspellCompositionModel::PrivateData
{
public:
    Hunhandle* hunspellHandle;
    QVector< QString > candidates;
};

QskHunspellCompositionModel::QskHunspellCompositionModel() :
    QskInputCompositionModel(),
    m_data( new PrivateData() )
{
#if 1
    //  ship with code if license allows:
    // loading the language specific one depending on the locale

    m_data->hunspellHandle = Hunspell_create(
        "/usr/share/hunspell/en_US.aff",
        "/usr/share/hunspell/en_US.dic" );
#endif
}

QskHunspellCompositionModel::~QskHunspellCompositionModel()
{
    Hunspell_destroy( m_data->hunspellHandle );
}

bool QskHunspellCompositionModel::supportsSuggestions() const
{
    return true;
}

void QskHunspellCompositionModel::commitCandidate( int index )
{
    if( index < m_data->candidates.count() )
    {
        // The user usually selects a full word, so we can add the space
        QString commitString = candidate( index ) + " ";
        commit( commitString );
    }
}

int QskHunspellCompositionModel::candidateCount() const
{
    return m_data->candidates.count();
}

QString QskHunspellCompositionModel::candidate( int pos ) const
{
    return m_data->candidates[ pos ];
}

QString QskHunspellCompositionModel::polishPreedit( const QString& preedit )
{
    if( preedit.isEmpty() )
    {
        // new word: delete suggestions
        m_data->candidates.clear();
    }
    else
    {
        char** suggestions;
        QByteArray word = preedit.toUtf8(); // ### do we need to check the encoding (see qtvirtualkeyboard)?
        int suggestionCount = Hunspell_suggest(
            m_data->hunspellHandle, &suggestions, word.constData() );

        QVector< QString > candidates;
        candidates.reserve( suggestionCount );

        for( int a = 0; a < suggestionCount; ++a )
        {
            const QString suggestion = QString::fromUtf8( suggestions[a] ); // ### encoding?

            if( suggestion.startsWith( preedit ) )
            {
                candidates.prepend( suggestion );
            }
            else
            {
                candidates.append( suggestion );
            }
        }
        Hunspell_free_list( m_data->hunspellHandle, &suggestions, suggestionCount );

        m_data->candidates = candidates;
    }

    Q_EMIT candidatesChanged();

    return preedit;
}

bool QskHunspellCompositionModel::isComposable( const QStringRef& preedit ) const
{
    // ### What is this function even supposed do?
    Q_UNUSED( preedit );
    return false;
}

bool QskHunspellCompositionModel::hasIntermediate() const
{
    return true;
}
