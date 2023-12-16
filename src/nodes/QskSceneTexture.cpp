/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSceneTexture.h"
#include "QskTreeNode.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qsgbatchrenderer_p.h>
QSK_QT_PRIVATE_END

namespace
{
    inline QSGRendererInterface::RenderMode contextRenderMode(
        QSGDefaultRenderContext* context )
    {
        return context->useDepthBufferFor2D()
            ? QSGRendererInterface::RenderMode2D
            : QSGRendererInterface::RenderMode2DNoDepthBuffer;
    }

    class Renderer final : public QSGBatchRenderer::Renderer
    {
        using Inherited = QSGBatchRenderer::Renderer;

      public:
        Renderer( QskSceneTexture*, QSGDefaultRenderContext* );
        ~Renderer() override;

        void setFinalNode( QSGTransformNode* );
        QRhiTexture* texture() const { return m_rhiTexture; }

        void setProjection( const QRectF& );
        void setTextureSize( const QSize& );

      protected:
        void nodeChanged( QSGNode*, QSGNode::DirtyState ) override;
        void render() override;

      private:
        void createTarget( const QSize& );
        void clearTarget();

      private:
        QRhiTexture* m_rhiTexture = nullptr;
        QSGTransformNode* m_finalNode = nullptr;

        QskSceneTexture* m_texture = nullptr;
    };

    Renderer::Renderer( QskSceneTexture* texture, QSGDefaultRenderContext* context )
        : Inherited( context, contextRenderMode( context ) )
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
        const auto rhi = context()->rhi();

        auto r = rect;

        if ( rhi->isYUpInFramebuffer() )
        {
            r.moveTop( r.bottom() );
            r.setHeight( -r.height() );
        }

        MatrixTransformFlags matrixFlags;

        if ( !rhi->isYUpInNDC() )
            matrixFlags |= QSGAbstractRenderer::MatrixTransformFlipY;

        setProjectionMatrixToRect( r, matrixFlags );
    }

    void Renderer::setTextureSize( const QSize& size )
    {
        if ( m_rt.rt && m_rt.rt->pixelSize() != size )
            clearTarget();

        if ( m_rt.rt == nullptr )
            createTarget( size );

        const QRect r( 0, 0, size.width(), size.height() );

        setDeviceRect( r );
        setViewportRect( r );
    }

    void Renderer::render()
    {
        qskTryBlockTrailingNodes( m_finalNode, rootNode(), true, false );
#if 0
        QSGNodeDumper::dump( rootNode() );
#endif
        Inherited::render();
        qskTryBlockTrailingNodes( m_finalNode, rootNode(), false, false );
    }

    void Renderer::nodeChanged( QSGNode* node, QSGNode::DirtyState state )
    {
        // do not forward nodes that are blocked while rendering

        const bool isTrailingNode = false; // TODO ...

        if ( !isTrailingNode )
        {
            Inherited::nodeChanged( node, state );
            Q_EMIT m_texture->updateRequested();
        }
    }

    void Renderer::createTarget( const QSize& size )
    {
        const auto rhi = context()->rhi();

        auto flags = QRhiTexture::RenderTarget | QRhiTexture::UsedAsTransferSource;

        m_rhiTexture = rhi->newTexture( QRhiTexture::RGBA8, size, 1, flags );
        m_rhiTexture->create();

        QRhiColorAttachment color0( m_rhiTexture );
        auto target = rhi->newTextureRenderTarget( { color0 } );

        target->setRenderPassDescriptor(
            target->newCompatibleRenderPassDescriptor() );

        target->create();

        m_rt.rt = target;
        m_rt.rpDesc = target->renderPassDescriptor();

        auto defaultContext = qobject_cast< QSGDefaultRenderContext* >( context() );
        m_rt.cb = defaultContext->currentFrameCommandBuffer();
    }

    void Renderer::clearTarget()
    {
        delete m_rt.rt;
        m_rt.rt = nullptr;

        delete m_rt.rpDesc;
        m_rt.rpDesc = nullptr;

        delete m_rhiTexture;
        m_rhiTexture = nullptr;
    }
}

class QskSceneTexturePrivate final : public QSGTexturePrivate
{
  public:
    QskSceneTexturePrivate( QskSceneTexture* texture )
        : QSGTexturePrivate( texture )
    {
    }

    QRectF rect;
    qreal devicePixelRatio = 1.0;

    Renderer* renderer = nullptr;
    QSGDefaultRenderContext* context = nullptr;
};

QskSceneTexture::QskSceneTexture( QSGRenderContext* context )
    : Inherited(*( new QskSceneTexturePrivate(this) ) )
{
    d_func()->context = static_cast< QSGDefaultRenderContext* >( context );
}

QskSceneTexture::~QskSceneTexture()
{
    delete d_func()->renderer;
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

qint64 QskSceneTexture::comparisonKey() const
{
    return qint64( rhiTexture() );
}

QRhiTexture* QskSceneTexture::rhiTexture() const
{
    Q_D( const QskSceneTexture );
    return d->renderer ? d->renderer->texture() : nullptr;
}

void QskSceneTexture::render( QSGRootNode* rootNode,
    QSGTransformNode* finalNode, const QRectF& rect )
{
    Q_D( QskSceneTexture );

    d->rect = rect;

    const auto pixelSize = textureSize();

    if ( d->renderer == nullptr )
    {
        d->renderer = new Renderer( this, d->context );
        d->renderer->setDevicePixelRatio( d->devicePixelRatio );
    }

    d->renderer->setRootNode( rootNode );
    d->renderer->setFinalNode( finalNode );
    d->renderer->setProjection( d->rect );
    d->renderer->setTextureSize( pixelSize );
    d->renderer->renderScene();
}

void QskSceneTexture::setDevicePixelRatio( qreal ratio )
{
    d_func()->devicePixelRatio = ratio;
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

void QskSceneTexture::commitTextureOperations( QRhi*, QRhiResourceUpdateBatch* )
{
    // what to do here ?
}

#include "moc_QskSceneTexture.cpp"
