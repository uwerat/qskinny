/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "SceneTexture.h"
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
                m_renderPassData.setDirty();
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
            prepareRenderPass( &m_renderPassData );
            m_renderPassData.postPrepare( rootNode(), m_finalNode );

            beginRenderPass( &m_renderPassData );
            recordRenderPass( &m_renderPassData );
            endRenderPass( &m_renderPassData );
        }

      private:
        void nodeChanged( QSGNode* node, QSGNode::DirtyState state ) override
        {
            if ( state & ( QSGNode::DirtyNodeAdded | QSGNode::DirtyNodeRemoved ) )
                m_renderPassData.setDirty();

            Inherited::nodeChanged( node, state );
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

        class RenderPassData : public RenderPassContext
        {
          public:
            void setDirty()
            {
                m_nodesDirty = true;
            }

            void postPrepare( const QSGNode* rootNode, const QSGNode* finalNode )
            {
                using namespace QSGBatchRenderer;;

                if ( m_nodesDirty )
                {
                    m_blockedNodes.clear();

                    for ( auto node = finalNode;
                        node && node != rootNode; node = node->parent() )
                    {
                        for ( auto sibling = node->nextSibling();
                            sibling != nullptr; sibling = sibling->nextSibling() )
                        {
                            markNodesAsBlocked( sibling );
                        }
                    }

                    m_nodesDirty = false;
                }

                for ( const auto& renderBatch : opaqueRenderBatches )
                {
                    auto batch = const_cast< Batch* >( renderBatch.batch );
                    removeBlockedNodes( batch );
                }

                for ( auto& renderBatch : alphaRenderBatches )
                {
                    auto batch = const_cast< Batch* >( renderBatch.batch );
                    removeBlockedNodes( batch );
                }
            }

          private:
            void markNodesAsBlocked( const QSGNode* node )
            {
                if ( node->type() == QSGNode::GeometryNodeType )
                {
                    auto n = static_cast< const QSGGeometryNode* >( node );
                    m_blockedNodes.insert( n );
                }

                for ( auto child = node->firstChild();
                    child != nullptr; child = child->nextSibling() )
                {
                    markNodesAsBlocked( child );
                }
            }

            void removeBlockedNodes( QSGBatchRenderer::Batch* batch )
            {
                QSGBatchRenderer::Element** elementRef = &batch->first;

                while( auto element = *elementRef )
                {
                    if ( m_blockedNodes.contains( element->node ) )
                        *elementRef = element->nextInBatch;
                    else
                        elementRef = &element->nextInBatch;
                }
            }

            bool m_nodesDirty = true;
            QSet< const QSGGeometryNode* > m_blockedNodes;
        };

        RenderPassData m_renderPassData;
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
    d->renderer->renderScene();
}

void SceneTexture::setDevicePixelRatio( qreal ratio )
{
    d_func()->devicePixelRatio = ratio;
}

QRectF SceneTexture::normalizedTextureSubRect() const
{
    return QRectF( 0, 1, 1, -1 );
}
