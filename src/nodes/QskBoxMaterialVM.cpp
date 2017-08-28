/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxMaterialVM.h"
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

QSK_QT_PRIVATE_BEGIN

#if QT_VERSION < QT_VERSION_CHECK( 5, 8, 0 )
#include <private/qsgcontext_p.h>
typedef QSGRenderContext RenderContext;
#else
#include <private/qsgdefaultrendercontext_p.h>
typedef QSGDefaultRenderContext RenderContext;
#endif

QSK_QT_PRIVATE_END

#include <unordered_map>

#define SHADER_S(x) #x
#define SHADER(x) SHADER_S(x)

static qreal qskDevicePixelRatio = 0;

namespace
{
    class TextureData
    {
    public:
        TextureData( QSGTexture* texture ):
            texture( texture ),
            referenceCount( 1 )
        {
        }

        QSGTexture* texture;
        qint64 referenceCount;
    };

    class TextureCache: public std::unordered_map< uint, TextureData >
    {
    public:
        iterator findByTexture( QSGTexture* texture )
        {
            auto compare =
                [ texture ]( const std::pair< uint, TextureData >& value )
                {
                    return value.second.texture == texture;
                };

            return std::find_if( begin(), end(), compare );
        }
    };

    static TextureCache qskTextureCache;
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

class QskBoxMaterialShaderVM final : public QSGMaterialShader
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
        QSGMaterialShader::updateState( state, next, prev );

        auto newMaterial = static_cast< QskBoxMaterialVM* >( next );
        auto oldMaterial = static_cast< QskBoxMaterialVM* >( prev );

        if ( !oldMaterial || newMaterial->m_textures.first() != oldMaterial->m_textures.first() )
        {
            newMaterial->m_textures.first()->bind();
        }
        else
        {
            newMaterial->m_textures.first()->updateBindOptions();
        }

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

QskBoxMaterialVM::QskBoxMaterialVM()
{
    if ( qskDevicePixelRatio == 0 )
        qskDevicePixelRatio = qGuiApp->devicePixelRatio();

    setFlag( Blending );
}

QskBoxMaterialVM::~QskBoxMaterialVM()
{
    releaseTextures();
}

void QskBoxMaterialVM::setBoxOptions( const QskBoxOptions& options, uint key )
{
    auto it = qskTextureCache.find( key );
    if ( it != qskTextureCache.cend() )
    {
        auto& textureInfo = it->second;

        textureInfo.referenceCount++;
        m_textures.push_front( textureInfo.texture );
    }
    else
    {
        const QVector< QPainterPath > paths = qskCreateTexturePaths( options );

        const QMarginsF borders = options.unitedMargins();

        const QSize size( std::ceil( borders.left() + borders.right() ),
            std::ceil( borders.top() + borders.bottom() ) );

        const QImage textureImage = qskCreateImage( size, paths );

        releaseTextures();

        QSGRenderContext* renderContext =
            RenderContext::from( QOpenGLContext::currentContext() );

        auto texture = renderContext->createTexture( textureImage,
            QSGRenderContext::CreateTexture_Atlas );

        texture->setFiltering( qskDevicePixelRatio > 1 
            ? QSGTexture::Linear : QSGTexture::Nearest );

        m_textures.prepend( texture );
        qskTextureCache.emplace( key, TextureData( texture ) );

        // Release old textures

        constexpr int maxTextureSize = 64;
        while ( m_textures.length() > maxTextureSize )
            releaseTexture( m_textures.takeLast() );
    }
}

bool QskBoxMaterialVM::isValid() const
{
    return !m_textures.isEmpty();
}

QSizeF QskBoxMaterialVM::textureSize() const
{
    if ( m_textures.isEmpty() )
        return QSizeF( 0, 0 );

    return m_textures.first()->textureSize() / qskDevicePixelRatio;
}

QRectF QskBoxMaterialVM::textureCoordinates() const
{
    if ( m_textures.isEmpty() )
        return QRectF();

    return m_textures.first()->normalizedTextureSubRect();
}

int QskBoxMaterialVM::compare( const QSGMaterial* other ) const
{
    if ( this == other )
        return 0;

    auto material = static_cast< decltype( this ) >( other );
    if ( textureCoordinates() == material->textureCoordinates() )
        return 0;

    return QSGMaterial::compare( other );
}

void QskBoxMaterialVM::release( uint key )
{
    Q_UNUSED( key )
#if 0
    auto it = qskTextureCache.find( key );
    if ( it != qskTextureCache.cend() )
    {
        auto& textureInfo = it->second;

        if ( --textureInfo.referenceCount <= 0 )
        {
            textureInfo.texture->deleteLater();
            qskTextureCache.erase( it );
        }
    }
#endif
}

void QskBoxMaterialVM::releaseTexture( QSGTexture* texture )
{
    auto it = qskTextureCache.findByTexture( texture );
    if ( it != qskTextureCache.cend() )
    {
        auto& textureInfo = it->second;

        if ( --textureInfo.referenceCount <= 0 )
        {
            texture->deleteLater();
            qskTextureCache.erase( it );
        }
    }
}

void QskBoxMaterialVM::releaseTextures()
{
    for ( auto& texture : m_textures )
        releaseTexture( texture );

    m_textures.clear();
}

QSGMaterialShader* QskBoxMaterialVM::createShader() const
{
    return new QskBoxMaterialShaderVM;
}

QSGMaterialType* QskBoxMaterialVM::type() const
{
    static QSGMaterialType materialType;
    return &materialType;
}
