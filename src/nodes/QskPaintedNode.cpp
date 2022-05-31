/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPaintedNode.h"
#include "QskSGNode.h"

#include <qsgimagenode.h>
#include <qquickwindow.h>
#include <qimage.h>
#include <qpainter.h>

#include <qopenglframebufferobject.h>
#include <qopenglpaintdevice.h>
#include <qopenglfunctions.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qsgplaintexture_p.h>
#include <private/qquickwindow_p.h>
QSK_QT_PRIVATE_END

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )

#include <private/qopenglframebufferobject_p.h>

static GLuint qskTakeTexture( QOpenGLFramebufferObject& fbo )
{
    /*
        See https://bugreports.qt.io/browse/QTBUG-103929

        As we create a FBO for each update of a node we can't live
        without having this ( ugly ) workaround.
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

#else

static GLuint qskTakeTexture( QOpenGLFramebufferObject& fbo )
{
    return fbo.takeTexture();
}

#endif

namespace
{
    const quint8 imageRole = 250; // reserved for internal use

    inline QSGImageNode* findImageNode( QSGNode* parentNode )
    {
        return static_cast< QSGImageNode* >(
            QskSGNode::findChildNode( parentNode, imageRole ) );
    }

    static inline bool qskHasOpenGLRenderer( QQuickWindow* window )
    {
        if ( window == nullptr )
            return false;

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
        if ( QQuickWindowPrivate::get( window )->rhi )
        {
            // does not yet work with the experimental RHI implementation in Qt5
            return false;
        }
#endif

        const auto renderer = window->rendererInterface();
        return renderer->graphicsApi() == QSGRendererInterface::OpenGL;
    }
}

QskPaintedNode::QskPaintedNode()
{
}

QskPaintedNode::~QskPaintedNode()
{
}

void QskPaintedNode::update( QQuickWindow* window,
    QskTextureRenderer::RenderMode renderMode, const QRectF& rect )
{
    auto imageNode = findImageNode( this );

    if ( rect.isEmpty() )
    {
        if ( imageNode )
        {
            removeChildNode( imageNode );
            delete imageNode;
        }
    }
    else
    {
        bool isDirty = false;

        const auto newHash = hash();
        if ( ( newHash == 0 ) || ( newHash != m_hash ) )
        {
            m_hash = newHash;
            isDirty = true;
        }

        if ( !isDirty )
            isDirty = ( imageNode == nullptr ) || ( imageNode->rect() != rect );

        if ( isDirty )
        {
            if ( renderMode != QskTextureRenderer::Raster )
            {
                if ( !qskHasOpenGLRenderer( window ) )
                    renderMode = QskTextureRenderer::Raster;
            }

            if ( renderMode == QskTextureRenderer::Raster )
                updateImageNode( window, rect );
            else
                updateImageNodeGL( window, rect );
        }
    }
}

void QskPaintedNode::updateImageNode( QQuickWindow* window, const QRectF& rect )
{
    const auto ratio = window->effectiveDevicePixelRatio();
    const auto size = rect.size() * ratio;

    QImage image( size.toSize(), QImage::Format_RGBA8888_Premultiplied );
    image.fill( Qt::transparent );

    {
        QPainter painter( &image );

        /*
            setting a devicePixelRatio for the image only works for
            value >= 1.0. So we have to scale manually.
         */
        painter.scale( ratio, ratio );
        paint( &painter, rect.size() );
    }

    auto imageNode = findImageNode( this );

    if ( imageNode == nullptr )
    {
        imageNode = window->createImageNode();

        imageNode->setOwnsTexture( true );
        QskSGNode::setNodeRole( imageNode, imageRole );

        appendChildNode( imageNode );
    }

    if ( auto texture = qobject_cast< QSGPlainTexture* >( imageNode->texture() ) )
        texture->setImage( image );
    else
        imageNode->setTexture( window->createTextureFromImage( image ) );

    imageNode->setRect( rect );
}

void QskPaintedNode::updateImageNodeGL( QQuickWindow* window, const QRectF& rect )
{
    const auto ratio = window->effectiveDevicePixelRatio();
    const QSize size( ratio * rect.width(), ratio * rect.height() );

    auto imageNode = findImageNode( this );

    if ( imageNode == nullptr )
    {
        imageNode = window->createImageNode();

        imageNode->setOwnsTexture( true );
        QskSGNode::setNodeRole( imageNode, imageRole );

        appendChildNode( imageNode );
    }

    auto texture = qobject_cast< QSGPlainTexture* >( imageNode->texture() );
    if ( texture == nullptr )
    {
        texture = new QSGPlainTexture;
        texture->setHasAlphaChannel( true );
        texture->setOwnsTexture( true );

        imageNode->setTexture( texture );
    }

    const auto textureId = createTexture( window, size );

    auto rhi = QQuickWindowPrivate::get( window )->rhi;

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    texture->setTextureFromNativeTexture(
        rhi, quint64( textureId ), 0, size, {}, {} );
#else
    if ( rhi )
    {
        // enabled with: "export QSG_RHI=1"
        texture->setTextureFromNativeObject( rhi,
            QQuickWindow::NativeObjectTexture, &textureId, 0, size, false );
    }
    else
    {
        texture->setTextureId( textureId );
        texture->setTextureSize( size );
    }
#endif

    imageNode->setRect( rect );
}

// this method will be moved to QskTextureRenderer. TODO ...
uint32_t QskPaintedNode::createTexture( QQuickWindow* window, const QSize& size )
{
    auto context = QOpenGLContext::currentContext();

    QOpenGLFramebufferObjectFormat format1;
    format1.setAttachment( QOpenGLFramebufferObject::CombinedDepthStencil );

    format1.setSamples( context->format().samples() );

    QOpenGLFramebufferObject multisampledFbo( size, format1 );

    QOpenGLPaintDevice pd( size );
    pd.setPaintFlipped( true );

    {
        int bufferId;

        auto gl = context->functions();

        gl->glGetIntegerv( GL_ARRAY_BUFFER_BINDING, &bufferId);
        gl->glBindBuffer( GL_ARRAY_BUFFER, 0 );

        const auto ratio = window->effectiveDevicePixelRatio();

        QPainter painter( &pd );
        painter.scale( ratio, ratio );

        painter.setCompositionMode( QPainter::CompositionMode_Source );
        painter.fillRect( 0, 0, size.width(), size.height(), Qt::transparent );
        painter.setCompositionMode( QPainter::CompositionMode_SourceOver );

        paint( &painter, size );

        gl->glBindBuffer( GL_ARRAY_BUFFER, bufferId );
    }

    QOpenGLFramebufferObjectFormat format2;
    format2.setAttachment( QOpenGLFramebufferObject::NoAttachment );

    QOpenGLFramebufferObject fbo( size, format2 );

    const QRect fboRect( 0, 0, size.width(), size.height() );

    QOpenGLFramebufferObject::blitFramebuffer(
        &fbo, fboRect, &multisampledFbo, fboRect );

    return qskTakeTexture( fbo );
}
