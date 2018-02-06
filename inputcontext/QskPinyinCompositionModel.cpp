/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPinyinCompositionModel.h"

#include "pinyin/zh.h"

#include <QVector>

class QskPinyinCompositionModel::PrivateData
{
public:
    PrivateData():
        groups( 5 )
    {
    }

    QVector< Qt::Key > groups;

    char currentVowel;
    uchar currentSubtones;

    int candidateCount;
    const PinyinCandidateSection* candidates; // pointer to 6 values, unless null
};

QskPinyinCompositionModel::QskPinyinCompositionModel():
    QskInputCompositionModel(),
    m_data( new PrivateData )
{
    m_data->candidateCount = 0;
    m_data->candidates = nullptr;
    m_data->currentVowel = -1;
    m_data->currentSubtones = NoTone;
}

QskPinyinCompositionModel::~QskPinyinCompositionModel()
{
}

int QskPinyinCompositionModel::candidateCount() const
{
    return qMax( 0, m_data->candidateCount );
}

Qt::Key QskPinyinCompositionModel::candidate( int index ) const
{
    Q_ASSERT( m_data->candidates[ groupIndex() ].data
        && m_data->candidates[ groupIndex() ].size > index );

    return static_cast< Qt::Key >( m_data->candidates[ groupIndex() ].data[ index ] );
}

QVector< Qt::Key > QskPinyinCompositionModel::groups() const
{
    return m_data->groups;
}

bool QskPinyinCompositionModel::hasIntermediate() const
{
    return m_data->currentVowel == 0
           || ( m_data->currentVowel > 0 && m_data->candidates
                && m_data->candidates[0].data );
}

QString QskPinyinCompositionModel::polishPreedit( const QString& preedit )
{
    if ( preedit.isEmpty() )
    {
        m_data->candidates = nullptr;
        m_data->candidateCount = 0;
        Q_EMIT candidatesChanged();

        std::fill( m_data->groups.begin(), m_data->groups.end(), Qt::Key( 0 ) );
        Q_EMIT groupsChanged( m_data->groups );

        return QString();
    }

    const PinyinCandidates candidates = pinyinCandidates( preedit.toUtf8() );

    QString displayText = preedit;
    displayText.replace( 'v', QChar(0x00FC), Qt::CaseInsensitive );

    if ( candidates.vowel > 0 && groupIndex() != 0
         && ( candidates.subtones & ( 1 << ( groupIndex() - 1 ) ) ) )
    {
        const int vowelIndex = preedit.indexOf(candidates.vowel);
        Q_ASSERT(vowelIndex >= 0);
        QChar replacement = vowelWithTone(candidates.vowel, groupIndex() );

        displayText.replace(vowelIndex, 1, replacement);
    }
    else if ( groupIndex() != 0 )
    {
        setGroupIndex( 0 );
    }

    m_data->candidates = candidates.sections;
    m_data->candidateCount = m_data->candidates[ groupIndex() ].size;
    Q_EMIT candidatesChanged();

    if ( candidates.vowel == m_data->currentVowel
         && candidates.subtones == m_data->currentSubtones )
    {
        return displayText;
    }

    m_data->currentVowel = candidates.vowel;
    m_data->currentSubtones = candidates.subtones;

    QVector< Qt::Key > groups =
    {
        ( m_data->currentSubtones & FlatTone )
        ? vowelWithTone( m_data->currentVowel, 1 ) : Qt::Key( 0 ),
        ( m_data->currentSubtones & RisingTone )
        ? vowelWithTone( m_data->currentVowel, 2 ) : Qt::Key( 0 ),
        ( m_data->currentSubtones & LowTone )
        ? vowelWithTone( m_data->currentVowel, 3 ) : Qt::Key( 0 ),
        m_data->groups[4] = ( m_data->currentSubtones & FallingTone )
            ? vowelWithTone( m_data->currentVowel, 4 ) : Qt::Key( 0 ),
        ( m_data->currentSubtones & NeutralTone )
        ? vowelWithTone( m_data->currentVowel, 0 ) : Qt::Key( 0 )
    };

    if ( groups != m_data->groups )
    {
        m_data->groups = groups;
        Q_EMIT groupsChanged( m_data->groups );
    }

    return displayText;
}

bool QskPinyinCompositionModel::isComposable(const QStringRef& preedit) const
{
    const QByteArray text = preedit.toUtf8();
    PinyinCandidates candidates = pinyinCandidates( text );

    if ( candidates.vowel < 0 )
        return false;

    if ( candidates.vowel == 0 )
        return true;

    return candidates.sections[ 0 ].data;
}

void QskPinyinCompositionModel::handleGroupIndexChanged()
{
    m_data->candidateCount = m_data->candidates[ groupIndex() ].size;
}

#include "moc_QskPinyinCompositionModel.cpp"
