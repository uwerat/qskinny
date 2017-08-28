/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxMaterial.h"
#include "QskBoxOptions.h"

#include <QSGMaterialShader>
#include <QSGTexture>
#include <QSGGeometryNode>

#include <QGuiApplication>
#include <QImage>
#include <QOpenGLTexture>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QPainter>
#include <QtMath>

#include <unordered_map>

#define SHADER_S(x) #x
#define SHADER(x) SHADER_S(x)

static qreal qskDevicePixelRatio = 0;

class QskBoxMaterial::TextureData
{
public:
    TextureData() :
        referenceCount( 1 )
    {
    }

    QSizeF size;
    QRectF coordinates;
    QRect bounds;
    qint64 referenceCount;
};

namespace
{
    class TextureCache : public std::unordered_map< uint, QskBoxMaterial::TextureData* >
    {
    public:
        iterator findByData( QskBoxMaterial::TextureData* data )
        {
            auto compare = 
                [ data ] ( const std::pair< uint, QskBoxMaterial::TextureData* >& item )
                {
                    return item.second == data;
                };

            return std::find_if( begin(), end(), compare );
        }
    };

    static TextureCache qskTextureCache;
}

namespace
{
    class TextureAtlas
    {
    public:
        TextureAtlas() :
            m_texture( QOpenGLTexture::Target2D )
        {
            auto context = QOpenGLContext::currentContext();
            Q_ASSERT_X( context, "QskSymbolMaterial",
                "Did you access qskAtlas from the wrong thread? It should be only "
                "called from the Scene Graph thread." );

            QObject::connect( context, &QOpenGLContext::aboutToBeDestroyed,
                &TextureAtlas::cleanup );

            GLint size;
            context->functions()->glGetIntegerv( GL_MAX_TEXTURE_SIZE, &size );
            m_texture.setSize( size, size );
            m_texture.setAutoMipMapGenerationEnabled( false );
            m_texture.setFormat( QOpenGLTexture::RGBA8_UNorm );
            m_texture.setMinMagFilters( QOpenGLTexture::Nearest, QOpenGLTexture::Nearest );
            m_texture.setWrapMode( QOpenGLTexture::ClampToEdge );
            m_texture.allocateStorage( QOpenGLTexture::RGBA, QOpenGLTexture::UInt8 );
        }

        void insert( QskBoxMaterial::TextureData* data );
        void remove( const QskBoxMaterial::TextureData* data );

        inline void bindTexture()
        {
            m_texture.bind();
        }

        inline void releaseTexture()
        {
            m_texture.release();
        }

        static void cleanup();

    private:
        QOpenGLTexture m_texture;
        QRegion m_region;
    };

    Q_GLOBAL_STATIC( TextureAtlas, qskAtlas )

    void TextureAtlas::cleanup()
    {
        if ( qskAtlas.exists() && !qskAtlas.isDestroyed() )
            qskAtlas->m_texture.destroy();
    }

    void TextureAtlas::insert( QskBoxMaterial::TextureData* data )
    {
        const qreal width = data->size.width() * qskDevicePixelRatio;
        const qreal height = data->size.height() * qskDevicePixelRatio;

        const int bucketWidth = int( std::ceil( width ) );
        const int bucketHeight = int( std::ceil( height ) );

        const int boundsWidth = m_texture.width();
        const int boundsHeight = m_texture.height();

        if ( bucketWidth > boundsWidth || bucketHeight > boundsHeight )
        {
            qWarning() << "Requested texture is larger than maximum size";
            return;
        }

        const auto freeRegion = QRegion( 0, 0, boundsWidth, boundsHeight ).subtracted( m_region );
        for ( const auto& rect : freeRegion.rects() )
        {
            const QRect bounds( rect.x(), rect.y(), bucketWidth, bucketHeight );
            if ( rect.contains( bounds ) )
            {
                m_region += bounds;

                data->bounds = bounds;
                data->coordinates = QRectF(
                    qreal( bounds.x() ) / boundsWidth,
                    qreal( bounds.y() ) / boundsHeight,
                    width / boundsWidth,
                    height / boundsHeight );

                return;
            }
        }

        qWarning() << "Texture atlas is full. Check that the application is "
            "releasing unused texture data.";
    }

    void TextureAtlas::remove( const QskBoxMaterial::TextureData* data )
    {
        m_region -= data->bounds;
    }
}

namespace
{
    static QPainterPath qskBorderPath( QMarginsF borders, QMarginsF marginsExtra,
        const QskBoxOptions& options, bool inner )
    {
        const auto& metrics = options.metrics;

        qreal topLeftX = metrics.radius( Qt::TopLeftCorner ).width();
        qreal topRightX = metrics.radius( Qt::TopRightCorner ).width();

        qreal bottomRightX = metrics.radius( Qt::BottomRightCorner ).width();
        qreal bottomLeftX = metrics.radius( Qt::BottomLeftCorner ).width();

        qreal topLeftY = metrics.radius( Qt::TopLeftCorner ).height();
        qreal topRightY = metrics.radius( Qt::TopRightCorner ).height();

        qreal bottomRightY = metrics.radius( Qt::BottomRightCorner ).height();
        qreal bottomLeftY = metrics.radius( Qt::BottomLeftCorner).height();

        const auto spacingX = marginsExtra.left() + marginsExtra.right();
        const auto spacingY = marginsExtra.top() + marginsExtra.bottom();

        auto width = borders.left() + std::max( topLeftX, bottomLeftX )
            + spacingX + borders.right() + std::max( topRightX, bottomRightX );

        auto height = borders.top() + std::max( topLeftY, topRightY )
            + spacingY + borders.bottom() + std::max( bottomLeftY, bottomRightY );

        if ( inner )
        {
            topLeftX = std::max( topLeftX - borders.left(), 0.0 );
            topLeftY = std::max( topLeftY - borders.top(), 0.0 );
            topRightX = std::max( topRightX - borders.right(), 0.0 );
            topRightY = std::max( topRightY - borders.top(), 0.0 );
            bottomLeftX = std::max( bottomLeftX - borders.left(), 0.0 );
            bottomLeftY = std::max( bottomLeftY - borders.bottom(), 0.0 );
            bottomRightX = std::max( bottomRightX - borders.right(), 0.0 );
            bottomRightY = std::max( bottomRightY - borders.bottom(), 0.0 );
        }
        else
        {
            borders = QMarginsF();
        }

        QPainterPath path;

        path.moveTo( borders.left(), borders.top() + topLeftY );

        path.arcTo( borders.left(), borders.top(), topLeftX * 2, topLeftY * 2, 180, -90 );
        path.lineTo( width - topRightX - borders.right(), path.currentPosition().y() );

        path.arcTo( path.currentPosition().x() - topRightX, path.currentPosition().y(),
            topRightX * 2, topRightY * 2, 90, -90 );
        path.lineTo( path.currentPosition().x(), height - bottomRightY - borders.bottom() );

        path.arcTo( path.currentPosition().x() - bottomRightX * 2,
            path.currentPosition().y() - bottomRightY,
            bottomRightX * 2, bottomRightY * 2, 0, -90 );
        path.lineTo( borders.left() + bottomLeftX, path.currentPosition().y() );

        path.arcTo( path.currentPosition().x() - bottomLeftX,
            path.currentPosition().y() - bottomLeftY * 2,
            bottomLeftX * 2, bottomLeftY * 2, -90, -90 );
        path.closeSubpath();

        return path;
    }

    static inline void swizzle( const QImage& inputImage, QImage& outputImage, int bit )
    {
        Q_ASSERT( inputImage.size() == outputImage.size() );
        for ( int i = 0; i < inputImage.height(); ++i )
        {
            auto input = inputImage.scanLine(i);
            auto output = outputImage.scanLine(i);
            for ( int j = 0; j < inputImage.width(); ++j )
                output[j * 4 + bit] = input[j];
        }
    }

    QImage qskCreateImage( const QSize& size, const QVector< QPainterPath >& paths )
    {
        // Image to contain all layers
        QImage textureImage( size, QImage::Format_RGBA8888 );
        textureImage.fill( Qt::black );

        // Image to render single channels
        QImage channelImage( size, QImage::Format_Alpha8 );
        channelImage.setDevicePixelRatio( qskDevicePixelRatio );

        int currentColor = 0;
        for ( int i = 0; i < paths.size(); i++ )
        {
            channelImage.fill( Qt::transparent );

            QPainter painter( &channelImage );

            if ( size.width() > qskDevicePixelRatio
                 || size.height() > qskDevicePixelRatio )
            {
                painter.setRenderHint( QPainter::Antialiasing );
            }

            QBrush brush( Qt::black );
            if ( i == 2 )
            {
                static QBrush shadowBrush;
                if ( shadowBrush.style() == Qt::NoBrush )
                {
                    QRadialGradient gradient( 0.5, 0.5, 0.5 );
                    gradient.setCoordinateMode( QGradient::ObjectBoundingMode );
                    gradient.setStops( { { 0.0, Qt::black }, { 1.0, Qt::transparent } } );
                    shadowBrush = gradient;
                }

                brush = shadowBrush;
            }

            painter.fillPath( paths[i], brush );
            swizzle( channelImage, textureImage, currentColor++ );

            Q_ASSERT(currentColor < 4);
        }

        return textureImage;
    }

    static QVector< QPainterPath > qskCreateTexturePaths(
        const QskBoxOptions& options )
    {
        QVector< QPainterPath > paths;

        const QMarginsF allMargins = options.unitedMargins();
        const QMarginsF padding = options.padding();
        const QMarginsF borders = options.metrics.widths();

        const QMarginsF extra = allMargins - borders - options.shadows - padding;

        const auto width = allMargins.left() + allMargins.right();
        const auto height = allMargins.top() + allMargins.bottom();

        const QSize size( std::ceil( width ), std::ceil( height ) );
        const auto translateX = options.shadows.left() + ( size.width() - width ) * 0.5f;
        const auto translateY = options.shadows.top() + ( size.height() - height ) * 0.5f;

        {
            auto outerPath = qskBorderPath( borders, extra, options, false );
            outerPath = outerPath.translated( translateX, translateY );

            paths.append( outerPath );
        }

        if ( !borders.isNull() )
        {
            auto innerPath = qskBorderPath( borders, extra, options, true );
            innerPath = innerPath.translated( translateX, translateY );

            paths.append( innerPath );
        }
        else
        {
            paths.prepend( QPainterPath() ); // make outerPath the innerPath
        }

        if ( !options.shadows.isNull() )
        {
            auto shadowPath = qskBorderPath(
                borders + options.shadows, extra, options, false );

            shadowPath = shadowPath.translated( translateX - options.shadows.left(),
                translateY - options.shadows.top() );

            paths.append( shadowPath );
        }

        return paths;
    }
}

class QskBoxMaterialShader final : public QSGMaterialShader
{
public:
    virtual const char* const* attributeNames() const override final
    {
        static char const* const attributeNames[] =
        {
            "position", "texture", "background", "foreground", nullptr
        };
        return attributeNames;
    }

    virtual void initialize() override final
    {
        qt_Matrix = program()->uniformLocation( "qt_Matrix" );
        qt_Opacity = program()->uniformLocation( "qt_Opacity" );
    }

    virtual void updateState( const RenderState& state,
        QSGMaterial* next, QSGMaterial* prev ) override final
    {
        Q_UNUSED( next );
        if ( !prev )
            qskAtlas->bindTexture();

        if ( state.isOpacityDirty() )
        {
            program()->setUniformValue( qt_Opacity, state.opacity() );
        }

        if ( state.isMatrixDirty() )
        {
            program()->setUniformValue( qt_Matrix, state.combinedMatrix() );
        }
    }

    virtual const char* vertexShader() const override final
    {
        return SHADER(

            uniform highp mat4 qt_Matrix;
            attribute highp vec4 position;
            attribute highp vec2 texture;
            attribute highp vec4 background;
            attribute highp vec4 foreground;
            varying highp vec2 texCoord;
            varying lowp vec4 color0;
            varying lowp vec4 color1;
            varying lowp vec4 color2;

            void main()
            {
                texCoord = texture;
                color0 = foreground.bgra;
                color1 = background.bgra;
                color2 = foreground.bgra;
                gl_Position = qt_Matrix * position;
            }
        );
    }

    virtual const char* fragmentShader() const override final
    {
        return SHADER(

            varying lowp vec4 color0;
            varying lowp vec4 color1;
            varying lowp vec4 color2;
            uniform lowp float qt_Opacity;
            uniform sampler2D texture;
            varying highp vec2 texCoord;

            void main()
            {
                highp vec4 texel = texture2D( texture, texCoord );
                highp vec4 effect = color2 * texel.r;
                highp vec4 bg = color0 * texel.b;
                highp vec4 fg = color1 * texel.g;
                fg = fg + bg * ( 1.0 - fg.a );
                gl_FragColor = ( fg + effect * ( 1.0 - fg.a ) ) * qt_Opacity;
            }
        );
    }

private:
    int qt_Matrix;
    int qt_Opacity;
};

QskBoxMaterial::QskBoxMaterial() :
    m_data( nullptr )
{
    if ( qskDevicePixelRatio == 0 )
        qskDevicePixelRatio = qGuiApp->devicePixelRatio();

    setFlag( Blending );
}

QskBoxMaterial::~QskBoxMaterial()
{
    releaseTexture();
}

bool QskBoxMaterial::isValid() const
{
    return m_data != nullptr;
}

void QskBoxMaterial::setBoxOptions( const QskBoxOptions& options, uint key )
{
    auto it = qskTextureCache.find( key );
    if ( it != qskTextureCache.cend() )
    {
        if ( m_data == it->second )
            return;

        // De-ref and possibly release texture
        releaseTexture();

        m_data = it->second;
        ++m_data->referenceCount;
    }
    else
    {
        it = qskTextureCache.emplace( key, new TextureData ).first;
        m_data = it->second;

        const QVector< QPainterPath > paths = qskCreateTexturePaths( options );

        const QMarginsF borders = options.unitedMargins();

        m_data->size = QSizeF( borders.left() + borders.right(),
            borders.top() + borders.bottom() );

        qskAtlas->insert( m_data );

        const QImage textureImage = qskCreateImage( m_data->bounds.size(), paths );

        qskAtlas->bindTexture();

        auto gl = QOpenGLContext::currentContext()->functions();

        gl->glTexSubImage2D( GL_TEXTURE_2D, 0,
            m_data->bounds.x(), m_data->bounds.y(),
            m_data->bounds.width(), m_data->bounds.height(),
            QOpenGLTexture::BGRA, GL_UNSIGNED_BYTE, textureImage.constBits() );

        qskAtlas->releaseTexture();
    }
}

QSizeF QskBoxMaterial::textureSize() const
{
    return m_data ? m_data->size : QSizeF();
}

QRectF QskBoxMaterial::textureCoordinates() const
{
    return m_data ? m_data->coordinates : QRectF();
}

int QskBoxMaterial::compare( const QSGMaterial* other ) const
{
    if ( this == other )
        return 0;

    auto material = static_cast< decltype( this ) >( other );
    if ( textureCoordinates() == material->textureCoordinates() )
        return 0;

    return QSGMaterial::compare( other );
}

void QskBoxMaterial::releaseTexture()
{
    if ( m_data == nullptr || --m_data->referenceCount > 0 )
        return;

    qskTextureCache.erase( qskTextureCache.findByData( m_data ) );
    qskAtlas->remove( m_data );

    delete m_data;
    m_data = nullptr;
}

QSGMaterialShader* QskBoxMaterial::createShader() const
{
    return new QskBoxMaterialShader;
}

QSGMaterialType* QskBoxMaterial::type() const
{
    static QSGMaterialType materialType;
    return &materialType;
}
