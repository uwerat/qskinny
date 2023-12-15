/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "SceneTexture.h"

#include <QskTreeNode.h>
#include <private/qsgbatchrenderer_p.h>

namespace
{
    class Renderer : public QSGBatchRenderer::Renderer
    {
        using Inherited = QSGBatchRenderer::Renderer;

      public:
        Renderer( QSGDefaultRenderContext* context )
            : Inherited( context, renderMode( context ) )
        {
            setClearColor( Qt::transparent );
        }

        ~Renderer() override
        {
            clearTarget();
        }

        void setFinalNode( QSGTransformNode* node )
        {
            if ( node != m_finalNode )
            {
                m_finalNode = node;
            }
        }

        QRhiTexture* texture() const { return m_rhiTexture; }

        void setProjection( const QRectF& rect )
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

        void setTextureSize( const QSize& size )
        {
            if ( m_rt.rt && m_rt.rt->pixelSize() != size )
                clearTarget();

            if ( m_rt.rt == nullptr )
                createTarget( size );

            const QRect r( 0, 0, size.width(), size.height() );

            setDeviceRect( r );
            setViewportRect( r );
        }

        void render() override
        {
            setNodesBlocked( m_finalNode, true );
#if 0
            qDebug() << "===================";
            QSGNodeDumper::dump( rootNode() );
#endif
            Inherited::render();
            setNodesBlocked( m_finalNode, false );
        }

      private:
        void setNodesBlocked( QSGNode* node, bool on )
        {
            setSubtreeBlocked( node, on );

            for ( auto sibling = node->nextSibling();
                sibling != nullptr; sibling = sibling->nextSibling() )
            {
                setSubtreeBlocked( sibling, on );
            }

            if ( node != rootNode() )
            {
                if ( auto upperNode = node->parent() )
                {
                    upperNode = upperNode->nextSibling();
                    if ( upperNode )
                        setNodesBlocked( upperNode, on );
                }
            }
        }

        void setSubtreeBlocked( QSGNode* node, bool on )
        {
            if ( qskTrySubtreeBlocked( node, on, false ) )
                return;

            for ( auto child = node->firstChild();
                child != nullptr; child = child->nextSibling() )
            {
                setSubtreeBlocked( child, on );
            }
        }

        void createTarget( const QSize& size )
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

        void clearTarget()
        {
            delete m_rt.rt;
            m_rt.rt = nullptr;

            delete m_rt.rpDesc;
            m_rt.rpDesc = nullptr;

            delete m_rhiTexture;
            m_rhiTexture = nullptr;
        }

      private:
        inline QSGRendererInterface::RenderMode renderMode(
            QSGDefaultRenderContext* context ) const
        {
            return context->useDepthBufferFor2D()
                ? QSGRendererInterface::RenderMode2D
                : QSGRendererInterface::RenderMode2DNoDepthBuffer;
        }

        QRhiTexture* m_rhiTexture = nullptr;
        QSGTransformNode* m_finalNode = nullptr;
    };
};

class SceneTexturePrivate final : public QSGTexturePrivate
{
  public:
    SceneTexturePrivate( SceneTexture* texture )
        : QSGTexturePrivate( texture )
    {
    }

    QRectF rect;
    qreal devicePixelRatio = 1.0;

    Renderer* renderer = nullptr;
    QSGDefaultRenderContext* context = nullptr;

    bool isRendering = false;
};

SceneTexture::SceneTexture( QSGRenderContext* context )
    : Inherited(*( new SceneTexturePrivate(this) ) )
{
    d_func()->context = static_cast< QSGDefaultRenderContext* >( context );
}

SceneTexture::~SceneTexture()
{
    delete d_func()->renderer;
}

QSize SceneTexture::textureSize() const
{
    Q_D( const SceneTexture );

    QSize size( qCeil( d->rect.width() ), qCeil( d->rect.height() ) );
    size *= d->devicePixelRatio;

    const QSize minSize = d->context->sceneGraphContext()->minimumFBOSize();

    while ( size.width() < minSize.width() )
        size.rwidth() *= 2;

    while ( size.height() < minSize.height() )
        size.rheight() *= 2;

    return size;
}

qint64 SceneTexture::comparisonKey() const
{
    return qint64( rhiTexture() );
}

QRhiTexture* SceneTexture::rhiTexture() const
{
    Q_D( const SceneTexture );
    return d->renderer ? d->renderer->texture() : nullptr;
}

void SceneTexture::render( QSGRootNode* rootNode,
    QSGTransformNode* finalNode, const QRectF& rect )
{
    Q_D( SceneTexture );

    d->rect = rect;

    const auto pixelSize = textureSize();

    if ( d->renderer == nullptr )
    {
        d->renderer = new Renderer( d->context );
        d->renderer->setDevicePixelRatio( d->devicePixelRatio );
    }

    d->renderer->setRootNode( rootNode );
    d->renderer->setFinalNode( finalNode );
    d->renderer->setProjection( d->rect );
    d->renderer->setTextureSize( pixelSize );

    d->isRendering = true;
    d->renderer->renderScene();
    d->isRendering = false;
}

bool SceneTexture::isRendering() const
{
    return d_func()->isRendering;
}

void SceneTexture::setDevicePixelRatio( qreal ratio )
{
    d_func()->devicePixelRatio = ratio;
}

QRectF SceneTexture::normalizedTextureSubRect() const
{
    return QRectF( 0, 1, 1, -1 );
}
