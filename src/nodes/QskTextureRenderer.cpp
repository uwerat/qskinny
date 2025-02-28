/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextureRenderer.h"
#include "QskQuick.h"
#include "QskInternalMacros.h"

#include <qopenglcontext.h>
#include <qopenglframebufferobject.h>
#include <qopenglpaintdevice.h>

#include <qimage.h>
#include <qpainter.h>

#include <qquickwindow.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgplaintexture_p.h>
#include <private/qopenglframebufferobject_p.h>
QSK_QT_PRIVATE_END

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    #include <qquickopenglutils.h>
#endif

static GLuint qskTakeTexture( QOpenGLFramebufferObject& fbo )
{
    /*
        See https://bugreports.qt.io/browse/QTBUG-103929

        QOpenGLFramebufferObject::takeTexture produces a memory leak
        that can't be accepted as we create an FBO each time we update
        a texture.

        The suggested workarond is to call QOpenGLSharedResourceGuard::invalidateResource()
        manually. Unfortunately this is a protected method and we need this nasty hack.
     */
    class MyFBO
    {
      public:
        virtual ~MyFBO() = default;
        QScopedPointer< QOpenGLFramebufferObjectPrivate > d_ptr;
    };

    static_assert( sizeof( MyFBO ) == sizeof( QOpenGLFramebufferObject ),
        "Bad cast: QOpenGLFramebufferObject does not match" );

    auto& attachment = reinterpret_cast< MyFBO* >( &fbo )->d_ptr->colorAttachments[0];
    auto guard = attachment.guard;

    const auto textureId = fbo.takeTexture();

    if ( guard )
    {
        class MyGuard : public QOpenGLSharedResourceGuard
        {
          public:
            void invalidateTexture() { invalidateResource(); }
        };

        reinterpret_cast< MyGuard* >( guard )->invalidateTexture();
    }

    attachment.guard = guard;

    return textureId;
}

void QskTextureRenderer::setTextureId( QQuickWindow* window,
    quint32 textureId, const QSize& size, QSGTexture* texture )
{
    auto plainTexture = qobject_cast< QSGPlainTexture* >( texture );
    if ( plainTexture == nullptr )
        return;

    auto rhi = qskRenderingHardwareInterface( window );

#if QT_VERSION >= QT_VERSION_CHECK( 6, 4, 0 )

    const uint nativeFormat = 0;
    plainTexture->setTextureFromNativeTexture(
        rhi, quint64( textureId ), 0, nativeFormat, size, {}, {} );

#elif QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    plainTexture->setTextureFromNativeTexture(
        rhi, quint64( textureId ), 0, size, {}, {} );
#else
    if ( rhi )
    {
        // enabled with: "export QSG_RHI=1"
        plainTexture->setTextureFromNativeObject( rhi,
            QQuickWindow::NativeObjectTexture, &textureId, 0, size, false );
    }
    else
    {
        plainTexture->setTextureId( textureId );
        plainTexture->setTextureSize( size );
    }
#endif
}

quint32 QskTextureRenderer::createTextureGL(
    QQuickWindow* window, const QSize& size, PaintHelper* helper )
{
    /*
        Binding GL_ARRAY_BUFFER/GL_ELEMENT_ARRAY_BUFFER to 0 seems to be enough.

        However - as we do not know what is finally painted and what the
        OpenGL paint engine is doing we better reinitialize everything.

        Hope this has no side effects as the context will leave the function
        in a modified state. Otherwise we could try to change the buffers
        only and reset them, before leaving.

        QQuickFramebufferObject does the FBO rendering early
        ( QQuickWindow::beforeRendering ). But so far doing it below updatePaintNode
        seems to work as well. Let's see if we run into issues ...
     */

    window->beginExternalCommands();

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    QQuickOpenGLUtils::resetOpenGLState();
#else
    window->resetOpenGLState();
#endif

    auto context = QOpenGLContext::currentContext();

    QOpenGLFramebufferObjectFormat format1;
    format1.setAttachment( QOpenGLFramebufferObject::CombinedDepthStencil );

    format1.setSamples( context->format().samples() );

    QOpenGLFramebufferObject multisampledFbo( size, format1 );

    QOpenGLPaintDevice pd( size );
    pd.setPaintFlipped( true );

    {
        QPainter painter( &pd );

        painter.setCompositionMode( QPainter::CompositionMode_Source );
        painter.fillRect( 0, 0, size.width(), size.height(), Qt::transparent );
        painter.setCompositionMode( QPainter::CompositionMode_SourceOver );

        const auto ratio = window->effectiveDevicePixelRatio();

        painter.scale( ratio, ratio );
        helper->paint( &painter, size / ratio );

#if 1
        if ( format1.samples() > 0 )
        {
            /*
                Multisampling in the window surface might get lost
                as a side effect of rendering to the FBO.
                weired, needs to be investigated more
             */
            painter.setRenderHint( QPainter::Antialiasing, true );
        }
#endif
    }

    QOpenGLFramebufferObjectFormat format2;
    format2.setAttachment( QOpenGLFramebufferObject::NoAttachment );

    QOpenGLFramebufferObject fbo( size, format2 );

    const QRect fboRect( 0, 0, size.width(), size.height() );

    QOpenGLFramebufferObject::blitFramebuffer(
        &fbo, fboRect, &multisampledFbo, fboRect );

    window->endExternalCommands();

    return qskTakeTexture( fbo );
}

QSGTexture* QskTextureRenderer::createTextureRaster( QQuickWindow* window,
    const QSize& size, PaintHelper* helper )
{
    const auto ratio = window ? window->effectiveDevicePixelRatio() : 1.0;

    QImage image( size * ratio, QImage::Format_RGBA8888_Premultiplied );
    image.fill( Qt::transparent );

    {
        QPainter painter( &image );

        /*
            setting a devicePixelRatio for the image only works for
            value >= 1.0. So we have to scale manually.
         */
        painter.scale( ratio, ratio );

        helper->paint( &painter, size );
    }

    return window->createTextureFromImage( image, QQuickWindow::TextureHasAlphaChannel );
}
