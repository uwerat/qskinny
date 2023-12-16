/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "Overlay.h"
#include "BlurringNode.h"

#include <QskSceneTexture.h>
#include <QskTreeNode.h>

#include <private/qquickitem_p.h>
#include <private/qquickwindow_p.h>
#include <private/qsgrenderer_p.h>
#include <private/qquickitemchangelistener_p.h>

#include <qpointer.h>

class OverlayPrivate final : public QQuickItemPrivate, public QQuickItemChangeListener
{
  public:
    void itemGeometryChanged( QQuickItem*,
        QQuickGeometryChange change, const QRectF& )
    {
        if ( change.sizeChange() )
            q_func()->update();
    }

    void setAttached( bool on )
    {
        if ( grabbedItem )
        {
            auto d = QQuickItemPrivate::get( grabbedItem );

            if ( on )
            {
                d->refFromEffectItem( false );
                d->addItemChangeListener( this, Geometry );
            }
            else
            {
                d->removeItemChangeListener( this, Geometry );
                d->derefFromEffectItem( false );
            }
        }
    }

    QSGRootNode* grabbedNode()
    {
        if ( grabbedItem )
            return grabbedItem ? get( grabbedItem )->rootNode() : nullptr;

        if ( auto window = q_func()->window() )
        {
            if ( auto renderer = QQuickWindowPrivate::get( window )->renderer )
                return renderer->rootNode();
        }

        return nullptr;
    }

    QSGTransformNode* createTransformNode() override
    {
        return new QskItemNode();
    }

    QPointer< QQuickItem > grabbedItem;

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
    update();
}

QSGNode* Overlay::updatePaintNode( QSGNode* oldNode, UpdatePaintNodeData* )
{
    Q_D( Overlay );

    auto grabbedNode = d->grabbedNode();
        
    if ( grabbedNode == nullptr || size().isEmpty() )
    {
        delete oldNode;
        return nullptr;
    }

    auto node = static_cast< BlurringNode* >( oldNode );

    if ( node == nullptr )
    {
        node = new BlurringNode();

        auto texture = new QskSceneTexture( d->sceneGraphRenderContext() );
        texture->setDevicePixelRatio( window()->effectiveDevicePixelRatio() );

        connect( texture, &QskSceneTexture::updateRequested,
            this, &QQuickItem::update );

        node->setTexture( texture );
    }

    auto texture = qobject_cast< QskSceneTexture* >( node->texture() );
    Q_ASSERT( texture );

    texture->setFiltering( smooth() ? QSGTexture::Linear : QSGTexture::Nearest );

    texture->render( grabbedNode, d->itemNode(),
        QRectF( x(), y(), width(), height() ) );

    node->setRect( QRectF( 0, 0, width(), height() ) );

#if 0
    // always updating ...
    QMetaObject::invokeMethod( this, &QQuickItem::update );
#endif

    return node;
}

#include "moc_Overlay.cpp"
