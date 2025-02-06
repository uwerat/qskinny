/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "Overlay.h"
#include "TextureFilterNode.h"
#include "TextureFilterMaterial.h"

#include <QskSkinlet.h>
#include <QskQuick.h>
#include <QskBoxShapeMetrics.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxBorderColors.h>
#include <QskGradient.h>
#include <QskSceneTexture.h>
#include <QskSGNode.h>
#include <QskRgbValue.h>

namespace
{
    class Material final : public TextureFilterMaterial
    {
      public:
        using TextureFilterMaterial::TextureFilterMaterial;

        QSGMaterialType* type() const override
        {
            static QSGMaterialType staticType;
            return &staticType;
        }
    };

    class FilterNode final : public TextureFilterNode
    {
      public:
        FilterNode( bool useRhi, QSGTexture* texture )
        {
            QString shaders[] = { ":/shaders/blur.vert", ":/shaders/blur.frag" };

            if ( useRhi )
            {
                shaders[0] += ".qsb";
                shaders[1] += ".qsb";
            }

            setFlag( QSGNode::OwnsMaterial, true );
            setTextureMaterial( new Material( shaders[0], shaders[1] ) );

            setOwnsTexture( true );
            setTexture( texture );
        }
    };

    class Skinlet final : public QskSkinlet
    {
        using Inherited = QskSkinlet;

      public:
        enum NodeRole { FillRole, BorderRole };

        Skinlet()
        {
            setNodeRoles( { FillRole, BorderRole } );
        }

        QRectF subControlRect( const QskSkinnable*,
            const QRectF& contentsRect, QskAspect::Subcontrol ) const override
        {
            return contentsRect;
        }

        QSGNode* updateSubNode( const QskSkinnable* skinnable,
            quint8 nodeRole, QSGNode* node ) const override
        {
            const auto overlay = static_cast< const Overlay* >( skinnable );

            switch ( nodeRole )
            {
                case FillRole:
                    return updateFillNode( overlay, node );

                case BorderRole:
                    return updateBoxNode( skinnable, node, Overlay::Panel );
            };

            return nullptr;
        }

      private:
        QSGNode* updateFillNode( const Overlay* overlay, QSGNode* node ) const
        {
            /*
                There should be a way to avoid the clip node by passing the
                vertex list directly to the texture node. TODO ...
             */
            using Q = Overlay;

            QSGNode* clipNode = nullptr;
            QSGNode* textureNode = nullptr;

            if ( node )
            {
                if ( node->firstChild() )
                {
                    clipNode = node;
                    textureNode = node->firstChild();
                }
                else
                {
                    textureNode = node;
                }
            }

            textureNode = updateTextureNode( overlay, textureNode );

            if ( overlay->boxShapeHint( Q::Panel ).isRectangle() )
            {
                delete clipNode;
                clipNode = nullptr;
            }
            else
            {
                clipNode = updateBoxClipNode( overlay, clipNode, Q::Panel );
                QskSGNode::setParentNode( textureNode, clipNode );
            }

            return clipNode ? clipNode : textureNode;
        }

        QSGNode* updateTextureNode( const Overlay* overlay, QSGNode* node ) const
        {
            const auto window = overlay->window();

            const auto rect = overlay->subControlRect( Overlay::Panel );
            if ( rect.isEmpty() )
                return nullptr;

            auto rootNode = qskScenegraphAnchorNode( window );
            if ( rootNode == nullptr )
                return nullptr;

            auto textureNode = static_cast< FilterNode* >( node );
            if ( textureNode == nullptr )
            {
                auto texture = new QskSceneTexture( window );
                QObject::connect( texture, &QskSceneTexture::updateRequested,
                    overlay, &QQuickItem::update );

                const bool useRhi = qskRenderingHardwareInterface( window );

                textureNode = new FilterNode( useRhi, texture );
            }

            auto texture = qobject_cast< QskSceneTexture* >( textureNode->texture() );
            Q_ASSERT( texture );

            if ( texture->isDirty() || rect != textureNode->rect() )
            {
                texture->setFiltering(
                    overlay->smooth() ? QSGTexture::Linear : QSGTexture::Nearest );

                auto finalNode = const_cast< QSGTransformNode* >( qskItemNode( overlay ) );

                texture->render( rootNode, finalNode,
                    rect.translated( overlay->position() ) );

                textureNode->markDirty( QSGNode::DirtyMaterial );
            }

            textureNode->setRect( rect );

            return textureNode;
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
