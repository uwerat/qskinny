/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPinyinTextPredictor.h"
#include "QskInputContext.h"

#include "pinyinime.h"

#include <QDebug>
#include <QStringList>

class QskPinyinTextPredictor::PrivateData
{
  public:
    QStringList candidates;
};

QskPinyinTextPredictor::QskPinyinTextPredictor( QObject* parent )
    : Inherited( Attributes(), parent )
    , m_data( new PrivateData )
{
#if 1
    const char dictionary[] = "XXX/3rdparty/pinyin/data/dict_pinyin.dat";
#endif

    // ### prevent having 2 calls to im_open_decoder by using a singleton or so
    bool opened = ime_pinyin::im_open_decoder( dictionary, "" );

    if ( !opened )
    {
        qWarning() << "could not open pinyin decoder dictionary at" << dictionary;
    }
}

QskPinyinTextPredictor::~QskPinyinTextPredictor()
{
    ime_pinyin::im_close_decoder();
}

int QskPinyinTextPredictor::candidateCount() const
{
    return m_data->candidates.count();
}

QString QskPinyinTextPredictor::candidate( int index ) const
{
    if ( ( index >= 0 ) && ( index < m_data->candidates.count() ) )
        return m_data->candidates[ index ];

    return QString();
}

void QskPinyinTextPredictor::reset()
{
    ime_pinyin::im_reset_search();

    if ( !m_data->candidates.isEmpty() )
    {
        m_data->candidates.clear();
        Q_EMIT predictionChanged();
    }
}

void QskPinyinTextPredictor::request( const QString& text )
{
    const QByteArray bytes = text.toLatin1();

    size_t count = ime_pinyin::im_search(
        bytes.constData(), size_t( bytes.length() ) );

    if ( count <= 0 )
        return;

    const size_t maxCount = 20;
    if ( count > maxCount )
        count = maxCount;

    QVector< QChar > candidateBuffer;
    candidateBuffer.resize( ime_pinyin::kMaxSearchSteps + 1 );

    QStringList candidates;
    candidates.reserve( count );

    for ( unsigned int i = 0; i < count; i++ )
    {
        size_t length = static_cast< size_t >( candidateBuffer.length() - 1 );
        const auto buf = reinterpret_cast< ime_pinyin::char16* >( candidateBuffer.data() );

        const bool found = ime_pinyin::im_get_candidate( i, buf, length );
        Q_ASSERT( found );

        candidateBuffer.last() = 0;

        auto candidate = QString( candidateBuffer.data() );
        candidate = QChar( Qt::Key( candidate[ 0 ].unicode() ) );

        candidates += candidate;
    }

    m_data->candidates = candidates;
    Q_EMIT predictionChanged();
}
