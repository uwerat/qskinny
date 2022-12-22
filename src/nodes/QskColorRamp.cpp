/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskColorRamp.h"
#include "QskRgbValue.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qrhi_p.h>
#include <private/qsgplaintexture_p.h>
QSK_QT_PRIVATE_END

#include <qcoreapplication.h>

namespace
{
    class Texture : public QSGPlainTexture
    {
      public:
        Texture( const QskGradientStops& stops, QskGradient::SpreadMode spreadMode )
        {
            /*
                Qt creates tables of 1024 colors, while Chrome, Firefox, and Android
                seem to use 256 colors only ( according to maybe outdated sources
                from the internet ),
             */

            setImage( QskRgb::colorTable( 256, stops ) );

            const auto wrapMode = this->wrapMode( spreadMode );

            setHorizontalWrapMode( wrapMode );
            setVerticalWrapMode( wrapMode );

            setFiltering( QSGTexture::Linear );
        };

      private:
        static inline QSGTexture::WrapMode wrapMode( QskGradient::SpreadMode spreadMode )
        {
            switch ( spreadMode )
            {
                case QskGradient::RepeatSpread:
                    return QSGTexture::Repeat;

                case QskGradient::ReflectSpread:
                    return QSGTexture::MirroredRepeat;

                default:
                    return QSGTexture::ClampToEdge;
            }
        }
    };

    class HashKey
    {
      public:
        inline bool operator==( const HashKey& other ) const
        {
            return rhi == other.rhi && spreadMode == other.spreadMode && stops == other.stops;
        }

        const void* rhi;
        const QskGradientStops stops;
        const QskGradient::SpreadMode spreadMode;
    };

    inline size_t qHash( const HashKey& key, size_t seed = 0 )
    {
        size_t values = seed + key.spreadMode;

        for ( const auto& stop : key.stops )
            values += stop.rgb();

        return values;
    }

    class Cache
    {
      public:
        ~Cache() { qDeleteAll( m_hashTable ); }

        void cleanupRhi( const QRhi* );

        Texture* texture( const void* rhi,
            const QskGradientStops&, QskGradient::SpreadMode );

      private:
        QHash< HashKey, Texture* > m_hashTable;
        QVector< const QRhi* > m_rhiTable; // no QSet: we usually have only one entry
    };

    static Cache* s_cache;
}

static void qskCleanupCache()
{
    delete s_cache;
    s_cache = nullptr;
}

static void qskCleanupRhi( const QRhi* rhi )
{
    if ( s_cache )
        s_cache->cleanupRhi( rhi );
}

Texture* Cache::texture( const void* rhi,
    const QskGradientStops& stops, QskGradient::SpreadMode spreadMode )
{
    const HashKey key { rhi, stops, spreadMode };

    auto texture = m_hashTable[key];
    if ( texture == nullptr )
    {
        texture = new Texture( stops, spreadMode );
        m_hashTable[ key ] = texture;

        if ( rhi != nullptr )
        {
            auto myrhi = ( QRhi* )rhi;

            if ( !m_rhiTable.contains( myrhi ) )
            {
                myrhi->addCleanupCallback( qskCleanupRhi );
                m_rhiTable += myrhi;
            }
        }
    }

    return texture;
}

void Cache::cleanupRhi( const QRhi* rhi )
{
    for ( auto it = m_hashTable.begin(); it != m_hashTable.end(); )
    {
        if ( it.key().rhi == rhi )
        {
            delete it.value();
            it = m_hashTable.erase( it );
        }
        else
        {
            ++it;
        }
    }

    m_rhiTable.removeAll( rhi );
}

QSGTexture* QskColorRamp::texture( const void* rhi,
    const QskGradientStops& stops, QskGradient::SpreadMode spreadMode )
{
    if ( s_cache == nullptr )
    {
        s_cache = new Cache();

        /*
            For RHI we have QRhi::addCleanupCallback, but with
            OpenGL we would have to fiddle around with QOpenGLSharedResource
            But as the OpenGL path is only for Qt5 we do not want to spend
            much energy on finetuning the resource management.
         */
        qAddPostRoutine( qskCleanupCache );
    }

    return s_cache->texture( rhi, stops, spreadMode );
}
