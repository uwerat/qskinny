/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "Overlay.h"
#include "BlurringNode.h"

#include <private/qquickitem_p.h>
#include <private/qquickwindow_p.h>
#include <private/qsgadaptationlayer_p.h>
#include <private/qquickitemchangelistener_p.h>

#include <qpointer.h>

namespace
{
    class TransformNode final : public QSGTransformNode
    {
      public:
        bool isSubtreeBlocked() const override
        {
            return isBlocked || QSGTransformNode::isSubtreeBlocked();
        }

        bool isBlocked = false;;
    };
}

class OverlayPrivate final : public QQuickItemPrivate, public QQuickItemChangeListener
{
  public:

    QSGTransformNode* createTransformNode() override
    {
        return new TransformNode();
    }

    void itemGeometryChanged( QQuickItem*,
        QQuickGeometryChange change, const QRectF& )
    {
        if ( change.sizeChange() )
            q_func()->update();
    }

    void setCovering( bool on )
    {
        if ( on == covering )
            return;

        if ( grabbedItem )
        {
            auto sd = QQuickItemPrivate::get( grabbedItem );

            sd->refFromEffectItem( on );
            sd->derefFromEffectItem( covering );
        }

        covering = on;
    }

    void setAttached( bool on )
    {
        if ( grabbedItem )
        {
            auto d = QQuickItemPrivate::get( grabbedItem );

            if ( on )
            {
                d->refFromEffectItem( covering );
                d->addItemChangeListener( this, Geometry );
            }
            else
            {
                d->removeItemChangeListener( this, Geometry );
                d->derefFromEffectItem( covering );
            }
        }
    }

    QSGLayer* createTexture()
    {
        auto renderContext = sceneGraphRenderContext();

        auto layer = renderContext->sceneGraphContext()->createLayer( renderContext );

        layer->setMipmapFiltering( QSGTexture::None );
        layer->setHorizontalWrapMode( QSGTexture::ClampToEdge );
        layer->setVerticalWrapMode( QSGTexture::ClampToEdge );
        layer->setFormat( QSGLayer::RGBA8 );
        layer->setHasMipmaps( false );
        layer->setMirrorHorizontal( false );
        layer->setMirrorVertical( true );

        if ( q_func()->window()->format().samples() > 2 )
        {
            /*
                We want to disable multisampling as it doesn't make any sense
                in combination with blurring afterwards. Unfortunately
                QSGLayer uses the samples from the window when setting samples
                below 2 here.
             */
            layer->setSamples( 2 );
        }

        return layer;
    }

    void updateTexture( QSGLayer* layer )
    {
        Q_Q( Overlay );

        const auto pixelRatio = q->window()->effectiveDevicePixelRatio();

        layer->setLive( true );
        layer->setItem( QQuickItemPrivate::get( grabbedItem )->itemNode() );

        const auto rect = QRectF( q->position(), q->size() );
        layer->setRect( rect );

        QSize textureSize( qCeil( rect.width() ), qCeil( rect.height() ) );
        textureSize *= pixelRatio;

        const QSize minTextureSize = sceneGraphContext()->minimumFBOSize();

        while ( textureSize.width() < minTextureSize.width() )
            textureSize.rwidth() *= 2;

        while ( textureSize.height() < minTextureSize.height() )
            textureSize.rheight() *= 2;

        layer->setDevicePixelRatio( pixelRatio );
        layer->setSize( textureSize );
        layer->setRecursive( false );
        layer->setFiltering( q->smooth() ? QSGTexture::Linear : QSGTexture::Nearest );
    }

    QPointer< QQuickItem > grabbedItem;
    bool covering = false;

    Q_DECLARE_PUBLIC(Overlay)
};

Overlay::Overlay( QQuickItem* parent )
    : QQuickItem( *new OverlayPrivate(), parent )
{
    setFlag( ItemHasContents );
}

Overlay::~Overlay()
{
    d_func()->setAttached( false );
}

QQuickItem*Overlay::grabbedItem() const
{
    return d_func()->grabbedItem;
}

void Overlay::setGrabbedItem( QQuickItem* item )
{
    Q_D( Overlay );

    if ( item == d->grabbedItem )
        return;

    d->setAttached( false );
    d->grabbedItem = item;
    d->setAttached( true );

    update();
}

void Overlay::geometryChange(
    const QRectF& newGeometry, const QRectF& oldGeometry )
{
    Inherited::geometryChange( newGeometry, oldGeometry );

    /*
        When newGeometry covers the grabbedItem completely we could
        set covering to true. TODO ...
     */

    if ( d_func()->grabbedItem )
        update();
}

QSGNode* Overlay::updatePaintNode( QSGNode* oldNode, UpdatePaintNodeData* )
{
    if ( size().isEmpty() )
    {
        delete oldNode;
        return nullptr;
    }

    Q_D( Overlay );

    auto node = static_cast< BlurringNode* >( oldNode );

    if ( node == nullptr )
    {
        node = new BlurringNode();

        auto layer = d->createTexture();
        node->setTexture( layer );

        connect( layer, &QSGLayer::updateRequested,
            this, &QQuickItem::update );
    }

    auto itemNode = static_cast< TransformNode* >( d->itemNode() );

    if ( !itemNode->isBlocked )
    {
        itemNode->isBlocked = true;

        auto layer = static_cast< QSGLayer* >( node->texture() );

        d->updateTexture( layer );

        layer->updateTexture();
        itemNode->isBlocked = false;
    }

    node->setRect( QRectF( 0, 0, width(), height() ) );

    return node;
}

#include "moc_Overlay.cpp"
