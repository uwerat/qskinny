/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskPinyinTextPredictor.h"

#include <libime/pinyin/pinyinime.h>
#include <libime/pinyin/pinyincontext.h>
#include <libime/pinyin/pinyindictionary.h>
#include <libime/pinyin/pinyinencoder.h>
#include <libime/core/languagemodel.h>
#include <libime/core/userlanguagemodel.h>
#include <libime/core/lattice.h>

#include <QDebug>
#include <QFile>
#include <QStringList>

#include <algorithm>
#include <exception>
#include <memory>
#include <string_view>

namespace
{
    // The Simplified Chinese pinyin dictionary shipped by libime-data. The
    // location can be overridden with QSK_PINYIN_DICT for custom deployments.
    QString pinyinDictionaryFile()
    {
        auto file = qEnvironmentVariable( "QSK_PINYIN_DICT" );

        if ( file.isEmpty() )
            file = QStringLiteral( "/usr/share/libime/sc.dict" );

        return file;
    }
}

class QskPinyinTextPredictor::PrivateData
{
  public:
    std::unique_ptr< libime::PinyinIME > ime;
    std::unique_ptr< libime::PinyinContext > context;

    QStringList candidates;
};

QskPinyinTextPredictor::QskPinyinTextPredictor( QObject* parent )
    : Inherited( parent )
    , m_data( new PrivateData )
{
    /*
        Loading the dictionary and language model is expensive and calls into
        virtual code paths - defer it out of the constructor, like the Hunspell
        predictor does.
     */
    QMetaObject::invokeMethod( this,
        &QskPinyinTextPredictor::loadDictionary, Qt::QueuedConnection );
}

QskPinyinTextPredictor::~QskPinyinTextPredictor() = default;

void QskPinyinTextPredictor::loadDictionary()
{
    const auto dictFile = pinyinDictionaryFile();

    if ( !QFile::exists( dictFile ) )
    {
        qWarning() << "QskPinyinTextPredictor: pinyin dictionary not found at"
                   << dictFile << "- set QSK_PINYIN_DICT to override.";
        return;
    }

    try
    {
        using namespace libime;

        auto dictionary = std::make_unique< PinyinDictionary >();
        dictionary->load( PinyinDictionary::SystemDict,
            dictFile.toLocal8Bit().constData(), PinyinDictFormat::Binary );

        /*
            The language model is resolved through libime's default resolver,
            so we don't have to hardcode the ( architecture specific ) path of
            the zh_CN model shipped by libime-data-language-model.
         */
        auto modelFile = DefaultLanguageModelResolver::instance()
            .languageModelFileForLanguage( "zh_CN" );
        auto model = std::make_unique< UserLanguageModel >( std::move( modelFile ) );

        m_data->ime = std::make_unique< PinyinIME >(
            std::move( dictionary ), std::move( model ) );

        m_data->ime->setNBest( 12 );
        m_data->ime->setFuzzyFlags( PinyinFuzzyFlag::None );

        m_data->context = std::make_unique< PinyinContext >( m_data->ime.get() );
    }
    catch ( const std::exception& e )
    {
        qWarning() << "QskPinyinTextPredictor: failed to initialize:" << e.what();
        m_data->context.reset();
        m_data->ime.reset();
    }
}

void QskPinyinTextPredictor::reset()
{
    if ( m_data->context )
        m_data->context->clear();

    if ( !m_data->candidates.isEmpty() )
    {
        m_data->candidates.clear();
        Q_EMIT predictionChanged( QString(), {} );
    }
}

void QskPinyinTextPredictor::request( const QString& text )
{
    if ( m_data->context == nullptr )
    {
        Q_EMIT predictionChanged( text, {} );
        return;
    }

    auto* context = m_data->context.get();
    context->clear();

    const auto bytes = text.toLatin1();
    context->type( std::string_view( bytes.constData(), bytes.size() ) );

    const auto& results = context->candidates();

    constexpr size_t maxCount = 20;
    const size_t count = std::min( results.size(), maxCount );

    QStringList candidates;
    candidates.reserve( static_cast< int >( count ) );

    for ( size_t i = 0; i < count; i++ )
        candidates += QString::fromStdString( results[ i ].toString() );

    m_data->candidates = candidates;
    Q_EMIT predictionChanged( text, m_data->candidates );
}

#include "moc_QskPinyinTextPredictor.cpp"
