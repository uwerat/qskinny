/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGraphicTextureFactory.h"

#include <qopenglcontext.h>
#include <qopenglframebufferobject.h>
#include <qopenglfunctions.h>
#include <qopenglextrafunctions.h>
#include <qopenglpaintdevice.h>
#include <qopengltexture.h>

#include <qpainter.h>
#include <qquickwindow.h>

static uint qskTextureFBO(
    const QRect& rect, Qt::AspectRatioMode scalingMode,
    const QskGraphic& graphic, const QskColorFilter& filter )
{
    QOpenGLFramebufferObjectFormat format1;
    format1.setAttachment( QOpenGLFramebufferObject::CombinedDepthStencil );
    // ### TODO: get samples from window instead
    format1.setSamples( QOpenGLContext::currentContext()->format().samples() );

    const QRect sourceRect( QPoint(), rect.size() );

    QOpenGLFramebufferObject multisampledFbo( sourceRect.size(), format1 );

    QOpenGLPaintDevice pd( sourceRect.size() );

    QPainter painter( &pd );

    graphic.render( &painter, sourceRect, filter, scalingMode );

#if 1
    if ( format1.samples() > 0 )
    {
        // multisampling in the window surface might get lost
        // as a side effect of rendering to the FBO.
        // weired, needs to be investigated more
        painter.setRenderHint( QPainter::Antialiasing, true );
    }
#endif
    painter.end();

    QOpenGLFramebufferObjectFormat format2;
    format2.setAttachment( QOpenGLFramebufferObject::NoAttachment );

    QOpenGLFramebufferObject fbo( sourceRect.size(), format2 );

    // mirror vertically to be compliant with what QQuickTextureFactory expects.
    // for some reason we have to add 1 pixel to avoid that the mirrored
    // image gets cut off.

    const QRect targetRect( sourceRect.x(), sourceRect.bottom() + 1,
        sourceRect.width(), -sourceRect.height() );

    QOpenGLFramebufferObject::blitFramebuffer(
        &fbo, sourceRect, &multisampledFbo, targetRect );

    return fbo.takeTexture();
}

static uint qskTextureRaster(
    const QRect& rect, Qt::AspectRatioMode scalingMode,
    const QskGraphic& graphic, const QskColorFilter& filter )
{
    QImage image( rect.size(), QImage::Format_RGBA8888_Premultiplied );
    image.fill( Qt::transparent );

    {
        QPainter painter( &image );
        graphic.render( &painter, rect, filter, scalingMode );
    }

    const auto target = QOpenGLTexture::Target2D;

    auto context = QOpenGLContext::currentContext();
    if ( context == nullptr )
        return 0;

    auto& f = *context->functions();

    GLint oldTexture; // we can't rely on having OpenGL Direct State Access
    f.glGetIntegerv( QOpenGLTexture::BindingTarget2D, &oldTexture );

    GLuint textureId;
    f.glGenTextures( 1, &textureId );

    f.glBindTexture( target, textureId );
    
    f.glTexParameteri( target, GL_TEXTURE_MIN_FILTER, QOpenGLTexture::Nearest );
    f.glTexParameteri( target, GL_TEXTURE_MAG_FILTER, QOpenGLTexture::Nearest );

    f.glTexParameteri( target, GL_TEXTURE_WRAP_S, QOpenGLTexture::ClampToEdge );
    f.glTexParameteri( target, GL_TEXTURE_WRAP_T, QOpenGLTexture::ClampToEdge );

    if ( QOpenGLTexture::hasFeature( QOpenGLTexture::ImmutableStorage ) )
    {
        auto& ef = *context->extraFunctions();
        ef.glTexStorage2D( target, 1, 
            QOpenGLTexture::RGBA8_UNorm, image.width(), image.height() );

        f.glTexSubImage2D( target, 0, 0, 0, image.width(), image.height(),
            QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, image.constBits() );
    }
    else
    {
        f.glTexImage2D( target, 0, QOpenGLTexture::RGBA8_UNorm,
            image.width(), image.height(), 0,
            QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, image.constBits() );
    }

    f.glBindTexture( target, oldTexture );

    return textureId;
}

QskGraphicTextureFactory::QskGraphicTextureFactory()
{
}

QskGraphicTextureFactory::QskGraphicTextureFactory(
        const QskGraphic& graphic, const QSize& size )
    : m_graphic( graphic )
    , m_size( size )
{
}

QskGraphicTextureFactory::~QskGraphicTextureFactory()
{
}

QSGTexture* QskGraphicTextureFactory::createTexture( QQuickWindow* window ) const
{
    const QRect textureRect( 0, 0, m_size.width(), m_size.height() );

    const uint textureId = createTexture( QskGraphicTextureFactory::OpenGL,
        textureRect, Qt::KeepAspectRatio, m_graphic, m_colorFilter );

    const auto flags = static_cast< QQuickWindow::CreateTextureOptions >(
        QQuickWindow::TextureHasAlphaChannel | QQuickWindow::TextureOwnsGLTexture );

    return window->createTextureFromId( textureId, m_size, flags );
}

QSize QskGraphicTextureFactory::textureSize() const
{
    return m_size;
}

int QskGraphicTextureFactory::textureByteCount() const
{
    return m_size.width() * m_size.height() * 4;
}

QImage QskGraphicTextureFactory::image() const
{
    return m_graphic.toImage( m_size, Qt::KeepAspectRatio );
}

// ### TODO: get the FBO samples from the window
uint QskGraphicTextureFactory::createTexture(
    RenderMode mode, const QRect& rect, Qt::AspectRatioMode scalingMode,
    const QskGraphic& graphic, const QskColorFilter& filter )
{
    if ( mode == QskGraphicTextureFactory::Raster )
        return qskTextureRaster( rect, scalingMode, graphic, filter );
    else
        return qskTextureFBO( rect, scalingMode, graphic, filter );
}
