/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGraphicTextureFactory.h"
#include "QskTextureRenderer.h"

#include <qquickwindow.h>
#include <qsgtexture.h>

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    #include <qsgtexture_platform.h>
#endif

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
    const uint textureId = QskTextureRenderer::createTextureFromGraphic(
        QskTextureRenderer::OpenGL, m_size, m_graphic, m_colorFilter,
        Qt::IgnoreAspectRatio );

    const auto flags = static_cast< QQuickWindow::CreateTextureOptions >(
        QQuickWindow::TextureHasAlphaChannel | QQuickWindow::TextureOwnsGLTexture );

    QSGTexture* texture;

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )

    texture = QNativeInterface::QSGOpenGLTexture::fromNative(
        textureId, window, m_size, flags );

#elif QT_VERSION >= QT_VERSION_CHECK( 5, 14, 0 )
    const int nativeLayout = 0; // VkImageLayout in case of Vulkan

    texture = window->createTextureFromNativeObject( 
        QQuickWindow::NativeObjectTexture, &textureId, nativeLayout,
        m_size, flags );
#else
    texture = window->createTextureFromId( textureId, m_size, flags );
#endif

    return texture;
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
