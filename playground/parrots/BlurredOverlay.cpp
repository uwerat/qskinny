/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "BlurredOverlay.h"
#include "BlurredTextureNode.h"

#include <private/qquickitem_p.h>
#include <private/qquickwindow_p.h>
#include <private/qsgadaptationlayer_p.h>
#include <private/qquickitemchangelistener_p.h>

#include <qpointer.h>

class BlurredOverlayPrivate : public QQuickItemPrivate, public QQuickItemChangeListener
{
  public:

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
        layer->setSamples( 0 );

        return layer;
    }

    void updateTexture( QSGLayer* layer )
    {
        Q_Q( BlurredOverlay );

        layer->setLive( live );
        layer->setItem( QQuickItemPrivate::get( grabbedItem )->itemNode() );

        auto r = grabRect;
        if ( r.isEmpty() )
            r = QRectF(0, 0, grabbedItem->width(), grabbedItem->height() );

        layer->setRect( r );

        QSize textureSize( qCeil( qAbs( r.width() ) ),
            qCeil( qAbs( r.height() ) ) );

        const auto pixelRatio = q->window()->effectiveDevicePixelRatio();
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
    QRectF grabRect;

    const bool live = true;
    bool covering = true;

    Q_DECLARE_PUBLIC(BlurredOverlay)
};

BlurredOverlay::BlurredOverlay( QQuickItem* parent )
    : QQuickItem( *new BlurredOverlayPrivate(), parent )
{
    setFlag( ItemHasContents );
}

BlurredOverlay::~BlurredOverlay()
{
    Q_D( BlurredOverlay );
    d->setAttached( false );
}

QQuickItem*BlurredOverlay::grabbedItem() const
{
    return d_func()->grabbedItem;
}

void BlurredOverlay::setGrabbedItem( QQuickItem* item )
{
    Q_D( BlurredOverlay );

    if ( item == d->grabbedItem )
        return;

    d->setAttached( false );
    d->grabbedItem = item;
    d->setAttached( true );

    update();
}

QRectF BlurredOverlay::grabRect() const
{
    return d_func()->grabRect;
}

void BlurredOverlay::setGrabRect( const QRectF& rect )
{
    Q_D( BlurredOverlay );

    QRectF r;
    if ( !rect.isEmpty() )
        r = rect;

    if ( r == d->grabRect )
        return;

    if ( r.isEmpty() != d->grabRect.isEmpty() )
        d->setCovering( r.isEmpty() );

    d->grabRect = r;

    if ( d->grabbedItem )
        update();
}

void BlurredOverlay::resetGrabRect()
{
    setGrabRect( QRectF() );
}

void BlurredOverlay::geometryChange(
    const QRectF& newGeometry, const QRectF& oldGeometry )
{
    update();
    Inherited::geometryChange( newGeometry, oldGeometry );
}

QSGNode* BlurredOverlay::updatePaintNode( QSGNode* oldNode, UpdatePaintNodeData* )
{
    if ( size().isEmpty() )
    {
        delete oldNode;
        return nullptr;
    }

    Q_D( BlurredOverlay );

    auto node = static_cast< BlurredTextureNode* >( oldNode );

    if ( node == nullptr )
    {
        node = new BlurredTextureNode();

        auto layer = d->createTexture();
        node->setTexture( layer );

        connect( layer, &QSGLayer::updateRequested,
            this, &QQuickItem::update );
    }

    auto layer = static_cast< QSGLayer* >( node->texture() );

    d->updateTexture( layer );

    layer->updateTexture();
    node->setRect( QRectF( 0, 0, width(), height() ) );

    return node;
}

#include "moc_BlurredOverlay.cpp"
