/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "Overlay.h"
#include "BlurringNode.h"
#include "SceneTexture.h"

#include <private/qquickitem_p.h>
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
        return grabbedItem ? get( grabbedItem )->rootNode() : nullptr;
    }

    QSGTransformNode* grabbedItemNode()
    {
        return grabbedItem ? get( grabbedItem )->itemNode() : nullptr;
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

    if ( d_func()->grabbedItem )
        update();
}

QSGNode* Overlay::updatePaintNode( QSGNode* oldNode, UpdatePaintNodeData* )
{
    Q_D( Overlay );

    if ( d->grabbedItemNode() == nullptr || size().isEmpty() )
    {
        delete oldNode;
        return nullptr;
    }

    auto node = static_cast< BlurringNode* >( oldNode );

    if ( node == nullptr )
    {
        node = new BlurringNode();

        auto texture = new SceneTexture( d->sceneGraphRenderContext() );
        texture->setDevicePixelRatio( window()->effectiveDevicePixelRatio() );

        node->setTexture( texture );
    }

    auto itemNode = static_cast< TransformNode* >( d->itemNode() );

    if ( !itemNode->isBlocked )
    {
        itemNode->isBlocked = true;

        auto texture = static_cast< SceneTexture* >( node->texture() );
        texture->setFiltering( smooth() ? QSGTexture::Linear : QSGTexture::Nearest );

        texture->render( d->grabbedNode(), itemNode,
            QRectF( x(), y(), width(), height() ) );

        itemNode->isBlocked = false;
        QMetaObject::invokeMethod( this, &QQuickItem::update );
    }

    node->setRect( QRectF( 0, 0, width(), height() ) );

    return node;
}

#include "moc_Overlay.cpp"
