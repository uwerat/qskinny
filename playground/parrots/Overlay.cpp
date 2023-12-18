/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "Overlay.h"
#include "BlurringNode.h"

#include <QskSkinlet.h>
#include <QskSceneTexture.h>
#include <QskQuick.h>
#include <QskBoxShapeMetrics.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxBorderColors.h>
#include <QskGradient.h>
#include <QskSGNode.h>
#include <QskRgbValue.h>

namespace
{
    class Skinlet : public QskSkinlet
    {
        using Inherited = QskSkinlet;

      public:
        enum NodeRole { EffectRole, BorderRole };

        Skinlet()
        {
            setNodeRoles( { EffectRole, BorderRole } );
        }

        QRectF subControlRect( const QskSkinnable*,
            const QRectF& contentsRect, QskAspect::Subcontrol ) const override
        {
            return contentsRect;
        }

        QSGNode* updateSubNode(
            const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
        {
            const auto overlay = static_cast< const Overlay* >( skinnable );

            switch ( nodeRole )
            {
                case EffectRole:
                    return updateClippedEffectNode( overlay, node );

                case BorderRole:
                    return updateBoxNode( skinnable, node, Overlay::Panel );
                    break;
            };

            return nullptr;
        }

      private:
        QSGNode* updateClippedEffectNode( const Overlay* overlay, QSGNode* node ) const
        {
            /*
                There should be a way to avoid the clip node by passing the
                vertex list directly to the effect node. TODO ...
             */
            using Q = Overlay;

            QSGNode* clipNode = nullptr;
            QSGNode* effectNode = nullptr;

            if ( node )
            {
                if ( node->firstChild() )
                {
                    clipNode = node;
                    effectNode = node->firstChild();
                }
                else
                {
                    effectNode = node;
                }
            }

            effectNode = updateEffectNode( overlay, effectNode );

            if ( overlay->boxShapeHint( Q::Panel ).isRectangle() )
            {
                delete clipNode;
                clipNode = nullptr;
            }
            else
            {
                clipNode = updateBoxClipNode( overlay, clipNode, Q::Panel );
            }

            QskSGNode::setParentNode( effectNode, clipNode );

            return clipNode ? clipNode : effectNode;
        }

        QSGNode* updateEffectNode( const Overlay* overlay, QSGNode* node ) const
        {
            const auto window = overlay->window();

            if ( overlay->size().isEmpty() )
                return nullptr;

            auto rootNode = qskScenegraphAnchorNode( window );
            if ( rootNode == nullptr )
                return nullptr;

            auto effectNode = static_cast< BlurringNode* >( node );

            if ( effectNode == nullptr )
            {
                auto texture = new QskSceneTexture( window );
                QObject::connect( texture, &QskSceneTexture::updateRequested,
                    overlay, &QQuickItem::update );

                effectNode = new BlurringNode();
                effectNode->setTexture( texture );
            }

            {
                auto texture = qobject_cast< QskSceneTexture* >( effectNode->texture() );
                Q_ASSERT( texture );

                texture->setFiltering(
                    overlay->smooth() ? QSGTexture::Linear : QSGTexture::Nearest );

                auto finalNode = const_cast< QSGTransformNode* >( qskItemNode( overlay ) );
                texture->render( rootNode, finalNode, overlay->geometry() );
            }

            {
                const auto rect = overlay->subControlRect( Overlay::Panel );
                effectNode->setRect( rect );
            }

            return effectNode;
        }
    };
}

QSK_SUBCONTROL( Overlay, Panel )

Overlay::Overlay( QQuickItem* parent )
    : Inherited( parent )
{
    setSkinlet( new Skinlet() );

    setBoxBorderMetricsHint( Panel, 1 );
    setBoxBorderColorsHint( Panel, QskRgb::toTransparent( QskRgb::DarkGrey, 100 ) );
    setBoxShapeHint( Panel, QskBoxShapeMetrics( 25, Qt::RelativeSize ) );
}

Overlay::~Overlay()
{
}

QRectF Overlay::layoutRectForSize( const QSizeF& size ) const
{
    return subControlContentsRect( size, Panel );
}

void Overlay::geometryChange(
    const QRectF& newGeometry, const QRectF& oldGeometry )
{
    Inherited::geometryChange( newGeometry, oldGeometry );
    update();
}

#include "moc_Overlay.cpp"
