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

        QSGNode* updateSubNode(
            const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
        {
            const auto overlay = static_cast< const Overlay* >( skinnable );

            switch ( nodeRole )
            {
                case EffectRole:
                    return updateEffectNode( overlay, node );

                case BorderRole:
                    return updateBoxNode( overlay, node, overlay->contentsRect(),
                        QskBoxShapeMetrics(), 1, Qt::darkGray, QskGradient() );
                    break;
            };

            return nullptr;
        }

      private:
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

            auto texture = qobject_cast< QskSceneTexture* >( effectNode->texture() );
            Q_ASSERT( texture );

            texture->setFiltering( overlay->smooth() ? QSGTexture::Linear : QSGTexture::Nearest );

            auto finalNode = const_cast< QSGTransformNode* >( qskItemNode( overlay ) );
            texture->render( rootNode, finalNode, overlay->geometry() );

            effectNode->setRect( overlay->rect() );

            return effectNode;
        }
    };
}

Overlay::Overlay( QQuickItem* parent )
    : Inherited( parent )
{
    setSkinlet( new Skinlet() );
}

Overlay::~Overlay()
{
}

void Overlay::geometryChange(
    const QRectF& newGeometry, const QRectF& oldGeometry )
{
    Inherited::geometryChange( newGeometry, oldGeometry );
    update();
}

#include "moc_Overlay.cpp"
