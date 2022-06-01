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
#include <private/qopenglframebufferobject_p.h>
QSK_QT_PRIVATE_END

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
#include <qquickopenglutils.h>
#endif

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

static inline QSGImageNode::TextureCoordinatesTransformMode
    qskEffectiveTransformMode( const Qt::Orientations mirrored )
{
    QSGImageNode::TextureCoordinatesTransformMode mode;

    if ( mirrored & Qt::Vertical )
        mode |= QSGImageNode::MirrorVertically;

    if ( mirrored & Qt::Horizontal )
        mode |= QSGImageNode::MirrorHorizontally;

    return mode;
}

namespace
{
    const quint8 imageRole = 250; // reserved for internal use

    inline QSGImageNode* findImageNode( const QSGNode* parentNode )
    {
        auto node = QskSGNode::findChildNode(
            const_cast< QSGNode* >( parentNode ), imageRole );

        return static_cast< QSGImageNode* >( node );
    }

    static inline bool qskHasOpenGLRenderer( QQuickWindow* window )
    {
        if ( window == nullptr )
            return false;

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

void QskPaintedNode::setRenderHint( RenderHint renderHint )
{
    m_renderHint = renderHint;
}

QskPaintedNode::RenderHint QskPaintedNode::renderHint() const
{
    return m_renderHint;
}

void QskPaintedNode::setMirrored( Qt::Orientations orientations )
{
    if ( orientations != m_mirrored )
    {
        m_mirrored = orientations;

        if ( auto imageNode = findImageNode( this ) )
        {
            imageNode->setTextureCoordinatesTransform(
                qskEffectiveTransformMode( orientations ) );
        }
    }
}

Qt::Orientations QskPaintedNode::mirrored() const
{
    return m_mirrored;
}

QRectF QskPaintedNode::rect() const
{
    const auto imageNode = findImageNode( this );
    return imageNode ? imageNode->rect() : QRectF();
}

void QskPaintedNode::update( QQuickWindow* window,
    const QRectF& rect, const void* nodeData )
{
    auto imageNode = findImageNode( this );

    if ( rect.isEmpty() )
    {
        if ( imageNode )
        {
            removeChildNode( imageNode );
            delete imageNode;
        }

        return;
    }

    bool isDirty = false;

    const auto newHash = hash( nodeData );
    if ( ( newHash == 0 ) || ( newHash != m_hash ) )
    {
        m_hash = newHash;
        isDirty = true;
    }

    if ( !isDirty )
        isDirty = ( imageNode == nullptr ) || ( imageNode->rect() != rect );

    if ( isDirty )
    {
        if ( ( m_renderHint == OpenGL ) && qskHasOpenGLRenderer( window ) )
            updateImageNodeGL( window, rect, nodeData );
        else
            updateImageNode( window, rect, nodeData );
    }

    imageNode = findImageNode( this );
    if ( imageNode )
    {
        imageNode->setRect( rect );
        imageNode->setTextureCoordinatesTransform(
            qskEffectiveTransformMode( m_mirrored ) );
    }
}

void QskPaintedNode::updateImageNode(
    QQuickWindow* window, const QRectF& rect, const void* nodeData )
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
        paint( &painter, rect.size(), nodeData );
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
}

void QskPaintedNode::updateImageNodeGL(
    QQuickWindow* window, const QRectF& rect, const void* nodeData )
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

    /*
        QQuickFramebufferObject does the FBO rendering early
        ( QQuickWindow::beforeRendering ). However doing it below updatePaintNode
        seems to work as well. Let's see if we run into issues ...
     */
    const auto textureId = createTexture( window, size, nodeData );

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
}

uint32_t QskPaintedNode::createTexture(
    QQuickWindow* window, const QSize& size, const void* nodeData )
{
    /*
        Binding GL_ARRAY_BUFFER/GL_ELEMENT_ARRAY_BUFFER to 0 seems to be enough.

        However - as we do not know what is finally painted and what the
        OpenGL paint engine is doing with better reinitialize everything.

        Hope this has no side effects as the context will leave the function
        in a modified state. Otherwise we could try to change the buffers
        only and reset them, before leaving.
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
        const auto ratio = window->effectiveDevicePixelRatio();

        QPainter painter( &pd );
        painter.scale( ratio, ratio );

        painter.setCompositionMode( QPainter::CompositionMode_Source );
        painter.fillRect( 0, 0, size.width(), size.height(), Qt::transparent );
        painter.setCompositionMode( QPainter::CompositionMode_SourceOver );

        paint( &painter, size, nodeData );
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
