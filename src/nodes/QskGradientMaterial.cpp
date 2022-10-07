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

        bool setGradient( const QLinearGradient* gradient )
        {
            bool changed = false;

            if ( gradient->stops() != stops() )
            {
                setStops( gradient->stops() );
                changed = true;
            }

            if ( gradient->spread() != spread() )
            {
                setSpread( gradient->spread() );
                changed = true;
            }

            const QVector2D start( gradient->start() );
            const QVector2D stop( gradient->finalStop() );

            if ( m_start != start || m_stop != stop )
            {
                m_start = start;
                m_stop = stop;
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

            if ( ( m_start != mat->m_start ) || ( m_stop != mat->m_stop ) )
                return QSGMaterial::compare( other );
            else
                return GradientMaterial::compare( other );
        }

        QSGMaterialShader* createShader() const override;

        QVector2D m_start;
        QVector2D m_stop;
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
        RadialMaterial()
            : GradientMaterial( QGradient::RadialGradient )
        {
        }

        QSGMaterialType* type() const override
        {
            static QSGMaterialType type;
            return &type;
        }

        bool setGradient( const QRadialGradient* gradient )
        {
            bool changed = false;

            if ( gradient->stops() != stops() )
            {
                setStops( gradient->stops() );
                changed = true;
            }

            if ( gradient->spread() != spread() )
            {
                setSpread( gradient->spread() );
                changed = true;
            }

            const QVector2D focalToCenter( gradient->center() - gradient->focalPoint() );
            const float centerRadius = gradient->centerRadius();

            const QVector2D focalPoint( gradient->focalPoint() );
            const float focalRadius = gradient->focalRadius();

            if ( ( focalPoint != m_focalPoint ) || ( focalRadius != m_focalRadius ) )
            {
                m_focalPoint = focalPoint;
                m_focalRadius = focalRadius;

                changed = true;
            }

            if ( ( focalToCenter != m_focalToCenter ) || ( m_centerRadius != centerRadius ) )
            {
                m_focalToCenter = focalToCenter;
                m_centerRadius = centerRadius;

                changed = true;
            }

            return changed;
        }

        int compare( const QSGMaterial* other ) const override
        {
            const auto mat = static_cast< const RadialMaterial* >( other );

            if ( ( m_focalPoint != mat->m_focalPoint )
                || ( m_focalToCenter != mat->m_focalToCenter )
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

        QSGMaterialShader* createShader() const override;

        QVector2D m_focalPoint;
        QVector2D m_focalToCenter;
        float m_centerRadius = 0.0;
        float m_focalRadius = 0.0;
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

            m_focalPointId = p->uniformLocation( "translationPoint" );
            m_focalToCenterId = p->uniformLocation( "focalToCenter" );
            m_centerRadiusId = p->uniformLocation( "centerRadius" );
            m_focalRadiusId = p->uniformLocation( "focalRadius" );
        }

        void updateUniformValues( const GradientMaterial* newMaterial ) override
        {
            auto material = static_cast< const RadialMaterial* >( newMaterial );

            auto p = program();

            p->setUniformValue( m_focalToCenterId, material->m_focalToCenter );
            p->setUniformValue( m_centerRadiusId, material->m_centerRadius);
            p->setUniformValue( m_focalRadiusId, material->m_focalRadius);
            p->setUniformValue( m_focalPointId, material->m_focalPoint);
        }

      private:
        int m_focalToCenterId = -1;
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

            if ( matOld == nullptr || matNew->m_focalToCenter != matOld->m_focalToCenter )
            {
                memcpy( data + 72, &matNew->m_focalToCenter, 8 );
                changed = true;
            }

            if ( matOld == nullptr || matNew->m_centerRadius != matOld->m_centerRadius )
            {
                memcpy( data + 80, &matNew->m_centerRadius, 4);
                changed = true;
            }

            if ( matOld == nullptr || matNew->m_focalRadius != matOld->m_focalRadius )
            {
                memcpy( data + 84, &matNew->m_focalRadius, 4 );
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

        bool setGradient( const QConicalGradient* gradient )
        {
            bool changed = false;

            if ( gradient->stops() != stops() )
            {
                setStops( gradient->stops() );
                changed = true;
            }

            const QVector2D center( gradient->center() );
            const float radians = -qDegreesToRadians( gradient->angle() );

            if ( center != m_center || radians != m_radians )
            {
                m_center = center;
                m_radians = radians;

                changed = true;
            }

            return changed;
        }

        int compare( const QSGMaterial* other ) const override
        {
            const auto mat = static_cast< const ConicMaterial* >( other );

            if ( ( m_center != mat->m_center ) || qskFuzzyCompare( m_radians, mat->m_radians ) )
                return QSGMaterial::compare( other );
            else
                return GradientMaterial::compare( other );
        }

        QSGMaterialShader* createShader() const override;

        QVector2D m_center;
        float m_radians = 0.0;
    };

#ifdef SHADER_GL
    class ConicShaderGL final : public GradientShaderGL
    {
      public:
        ConicShaderGL()
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
            auto material = static_cast< const ConicMaterial* >( newMaterial );

            program()->setUniformValue( m_radiansId, material->m_radians );
            program()->setUniformValue( m_centerPointId, material->m_center );
        }

      private:
        int m_radiansId = -1;
        int m_centerPointId = -1;
    };
#endif

#ifdef SHADER_RHI
    class ConicShaderRhi final : public GradientShaderRhi
    {
      public:
        ConicShaderRhi()
        {
            setShaderFiles( "conicalgradient" );
        }

        bool updateUniformData( RenderState& state,
            QSGMaterial* newMaterial, QSGMaterial* oldMaterial ) override
        {
            auto matNew = static_cast< ConicMaterial* >( newMaterial );
            auto matOld = static_cast< ConicMaterial* >( oldMaterial );

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

bool QskGradientMaterial::updateGradient( const QGradient* gradient )
{
    Q_ASSERT( gradient && gradient->type() == m_gradientType );

    if ( gradient == nullptr || gradient->type() != m_gradientType )
        return false;

    switch ( static_cast< int >( gradient->type() ) )
    {
        case QGradient::LinearGradient:
        {
            auto material = static_cast< LinearMaterial* >( this );
            return material->setGradient( static_cast< const QLinearGradient* >( gradient ) );
        }

        case QGradient::RadialGradient:
        {
            auto material = static_cast< RadialMaterial* >( this );
            return material->setGradient( static_cast< const QRadialGradient* >( gradient ) );
        }

        case QGradient::ConicalGradient:
        {
            auto material = static_cast< ConicMaterial* >( this );
            return material->setGradient( static_cast< const QConicalGradient* >( gradient ) );
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
