#include "QskGradientMaterial.h"
#include "QskFunctions.h"
#include "QskRgbValue.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qrhi_p.h>
#include <private/qdrawhelper_p.h>
#include <private/qsgplaintexture_p.h>
QSK_QT_PRIVATE_END

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

/*
    The shaders for the gradient are implemented in the quickshapes module
    Beside them we do not need this module.
 */

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )

    QSK_QT_PRIVATE_BEGIN
        #ifndef signals
            #define signals Q_SIGNALS
        #endif
        #include <private/qquickshape_p.h>
    QSK_QT_PRIVATE_END

    static void QQuickShapes_initializeModule()
    {
        QQuickShapesModule::defineModule();
    }

#else
    extern void QQuickShapes_initializeModule();
#endif

Q_CONSTRUCTOR_FUNCTION( QQuickShapes_initializeModule )

namespace
{
    /*
        Qt offers QGradient compatible shaders in the quickshapes module.
        So why reimplementing the wheel ...

        Those shaders ( f.e lineargradient.frag ) want to have a lookup table
        with the interpolated colors, being uploaded as a texture.
        A 1 dimensional 256x1 texture is how Chrome, Firefox, and Android render
        gradients - so let's do the same.
     */
    class GradientImage : public QImage
    {
      public:
        GradientImage( const QGradientStops& stops )
            : QImage( 256, 1, QImage::Format_RGBA8888_Premultiplied )
        {
            constexpr int numColors = 256;
            auto colorTable = reinterpret_cast< uint* >( bits() );

            int index1, index2;
            QRgb rgb1, rgb2;

            index1 = index2 = qRound( stops[0].first * numColors );
            rgb1 = rgb2 = stops[0].second.rgba();

            if ( index1 > 0 )
            {
                const auto v = value( rgb1 );

                for ( int i = 0; i < index1; i++ )
                    colorTable[i] = v;
            }

            for ( int i = 1; i < stops.count(); i++ )
            {
                const auto& stop = stops[i];

                index2 = qRound( stop.first * numColors );
                rgb2 = stop.second.rgba();

                const auto n = index2 - index1;

                for ( int j = 0; j < n; j++ )
                {
                    const auto rgb = QskRgb::interpolated( rgb1, rgb2, qreal( j ) / n );
                    colorTable[ index1 + j] = value( rgb );
                }

                index1 = index2;
                rgb1 = rgb2;
            }

            if ( index1 < numColors - 1 )
            {
                const auto v = value( rgb1 );

                for ( int i = index1; i < numColors ; i++ )
                    colorTable[i] = v;
            }
        }

      private:
        inline uint value( const QRgb rgb ) const
        {
            return ARGB2RGBA( qPremultiply( rgb ) );
        }
    };

    class GradientTexture : public QSGPlainTexture
    {
      public:
        GradientTexture( const QGradientStops& stops, QGradient::Spread spread )
        {
            setImage( GradientImage( stops ) );

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
                    For OpenGL we coud fiddle around with QOpenGLSharedResource
                    while with RHI we would have QRhi::addCleanupCallback

                    But let's keep things simple for the moment. TODO ...
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
            }

            return texture;
        }

      private:
        static void cleanup()
        {
            delete instance();
        }

        QHash< TextureHashKey, GradientTexture* > m_hashTable;
    };
}

namespace
{
    class GradientMaterial : public QskGradientMaterial
    {
      public:
        GradientMaterial( QGradient::Type type,
                const QGradientStops& stops, QGradient::Spread spread )
            : QskGradientMaterial( type, stops, spread )
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
            static const QString root( ":/qt-project.org/shapes/shaders/" );

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
            static const QString root( ":/qt-project.org/shapes/shaders_ng/" );

            setShaderFileName( VertexStage, root + name + ".vert.qsb" );
            setShaderFileName( FragmentStage, root + name + ".frag.qsb" );
        }

        void updateSampledImage( RenderState& state, int binding,
            QSGTexture** texture, QSGMaterial* newMaterial, QSGMaterial*) override final
        {
            if ( binding != 1 )
                return;

            auto material = static_cast< const GradientMaterial* >( newMaterial );

            auto txt = TextureCache::instance()->texture(
                state.rhi(), material->stops(), material->spread() );

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
            txt->updateRhiTexture( state.rhi(), state.resourceUpdateBatch() );
#else
            txt->commitTextureOperations( state.rhi(), state.resourceUpdateBatch() );
#endif

            *texture = txt;
        }
    };
#endif
}

namespace
{
    class LinearMaterial final : public GradientMaterial
    {
      public:
        LinearMaterial(
                const QGradientStops& stops, QGradient::Spread spread,
                const QPointF& start, const QPointF& stop )
            : GradientMaterial( QGradient::LinearGradient, stops, spread )
            , m_start( start )
            , m_stop( stop )
        {
        }

        QSGMaterialShader* createShader() const override;

        QSGMaterialType* type() const override
        {
            static QSGMaterialType type;
            return &type;
        }

        int compare( const QSGMaterial* other ) const override
        {
            const auto mat = static_cast< const LinearMaterial* >( other );

            if ( ( m_start != mat->m_start ) || ( m_stop != mat->m_stop ) )
                return QSGMaterial::compare( other );
            else
                return GradientMaterial::compare( other );
        }

        const QVector2D m_start;
        const QVector2D m_stop;
    };

#ifdef SHADER_GL
    class LinearShaderGL final : public GradientShaderGL
    {
      public:
        LinearShaderGL()
        {
            setShaderFiles( "lineargradient" );
        }

        void initialize() override
        {
            GradientShaderGL::initialize();

            m_startId = program()->uniformLocation( "gradStart" );
            m_stopId = program()->uniformLocation( "gradEnd" );
        }

        void updateUniformValues( const GradientMaterial* newMaterial ) override
        {
            auto material = static_cast< const LinearMaterial* >( newMaterial );

            program()->setUniformValue( m_startId, material->m_start );
            program()->setUniformValue( m_stopId, material->m_stop );
        }

      private:
        int m_startId = -1;
        int m_stopId = -1;
    };
#endif

#ifdef SHADER_RHI
    class LinearShaderRhi final : public GradientShaderRhi
    {
      public:
        LinearShaderRhi()
        {
            setShaderFiles( "lineargradient" );
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

            if ( matOld == nullptr || matNew->m_start != matOld->m_start )
            {
                memcpy( data + 64, &matNew->m_start, 8 );
                changed = true;
            }

            if ( matOld == nullptr || matNew->m_stop != matOld->m_stop )
            {
                memcpy( data + 72, &matNew->m_stop, 8 );
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
        RadialMaterial(
                const QGradientStops& stops, QGradient::Spread spread,
                const QPointF& center, qreal centerRadius,
                const QPointF& focalPoint, qreal focalRadius )
            : GradientMaterial( QGradient::RadialGradient, stops, spread )
            , m_center( center )
            , m_focalPoint( focalPoint )
            , m_centerRadius( centerRadius )
            , m_focalRadius( focalRadius )
        {
        }

        QSGMaterialShader* createShader() const override;

        QSGMaterialType* type() const override
        {
            static QSGMaterialType type;
            return &type;
        }

        int compare( const QSGMaterial* other ) const override
        {
            const auto mat = static_cast< const RadialMaterial* >( other );

            if ( ( m_center != mat->m_center )
                || ( m_focalPoint != mat->m_focalPoint )
                || qskFuzzyCompare( m_centerRadius, mat->m_centerRadius )
                || qskFuzzyCompare( m_focalRadius, mat->m_focalRadius ) )
            {
                return QSGMaterial::compare( other );
            }
            else
            {
                return GradientMaterial::compare( other );
            }
        }

        const QVector2D m_center;
        const QVector2D m_focalPoint;
        const float m_centerRadius;
        const float m_focalRadius;
    };

#ifdef SHADER_GL
    class RadialShaderGL final : public GradientShaderGL
    {
      public:
        RadialShaderGL()
        {
            setShaderFiles( "radialgradient" );
        }

        void initialize() override
        {
            GradientShaderGL::initialize();

            auto p = program();

            m_centerPointId = p->uniformLocation( "translationPoint" );
            m_focalPointId = p->uniformLocation( "focalToCenter" );
            m_centerRadiusId = p->uniformLocation( "centerRadius" );
            m_focalRadiusId = p->uniformLocation( "focalRadius" );
        }

        void updateUniformValues( const GradientMaterial* newMaterial ) override
        {
            auto material = static_cast< const RadialMaterial* >( newMaterial );

            auto p = program();

            p->setUniformValue( m_centerPointId, material->m_center );
            p->setUniformValue( m_centerRadiusId, material->m_centerRadius);
            p->setUniformValue( m_focalRadiusId, material->m_focalRadius);
            p->setUniformValue( m_focalPointId, material->m_focalPoint);
        }

      private:
        int m_centerPointId = -1;
        int m_focalPointId = -1;
        int m_centerRadiusId = -1;
        int m_focalRadiusId = -1;
    };
#endif

#ifdef SHADER_RHI
    class RadialShaderRhi final : public GradientShaderRhi
    {
      public:
        RadialShaderRhi()
        {
            setShaderFiles( "radialgradient" );
        }

        bool updateUniformData( RenderState& state,
            QSGMaterial* newMaterial, QSGMaterial* oldMaterial) override
        {
            auto matNew = static_cast< RadialMaterial* >( newMaterial );
            auto matOld = static_cast< RadialMaterial* >( oldMaterial );

            Q_ASSERT( state.uniformData()->size() >= 92 );

            auto data = state.uniformData()->data();
            bool changed = false;

            if ( state.isMatrixDirty() )
            {
                const auto matrix = state.combinedMatrix();
                memcpy( data + 0, matrix.constData(), 64 );

                changed = true;
            }

            if ( matOld == nullptr || matNew->m_focalPoint != matOld->m_focalPoint )
            {
                memcpy( data + 64, &matNew->m_focalPoint, 8 );
                changed = true;
            }

            if ( matOld == nullptr || matNew->m_focalPoint != matOld->m_focalPoint )
            {
                memcpy( data + 72, &matNew->m_focalPoint, 8 );
                changed = true;
            }

            if ( matOld == nullptr || matNew->m_centerRadius != matOld->m_centerRadius )
            {
                memcpy( data + 80, &matNew->m_centerRadius, 4);
                changed = true;
            }

            if ( matOld == nullptr || matNew->m_focalRadius != matOld->m_focalRadius )
            {
                memcpy( data + 84, &matOld->m_focalRadius, 4 );
                changed = true;
            }

            if ( state.isOpacityDirty() )
            {
                const float opacity = state.opacity();
                memcpy( data + 88, &opacity, 4 );

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
    class ConicalMaterial final : public GradientMaterial
    {
      public:
        ConicalMaterial( const QGradientStops& stops,
                const QPointF& center, qreal angle )
            : GradientMaterial( QGradient::ConicalGradient, stops, QGradient::PadSpread )
            , m_center( center )
            , m_radians( -qDegreesToRadians( angle ) )
        {
        }

        QSGMaterialShader* createShader() const override;

        QSGMaterialType* type() const override
        {
            static QSGMaterialType type;
            return &type;
        }

        int compare( const QSGMaterial* other ) const override
        {
            const auto mat = static_cast< const ConicalMaterial* >( other );

            if ( ( m_center != mat->m_center ) || qskFuzzyCompare( m_radians, mat->m_radians ) )
                return QSGMaterial::compare( other );
            else
                return GradientMaterial::compare( other );
        }

        const QVector2D m_center;
        const float m_radians;
    };

#ifdef SHADER_GL
    class ConicalShaderGL final : public GradientShaderGL
    {
      public:
        ConicalShaderGL()
        {
            setShaderFiles( "conicalgradient" );
        }

        void initialize() override
        {
            GradientShaderGL::initialize();

            m_radiansId = program()->uniformLocation( "angle" );
            m_centerPointId = program()->uniformLocation( "translationPoint" );
        }

        void updateUniformValues( const GradientMaterial* newMaterial ) override
        {
            auto material = static_cast< const ConicalMaterial* >( newMaterial );

            program()->setUniformValue( m_radiansId, material->m_radians );
            program()->setUniformValue( m_centerPointId, material->m_center );
        }

      private:
        int m_radiansId = -1;
        int m_centerPointId = -1;
    };
#endif

#ifdef SHADER_RHI
    class ConicalShaderRhi final : public GradientShaderRhi
    {
      public:
        ConicalShaderRhi()
        {
            setShaderFiles( "conicalgradient" );
        }

        bool updateUniformData( RenderState& state,
            QSGMaterial* newMaterial, QSGMaterial* oldMaterial ) override
        {
            auto matNew = static_cast< ConicalMaterial* >( newMaterial );
            auto matOld = static_cast< ConicalMaterial* >( oldMaterial );

            Q_ASSERT( state.uniformData()->size() >= 80 );

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

            if ( matOld == nullptr || matNew->m_radians != matOld->m_radians )
            {
                memcpy( data + 72, &matNew->m_radians, 4 );
                changed = true;
            }

            if ( state.isOpacityDirty() )
            {
                const float opacity = state.opacity();
                memcpy( data + 76, &opacity, 4 );

                changed = true;
            }

            return changed;
        }
    };
#endif

    QSGMaterialShader* ConicalMaterial::createShader() const
    {
#ifdef SHADER_GL
        if ( !( flags() & QSGMaterial::RhiShaderWanted ) )
            return new ConicalShaderGL;
#endif
        return new ConicalShaderRhi;
    }
}

QskGradientMaterial* QskGradientMaterial::create( const QGradient* gradient )
{
    switch ( static_cast< int >( gradient->type() ) )
    {
        case QGradient::LinearGradient:
        {
            auto linearGradient = static_cast< const QLinearGradient* >( gradient );

            return new LinearMaterial(
                linearGradient->stops(), linearGradient->spread(),
                linearGradient->start(), linearGradient->finalStop() );
        }

        case QGradient::RadialGradient:
        {
            auto radialGradient = static_cast< const QRadialGradient* >( gradient );

            return new RadialMaterial(
                radialGradient->stops(), radialGradient->spread(),
                radialGradient->center(), radialGradient->centerRadius(),
                radialGradient->focalPoint(), radialGradient->focalRadius() );
        }

        case QGradient::ConicalGradient:
        {
            auto conicalGradient = static_cast< const QConicalGradient* >( gradient );

            return new ConicalMaterial( conicalGradient->stops(),
                conicalGradient->center(), conicalGradient->angle() );
        }
    }

    return nullptr;
}


