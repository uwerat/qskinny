#include "QskHunspellCompositionModel.h"

#include "hunspell/hunspell.h"

#include <QDebug>

QskHunspellCompositionModel::QskHunspellCompositionModel()
    : QskInputCompositionModel(),
      // ### ship with code if license allows:
      // ### load the language specific one depending on the locale
      m_hunspellHandle( Hunspell_create( "/usr/share/hunspell/en_US.aff",
                                         "/usr/share/hunspell/en_US.dic" ) )
{
}

QskHunspellCompositionModel::~QskHunspellCompositionModel()
{
    Hunspell_destroy( m_hunspellHandle );
}

bool QskHunspellCompositionModel::supportsSuggestions() const
{
    return true;
}

void QskHunspellCompositionModel::commitCandidate( int index )
{
    if( index < m_candidates.count() )
    {
        // The user usually selects a full word, so we can add the space
        QString commitString = candidate( index ) + " ";
        commit( commitString );
    }
}

int QskHunspellCompositionModel::candidateCount() const
{
    return m_candidates.count();
}

QString QskHunspellCompositionModel::candidate( int pos ) const
{
    return m_candidates.at( pos );
}

QString QskHunspellCompositionModel::polishPreedit( const QString& preedit )
{
    if( preedit.isEmpty() )
    {
        // new word: delete suggestions
        m_candidates.clear();
    }
    else
    {
        char** suggestions;
        QByteArray word = preedit.toUtf8(); // ### do we need to check the encoding (see qtvirtualkeyboard)?
        int suggestionCount = Hunspell_suggest( m_hunspellHandle, &suggestions, word.constData() );

        QVector< QString > candidates;
        candidates.reserve( suggestionCount );

        for( int a = 0; a < suggestionCount; ++a )
        {
            QString suggestion = QString::fromUtf8( suggestions[a] ); // ### encoding?

            if( suggestion.startsWith( preedit ) )
            {
                candidates.prepend( suggestion );
            }
            else
            {
                candidates.append( suggestion );
            }
        }
        Hunspell_free_list( m_hunspellHandle, &suggestions, suggestionCount );

        m_candidates = candidates;
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
