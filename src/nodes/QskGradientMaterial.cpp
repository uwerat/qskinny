/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGradientMaterial.h"
#include "QskFunctions.h"
#include "QskRgbValue.h"

#include <qcoreapplication.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qrhi_p.h>
#include <private/qdrawhelper_p.h>
#include <private/qsgplaintexture_p.h>
QSK_QT_PRIVATE_END

#include <cmath>

// RHI shaders are supported by Qt 5.15 and Qt 6.x
#define SHADER_RHI

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    // Old type of shaders only with Qt 5.x
    #define SHADER_GL
#endif

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    #include <QSGMaterialRhiShader>
    using RhiShader = QSGMaterialRhiShader;
#else
    using RhiShader = QSGMaterialShader;
#endif

namespace
{
    class GradientTexture : public QSGPlainTexture
    {
      public:
        GradientTexture( const QGradientStops& stops, QGradient::Spread spread )
        {
#if 1
            /*
                Once we got rid of QGradient we will have QskGradientStops
                ( like in the gradients branch ). For the moment we have to copy
             */

            QskGradientStops qskStops;
            qskStops.reserve( stops.size() );

            for ( const auto& s : stops )
                qskStops += QskGradientStop( s.first, s.second );
#endif
            /*
                Qt creates tables of 1024 colors, while Chrome, Firefox, and Android
                seem to use 256 colors only ( according to maybe outdated sources
                from the internet ),
             */


            setImage( QskRgb::colorTable( 256, qskStops ) );

            const auto wrapMode = this->wrapMode( spread );

            setHorizontalWrapMode( wrapMode );
            setVerticalWrapMode( wrapMode );

            setFiltering( QSGTexture::Linear );
        };

      private:
        static inline QSGTexture::WrapMode wrapMode( QGradient::Spread spread )
        {
            switch ( spread )
            {
                case QGradient::RepeatSpread:
                    return QSGTexture::Repeat;

                case QGradient::ReflectSpread:
                    return QSGTexture::MirroredRepeat;

                default:
                    return QSGTexture::ClampToEdge;
            }
        }
    };

    class TextureHashKey
    {
      public:
        inline bool operator==( const TextureHashKey& other ) const
        {
            return rhi == other.rhi && spread == other.spread && stops == other.stops;
        }

        const void* rhi;
        const QGradientStops stops;
        const QGradient::Spread spread;
    };

    inline size_t qHash( const TextureHashKey& key, size_t seed = 0 )
    {
        size_t valus = seed + key.spread;

        for ( const auto& stop : key.stops )
            valus += stop.second.rgba();

        return valus;
    }

    class TextureCache
    {
      public:
        static TextureCache* instance()
        {
            static TextureCache* s_instance = nullptr;
            if ( s_instance == nullptr )
            {
                s_instance = new TextureCache();

                /*
                    For RHI we have QRhi::addCleanupCallback, but with
                    OpenGL we have to fiddle around with QOpenGLSharedResource
                    So let's keep things simple for the moment. TODO ...
                 */
                qAddPostRoutine( cleanup );
            }

            return s_instance;
        }

        ~TextureCache()
        {
            qDeleteAll( m_hashTable );
        }

        GradientTexture* texture( const void* rhi,
            const QGradientStops& stops, QGradient::Spread spread )
        {
            const TextureHashKey key { rhi, stops, spread };

            auto texture = m_hashTable[key];
            if ( texture == nullptr )
            {
                texture = new GradientTexture( stops, spread );
                m_hashTable[ key ] = texture;

                if ( rhi != nullptr )
                {
                    auto myrhi = ( QRhi* )rhi;

                    if ( !m_rhiTable.contains( myrhi ) )
                    {
                        myrhi->addCleanupCallback( TextureCache::cleanupRhi );
                        m_rhiTable += myrhi;
                    }
                }
            }

            return texture;
        }

      private:
        static void cleanup()
        {
            delete instance();
        }

        static void cleanupRhi( const QRhi *rhi )
        {
            auto cache = instance();

            auto& table = cache->m_hashTable;
            for ( auto it = table.begin(); it != table.end(); )
            {
                if ( it.key().rhi == rhi )
                {
                    delete it.value();
                    it = table.erase( it );
                }
                else
                {
                    ++it;
                }
            }

            cache->m_rhiTable.removeAll( rhi );
        }

        QHash< TextureHashKey, GradientTexture* > m_hashTable;
        QVector< const QRhi* > m_rhiTable;
    };
}

namespace
{
    class GradientMaterial : public QskGradientMaterial
    {
      public:
        GradientMaterial( QGradient::Type type )
            : QskGradientMaterial( type )
        {
            setFlag( Blending | RequiresFullMatrix );

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
            setFlag( QSGMaterial::SupportsRhiShader, true );
#endif
        }

        int compare( const QSGMaterial* other ) const override
        {
            const auto mat = static_cast< const GradientMaterial* >( other );

            if ( ( spread() == mat->spread() ) && ( stops() == mat->stops() ) )
                return 0;

            return QSGMaterial::compare( other );
        }

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
        // make Qt 5/6 APIs matchaing

        QSGMaterialShader* createShader(
            QSGRendererInterface::RenderMode ) const override final
        {
            return createShader();
        }

        virtual QSGMaterialShader* createShader() const = 0;
#endif
    };

#ifdef SHADER_GL

    class GradientShaderGL : public QSGMaterialShader
    {
      public:
        void setShaderFiles( const char* name )
        {
            static const QString root( ":/qskinny/shaders/" );

            setShaderSourceFile( QOpenGLShader::Vertex, root + name + ".vert" );
            setShaderSourceFile( QOpenGLShader::Fragment, root + name + ".frag" );
        }

        void initialize() override
        {
            m_opacityId = program()->uniformLocation( "opacity" );
            m_matrixId = program()->uniformLocation( "matrix" );
        }

        void updateState( const RenderState& state,
            QSGMaterial* newMaterial, QSGMaterial* ) override final
        {
            auto p = program();
            auto material = static_cast< GradientMaterial* >( newMaterial );

            if ( state.isOpacityDirty() )
                p->setUniformValue( m_opacityId, state.opacity() );

            if ( state.isMatrixDirty() )
                p->setUniformValue(m_matrixId, state.combinedMatrix() );

            updateUniformValues( material );

            auto texture = TextureCache::instance()->texture(
                nullptr, material->stops(), material->spread() );
            texture->bind();
        }

        char const* const* attributeNames() const override final
        {
            static const char* const attr[] = { "vertexCoord", nullptr };
            return attr;
        }

        virtual void updateUniformValues( const GradientMaterial* ) = 0;

      protected:
        int m_opacityId = -1;
        int m_matrixId = -1;
    };
#endif

#ifdef SHADER_RHI
    class GradientShaderRhi : public RhiShader
    {
      public:
        void setShaderFiles( const char* name )
        {
            static const QString root( ":/qskinny/shaders/" );

            setShaderFileName( VertexStage, root + name + ".vert.qsb" );
            setShaderFileName( FragmentStage, root + name + ".frag.qsb" );
        }

        void updateSampledImage( RenderState& state, int binding,
            QSGTexture* textures[], QSGMaterial* newMaterial, QSGMaterial*) override final
        {
            if ( binding != 1 )
                return;

            auto material = static_cast< const GradientMaterial* >( newMaterial );

            auto texture = TextureCache::instance()->texture(
                state.rhi(), material->stops(), material->spread() );

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
            texture->updateRhiTexture( state.rhi(), state.resourceUpdateBatch() );
#else
            texture->commitTextureOperations( state.rhi(), state.resourceUpdateBatch() );
#endif

            textures[0] = texture;
        }
    };
#endif
}

namespace
{
    class LinearMaterial final : public GradientMaterial
    {
      public:
        LinearMaterial()
            : GradientMaterial( QGradient::LinearGradient )
        {
        }

        bool setGradient( const QRectF& rect, const QLinearGradient& gradient )
        {
            bool changed = false;

            if ( gradient.stops() != stops() )
            {
                setStops( gradient.stops() );
                changed = true;
            }

            if ( gradient.spread() != spread() )
            {
                setSpread( gradient.spread() );
                changed = true;
            }

#if 0
            QTransform transform( rect.width(), 0, 0, rect.height(), rect.x(), rect.y());
#endif

            const QVector4D gradientRect(
                rect.left() + gradient.start().x() * rect.width(),
                rect.top() + gradient.start().y() * rect.height(), 
                gradient.finalStop().x() * rect.width(),
                gradient.finalStop().y() * rect.height() );

            if ( m_gradientRect != gradientRect )
            {
                m_gradientRect = gradientRect;
                changed = true;
            }

            return changed;
        }

        QSGMaterialType* type() const override
        {
            static QSGMaterialType type;
            return &type;
        }

        int compare( const QSGMaterial* other ) const override
        {
            const auto mat = static_cast< const LinearMaterial* >( other );

            if ( m_gradientRect != mat->m_gradientRect )
                return QSGMaterial::compare( other );
            else
                return GradientMaterial::compare( other );
        }

        QSGMaterialShader* createShader() const override;

        QVector4D m_gradientRect;
    };

#ifdef SHADER_GL
    class LinearShaderGL final : public GradientShaderGL
    {
      public:
        LinearShaderGL()
        {
            setShaderFiles( "gradientlinear" );
        }

        void initialize() override
        {
            GradientShaderGL::initialize();
            m_rectId = program()->uniformLocation( "rect" );
        }

        void updateUniformValues( const GradientMaterial* newMaterial ) override
        {
            auto material = static_cast< const LinearMaterial* >( newMaterial );
            program()->setUniformValue( m_rectId, material->m_gradientRect );
        }

      private:
        int m_rectId = -1;
    };
#endif

#ifdef SHADER_RHI
    class LinearShaderRhi final : public GradientShaderRhi
    {
      public:
        LinearShaderRhi()
        {
            setShaderFiles( "gradientlinear" );
        }

        bool updateUniformData( RenderState& state,
            QSGMaterial* newMaterial, QSGMaterial* oldMaterial ) override
        {
            auto matNew = static_cast< LinearMaterial* >( newMaterial );
            auto matOld = static_cast< LinearMaterial* >( oldMaterial );

            Q_ASSERT( state.uniformData()->size() >= 84 );

            auto data = state.uniformData()->data();
            bool changed = false;

            if ( state.isMatrixDirty() )
            {
                const auto matrix = state.combinedMatrix();
                memcpy( data + 0, matrix.constData(), 64 );

                changed = true;
            }

            if ( matOld == nullptr || matNew->m_gradientRect != matOld->m_gradientRect )
            {
                memcpy( data + 64, &matNew->m_gradientRect, 16 );
                changed = true;
            }

            if ( state.isOpacityDirty() )
            {
                const float opacity = state.opacity();
                memcpy( data + 80, &opacity, 4 );

                changed = true;
            }

            return changed;
        }
    };
#endif

    QSGMaterialShader* LinearMaterial::createShader() const
    {
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
        if ( !( flags() & QSGMaterial::RhiShaderWanted ) )
            return new LinearShaderGL;
#endif
        return new LinearShaderRhi;
    }
}

namespace
{
    class RadialMaterial final : public GradientMaterial
    {
      public:
        RadialMaterial()
            : GradientMaterial( QGradient::RadialGradient )
        {
        }

        QSGMaterialType* type() const override
        {
            static QSGMaterialType type;
            return &type;
        }

        bool setGradient( const QRectF& rect, const QRadialGradient& gradient )
        {
            bool changed = false;

            if ( gradient.stops() != stops() )
            {
                setStops( gradient.stops() );
                changed = true;
            }

            if ( gradient.spread() != spread() )
            {
                setSpread( gradient.spread() );
                changed = true;
            }

            const auto& center = gradient.center();
            const auto r = gradient.radius();

            const QVector2D pos(
                rect.left() + center.x() * rect.width(),
                rect.top() + center.y() * rect.height() );

            const QVector2D radius( r * rect.width(), r * rect.height() );

            if ( ( pos != m_center ) || ( m_radius != radius ) )
            {
                m_center = pos;
                m_radius = radius;

                changed = true;
            }

            return changed;
        }

        int compare( const QSGMaterial* other ) const override
        {
            const auto mat = static_cast< const RadialMaterial* >( other );

            if ( ( m_center != mat->m_center ) || ( m_radius != mat->m_radius ) )
            {
                return QSGMaterial::compare( other );
            }
            else
            {
                return GradientMaterial::compare( other );
            }
        }

        QSGMaterialShader* createShader() const override;

        QVector2D m_center;
        QVector2D m_radius;
    };

#ifdef SHADER_GL
    class RadialShaderGL final : public GradientShaderGL
    {
      public:
        RadialShaderGL()
        {
            setShaderFiles( "gradientradial" );
        }

        void initialize() override
        {
            GradientShaderGL::initialize();

            auto p = program();

            m_centerCoordId = p->uniformLocation( "centerCoord" );
            m_radiusId = p->uniformLocation( "radius" );
        }

        void updateUniformValues( const GradientMaterial* newMaterial ) override
        {
            auto material = static_cast< const RadialMaterial* >( newMaterial );

            auto p = program();

            p->setUniformValue( m_centerCoordId, material->m_center );
            p->setUniformValue( m_radiusId, material->m_radius );
        }

      private:
        int m_centerCoordId = -1;
        int m_radiusId = -1;
    };
#endif

#ifdef SHADER_RHI
    class RadialShaderRhi final : public GradientShaderRhi
    {
      public:
        RadialShaderRhi()
        {
            setShaderFiles( "gradientradial" );
        }

        bool updateUniformData( RenderState& state,
            QSGMaterial* newMaterial, QSGMaterial* oldMaterial) override
        {
            auto matNew = static_cast< RadialMaterial* >( newMaterial );
            auto matOld = static_cast< RadialMaterial* >( oldMaterial );

            Q_ASSERT( state.uniformData()->size() >= 84 );

            auto data = state.uniformData()->data();
            bool changed = false;

            if ( state.isMatrixDirty() )
            {
                const auto matrix = state.combinedMatrix();
                memcpy( data + 0, matrix.constData(), 64 );

                changed = true;
            }

            if ( matOld == nullptr || matNew->m_center != matOld->m_center )
            {
                memcpy( data + 64, &matNew->m_center, 8 );
                changed = true;
            }

            if ( matOld == nullptr || matNew->m_radius != matOld->m_radius )
            {
                memcpy( data + 72, &matNew->m_radius, 8 );
                changed = true;
            }

            if ( state.isOpacityDirty() )
            {
                const float opacity = state.opacity();
                memcpy( data + 80, &opacity, 4 );

                changed = true;
            }

            return changed;
        }
    };
#endif

    QSGMaterialShader* RadialMaterial::createShader() const
    {
#ifdef SHADER_GL
        if ( !( flags() & QSGMaterial::RhiShaderWanted ) )
            return new RadialShaderGL;
#endif

        return new RadialShaderRhi;
    }
}

namespace
{
    class ConicMaterial final : public GradientMaterial
    {
      public:
        ConicMaterial()
            : GradientMaterial( QGradient::ConicalGradient )
        {
        }

        QSGMaterialType* type() const override
        {
            static QSGMaterialType type;
            return &type;
        }

        bool setGradient( const QRectF& rect, const QConicalGradient& gradient, qreal spanAngle )
        {
            bool changed = false;

            if ( gradient.stops() != stops() )
            {
                setStops( gradient.stops() );
                changed = true;
            }

            if ( gradient.spread() != spread() )
            {
                setSpread( gradient.spread() );
                changed = true;
            }

            const QVector2D center(
                rect.left() + gradient.center().x() * rect.width(),
                rect.top() + gradient.center().y() * rect.height() );

            // Angles as ratio of a rotation

            float start = fmod( gradient.angle(), 360.0 ) / 360.0;
            if ( start < 0.0)
                start += 1.0;

            const float span = fmod( spanAngle, 360.0 ) / 360.0;

            if ( center != m_center )
            {
                m_center = center;
                changed = true;
            }

            if ( ( start != m_start ) || ( span != m_span ) )
            {
                m_start = start;
                m_span = span;

                changed = true;
            }

            return changed;
        }

        int compare( const QSGMaterial* other ) const override
        {
            const auto mat = static_cast< const ConicMaterial* >( other );

            if ( ( m_center != mat->m_center )
                || qskFuzzyCompare( m_start, mat->m_start )
                || qskFuzzyCompare( m_span, mat->m_span ) )
            {
                return QSGMaterial::compare( other );
            }

            return GradientMaterial::compare( other );
        }

        QSGMaterialShader* createShader() const override;

        QVector2D m_center;
        float m_start = 0.0;
        float m_span = 1.0;
    };

#ifdef SHADER_GL
    class ConicShaderGL final : public GradientShaderGL
    {
      public:
        ConicShaderGL()
        {
            setShaderFiles( "gradientconic" );
        }

        void initialize() override
        {
            GradientShaderGL::initialize();

            m_centerCoordId = program()->uniformLocation( "centerCoord" );
            m_startId = program()->uniformLocation( "start" );
            m_spanId = program()->uniformLocation( "span" );
        }

        void updateUniformValues( const GradientMaterial* newMaterial ) override
        {
            auto material = static_cast< const ConicMaterial* >( newMaterial );

            program()->setUniformValue( m_centerCoordId, material->m_center );
            program()->setUniformValue( m_startId, material->m_start );
            program()->setUniformValue( m_spanId, material->m_span );
        }

      private:
        int m_centerCoordId = -1;
        int m_startId = -1;
        int m_spanId = -1;
    };
#endif

#ifdef SHADER_RHI
    class ConicShaderRhi final : public GradientShaderRhi
    {
      public:
        ConicShaderRhi()
        {
            setShaderFiles( "gradientconic" );
        }

        bool updateUniformData( RenderState& state,
            QSGMaterial* newMaterial, QSGMaterial* oldMaterial ) override
        {
            auto matNew = static_cast< ConicMaterial* >( newMaterial );
            auto matOld = static_cast< ConicMaterial* >( oldMaterial );

            Q_ASSERT( state.uniformData()->size() >= 84 );

            auto data = state.uniformData()->data();
            bool changed = false;

            if ( state.isMatrixDirty() )
            {
                const auto matrix = state.combinedMatrix();
                memcpy( data + 0, matrix.constData(), 64 );

                changed = true;
            }

            if ( matOld == nullptr || matNew->m_center != matOld->m_center )
            {
                memcpy( data + 64, &matNew->m_center, 8 );
                changed = true;
            }

            if ( matOld == nullptr || matNew->m_start != matOld->m_start )
            {
                memcpy( data + 72, &matNew->m_start, 4 );
                changed = true;
            }

            if ( matOld == nullptr || matNew->m_span != matOld->m_span )
            {
                memcpy( data + 76, &matNew->m_span, 4 );
                changed = true;
            }

            if ( state.isOpacityDirty() )
            {
                const float opacity = state.opacity();
                memcpy( data + 80, &opacity, 4 );

                changed = true;
            }

            return changed;
        }
    };
#endif

    QSGMaterialShader* ConicMaterial::createShader() const
    {
#ifdef SHADER_GL
        if ( !( flags() & QSGMaterial::RhiShaderWanted ) )
            return new ConicShaderGL;
#endif
        return new ConicShaderRhi;
    }
}

QskGradientMaterial::QskGradientMaterial( QGradient::Type type )
    : m_gradientType( type )
{
}

template< typename Material >
inline Material* qskEnsureMaterial( QskGradientMaterial* material )
{
    if ( material == nullptr )
        material = new Material();

    return static_cast< Material* >( material );
}

bool QskGradientMaterial::updateGradient(
    const QRectF& rect, const QGradient* g, qreal extraValue )
{
    Q_ASSERT( g );

    if ( g == nullptr )
        return false;

    auto& gradient = *g;

    Q_ASSERT( gradient.type() == m_gradientType );

    if ( gradient.type() != m_gradientType )
        return false;

    switch ( static_cast< int >( gradient.type() ) )
    {
        case QGradient::LinearGradient:
        {
            auto material = static_cast< LinearMaterial* >( this );
            return material->setGradient( rect,
                *reinterpret_cast< const QLinearGradient* >( g ) );
        }

        case QGradient::RadialGradient:
        {
            auto material = static_cast< RadialMaterial* >( this );
            return material->setGradient( rect,
                *reinterpret_cast< const QRadialGradient* >( g ) );
        }

        case QGradient::ConicalGradient:
        {
            auto material = static_cast< ConicMaterial* >( this );
            return material->setGradient( rect,
                *reinterpret_cast< const QConicalGradient* >( g ), extraValue );
        }
    }

    return false;
}

QskGradientMaterial* QskGradientMaterial::createMaterial( QGradient::Type gradientType )
{
    switch ( gradientType )
    {
        case QGradient::LinearGradient:
            return new LinearMaterial();

        case QGradient::RadialGradient:
            return new RadialMaterial();

        case QGradient::ConicalGradient:
            return new ConicMaterial();

        default:
            return nullptr;
    }
}
