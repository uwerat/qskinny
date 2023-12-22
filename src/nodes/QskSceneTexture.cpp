/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSceneTexture.h"
#include "QskTreeNode.h"

#include <qmath.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qquickwindow_p.h>
#include <private/qsgtexture_p.h>

#define QT_BUILD_QUICK_LIB // suppress Qt5 warnings
#include <private/qsgbatchrenderer_p.h>
#undef QT_BUILD_QUICK_LIB

QSK_QT_PRIVATE_END

/*
    With Qt 5.15 Rhi can optionally be enbled by setting "export QSG_RHI=1".
    So we need to have a native QOpenGL implementation and one using
    the Rhi abstraction layer. For Qt6 we can rely on Rhi.
    Once Qt5 support has been dropped we can eliminate this #ifdef jungle
 */

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
#include <qopenglframebufferobject.h>
#endif

namespace
{
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    inline QSGRendererInterface::RenderMode contextRenderMode(
        QSGDefaultRenderContext* context )
    {
        return context->useDepthBufferFor2D()
            ? QSGRendererInterface::RenderMode2D
            : QSGRendererInterface::RenderMode2DNoDepthBuffer;
    }
#endif

    class Renderer final : public QSGBatchRenderer::Renderer
    {
        using Inherited = QSGBatchRenderer::Renderer;

      public:
        Renderer( QskSceneTexture*, QSGDefaultRenderContext* );
        ~Renderer() override;

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
        inline int textureId() const { return m_fbo ? m_fbo->texture() : 0; }
        inline void renderScene() { Inherited::renderScene( textureId() ); }
#endif

        inline QRhiTexture* rhiTexture() const { return m_rhiTexture; }

        inline bool isDirty() const { return m_dirty; }

        void setFinalNode( QSGTransformNode* );

        void setProjection( const QRectF& );
        void setTextureSize( const QSize& );

      protected:
        void nodeChanged( QSGNode*, QSGNode::DirtyState ) override;
        void render() override;

      private:
        void createTarget( const QSize& );
        void clearTarget();

        QSGTransformNode* m_finalNode = nullptr;
        QskSceneTexture* m_texture = nullptr;

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
        QOpenGLFramebufferObject* m_fbo;

        struct RenderTarget
        {
            QRhiRenderTarget* rt = nullptr;
            QRhiRenderPassDescriptor* rpDesc = nullptr;
            QRhiCommandBuffer* cb = nullptr;
        } m_rt;
#endif
        QRhiTexture* m_rhiTexture = nullptr;

        bool m_dirty = true;
    };

    Renderer::Renderer( QskSceneTexture* texture, QSGDefaultRenderContext* context )
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
        : Inherited( context )
#else
        : Inherited( context, contextRenderMode( context ) )
#endif
        , m_texture( texture )
    {
        setClearColor( Qt::transparent );
    }

    Renderer::~Renderer()
    {
        clearTarget();
    }

    void Renderer::setFinalNode( QSGTransformNode* node )
    {
        if ( node != m_finalNode )
        {
            m_finalNode = node;
        }
    }

    void Renderer::setProjection( const QRectF& rect )
    {
        bool flipFramebuffer = true;
        bool flipMatrix = false;

        if ( const auto rhi = context()->rhi() )
        {
            flipFramebuffer = rhi->isYUpInFramebuffer();
            flipMatrix = !rhi->isYUpInNDC();
        }

        auto r = rect;

        if ( flipFramebuffer )
        {
            r.moveTop( r.bottom() );
            r.setHeight( -r.height() );
        }

        MatrixTransformFlags matrixFlags;

        if ( flipMatrix )
            matrixFlags |= QSGAbstractRenderer::MatrixTransformFlipY;

        setProjectionMatrixToRect( r, matrixFlags );
    }

    void Renderer::setTextureSize( const QSize& size )
    {
        if ( const auto rhi = context()->rhi() )
        {
            if ( m_rt.rt && m_rt.rt->pixelSize() != size )
                clearTarget();

            if ( m_rt.rt == nullptr )
                createTarget( size );
        }
        else
        {
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
            if ( m_fbo && m_fbo->size() != size )
                clearTarget();

            if ( m_fbo == nullptr )
                createTarget( size );
#endif
        }

        const QRect r( 0, 0, size.width(), size.height() );

        setDeviceRect( r );
        setViewportRect( r );
    }

    void Renderer::render()
    {
        m_dirty = false;

        qskTryBlockTrailingNodes( m_finalNode, rootNode(), true, false );

#if 0
        static int counter = 0;
        qDebug() << ++counter;
        QSGNodeDumper::dump( rootNode() );
#endif
        Inherited::render();
        qskTryBlockTrailingNodes( m_finalNode, rootNode(), false, false );
    }

    void Renderer::nodeChanged( QSGNode* node, QSGNode::DirtyState state )
    {
        Inherited::nodeChanged( node, state );

        /*
            We want to limit updates to nodes, that are actually rendered. TODO ...

            In any case we need to block update requests, when the textureNode reports
            that it has been updated by us to the renderer of the window.
         */
        if ( ( state != QSGNode::DirtyMaterial )
            || ( node != m_texture->textureNode() ) )
        {
            m_dirty = true;
            Q_EMIT m_texture->updateRequested();
        }
    }

    void Renderer::createTarget( const QSize& size )
    {
        if ( const auto rhi = context()->rhi() )
        {
            auto flags = QRhiTexture::RenderTarget | QRhiTexture::UsedAsTransferSource;

            m_rhiTexture = rhi->newTexture( QRhiTexture::RGBA8, size, 1, flags );
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
            m_rhiTexture->build();
#else
            m_rhiTexture->create();
#endif

            QRhiColorAttachment color0( m_rhiTexture );
            auto target = rhi->newTextureRenderTarget( { color0 } );

            target->setRenderPassDescriptor(
                target->newCompatibleRenderPassDescriptor() );

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
            target->build();
#else
            target->create();
#endif

            m_rt.rt = target;
            m_rt.rpDesc = target->renderPassDescriptor();

            auto defaultContext = qobject_cast< QSGDefaultRenderContext* >( context() );
            m_rt.cb = defaultContext->currentFrameCommandBuffer();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
            setRenderTarget( m_rt.rt );
            setCommandBuffer( m_rt.cb );
            setRenderPassDescriptor( m_rt.rpDesc );
#endif
        }
        else
        {
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
            QOpenGLFramebufferObjectFormat format;
            format.setInternalTextureFormat( GL_RGBA8 );
            format.setSamples( 0 );
            format.setAttachment( QOpenGLFramebufferObject::CombinedDepthStencil );

            m_fbo = new QOpenGLFramebufferObject( size, format );
#endif
        }
    }

    void Renderer::clearTarget()
    {
        if ( const auto rhi = context()->rhi() )
        {
            delete m_rt.rt;
            m_rt.rt = nullptr;

            delete m_rt.rpDesc;
            m_rt.rpDesc = nullptr;

            delete m_rhiTexture;
            m_rhiTexture = nullptr;
        }
        else
        {
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
            delete m_fbo;
            m_fbo = nullptr;
#endif
        }
    }
}

class QskSceneTexturePrivate final : public QSGTexturePrivate
{
  public:
    QskSceneTexturePrivate( const QQuickWindow* window, QskSceneTexture* texture )
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
        : QSGTexturePrivate()
#else
        : QSGTexturePrivate( texture )
#endif
        , devicePixelRatio( window->effectiveDevicePixelRatio() )
    {
        Q_UNUSED( texture );

        // Qt5 needs the extra const_cast
        auto dw = QQuickWindowPrivate::get( const_cast< QQuickWindow* >( window ) );
        context = dynamic_cast< QSGDefaultRenderContext* >( dw->context );
    }

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    int comparisonKey() const override
        { return int( qintptr( rhiTexture() ) ); }

    QRhiTexture *rhiTexture() const override
        { return renderer ? renderer->rhiTexture() : nullptr; }
#endif

    QRectF rect;
    const qreal devicePixelRatio;

    Renderer* renderer = nullptr;
    QSGDefaultRenderContext* context = nullptr;

    const QSGGeometryNode* textureNode = nullptr;
};

QskSceneTexture::QskSceneTexture( const QQuickWindow* window )
    : Inherited( *new QskSceneTexturePrivate( window, this ) )
{
    Q_ASSERT( d_func()->context );
}

QskSceneTexture::~QskSceneTexture()
{
    delete d_func()->renderer;
}

void QskSceneTexture::setTextureNode( const QSGGeometryNode* node )
{
    d_func()->textureNode = node;
}

const QSGGeometryNode* QskSceneTexture::textureNode() const
{
    return d_func()->textureNode;
}

QSize QskSceneTexture::textureSize() const
{
    Q_D( const QskSceneTexture );

    QSize size( qCeil( d->rect.width() ), qCeil( d->rect.height() ) );
    size *= d->devicePixelRatio;

    const QSize minSize = d->context->sceneGraphContext()->minimumFBOSize();

    while ( size.width() < minSize.width() )
        size.rwidth() *= 2;

    while ( size.height() < minSize.height() )
        size.rheight() *= 2;

    return size;
}

void QskSceneTexture::render( const QSGRootNode* rootNode,
    const QSGTransformNode* finalNode, const QRectF& rect )
{
    Q_D( QskSceneTexture );

    d->rect = rect;

    if ( d->renderer == nullptr )
    {
        d->renderer = new Renderer( this, d->context );
        d->renderer->setDevicePixelRatio( d->devicePixelRatio );
    }

    d->renderer->setRootNode( const_cast< QSGRootNode* >( rootNode ) );
    d->renderer->setFinalNode( const_cast< QSGTransformNode* >( finalNode ) );

    d->renderer->setProjection( d->rect );
    d->renderer->setTextureSize( textureSize() );
    d->renderer->renderScene();
}

QRectF QskSceneTexture::normalizedTextureSubRect() const
{
    return QRectF( 0, 1, 1, -1 );
}

bool QskSceneTexture::hasAlphaChannel() const
{
    return false;
}

bool QskSceneTexture::hasMipmaps() const
{
    return false;
}

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )

void QskSceneTexture::bind()
{
    if ( d_func()->rhiTexture() == nullptr )
    {
        auto funcs = QOpenGLContext::currentContext()->functions();
        funcs->glBindTexture( GL_TEXTURE_2D, textureId() );

        updateBindOptions();
    }
}

int QskSceneTexture::textureId() const
{
    Q_D( const QskSceneTexture );
    return d->renderer ? d->renderer->textureId() : 0;
}

#else

qint64 QskSceneTexture::comparisonKey() const
{
    return qint64( rhiTexture() );
}

QRhiTexture* QskSceneTexture::rhiTexture() const
{
    Q_D( const QskSceneTexture );
    return d->renderer ? d->renderer->rhiTexture() : nullptr;
}

#endif

#include "moc_QskSceneTexture.cpp"
