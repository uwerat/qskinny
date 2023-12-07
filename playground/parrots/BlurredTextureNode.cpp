/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "BlurredTextureNode.h"

#include <qsgmaterialshader.h>
#include <qsgmaterial.h>
#include <qsgtexture.h>

#include <private/qsgnode_p.h>

namespace
{
    class Material : public QSGMaterial
    {
      public:
        Material()
        {
            setFlag(Blending | RequiresFullMatrix, true);
        }

        ~Material()
        {
            delete texture;
        }

        int compare( const QSGMaterial* other ) const
        {
            auto material = static_cast< const Material* >( other );

            const auto key1 = texture->comparisonKey();
            const auto key2 = material->texture->comparisonKey();

            return ( key1 == key2 ) ? 0 : ( ( key1 > key2 ) ? 1 : -1 );
        }

        QSGMaterialType* type() const override
        {
            static QSGMaterialType staticType;
            return &staticType;
        }

        QSGMaterialShader* createShader(
            QSGRendererInterface::RenderMode ) const override;

        QSGTexture* texture = nullptr;
    };
}

namespace
{
    class MaterialShader : public QSGMaterialShader
    {
      public:
        MaterialShader()
        {
            setFlag( UpdatesGraphicsPipelineState, true );

            setShaderFileName( VertexStage, ":/shaders/blur.vert.qsb" );
            setShaderFileName( FragmentStage, ":/shaders/blur.frag.qsb" );
        }

        bool updateUniformData( RenderState& state,
            QSGMaterial*, QSGMaterial* ) override
        {
            Q_ASSERT( state.uniformData()->size() >= 68 );

            auto data = state.uniformData()->data();
            bool changed = false;

            if ( state.isMatrixDirty() )
            {
                const auto matrix = state.combinedMatrix();
                memcpy( data + 0, matrix.constData(), 64 );

                changed = true;
            }

            if ( state.isOpacityDirty() )
            {
                const float opacity = state.opacity();
                memcpy( data + 64, &opacity, 4 );

                changed = true;
            }

            return changed;
        }

        void updateSampledImage( RenderState& state, int binding,
            QSGTexture** texture, QSGMaterial* newMaterial, QSGMaterial* ) override
        {
            Q_UNUSED( binding );
            Q_ASSERT( binding == 1 );

            auto mat = static_cast< Material* >( newMaterial );

            if ( mat->texture )
            {
                mat->texture->commitTextureOperations(
                    state.rhi(), state.resourceUpdateBatch() );

                *texture = mat->texture;
            }
        }
    };
}

QSGMaterialShader* Material::createShader(
    QSGRendererInterface::RenderMode ) const
{
    return new MaterialShader();
}

class BlurredTextureNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    BlurredTextureNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_TexturedPoint2D(), 4 )
    {
    }

    Material material;
    QSGGeometry geometry;

    QRectF rect;
};

BlurredTextureNode::BlurredTextureNode()
    : QSGGeometryNode( *new BlurredTextureNodePrivate )
{
    Q_D( BlurredTextureNode );

    setMaterial( &d->material );
    setGeometry( &d->geometry );
}

BlurredTextureNode::~BlurredTextureNode()
{
}

void BlurredTextureNode::setTexture( QSGTexture* texture )
{
    d_func()->material.texture = texture;
}

QSGTexture* BlurredTextureNode::texture()
{
    return d_func()->material.texture;
}

void BlurredTextureNode::setRect( const QRectF& rect )
{
    Q_D( BlurredTextureNode );

    if ( rect == d->rect )
        return;

    d->rect = rect;

    struct Point : QSGGeometry::Point2D
    {
        inline void operator=( const QPointF& pos )
        {
            x = static_cast< float >( pos.x() );
            y = static_cast< float >( pos.y() );
        }
    };

    const QRectF r0( 0, 0, 1, 1 );

    auto points = static_cast< Point* >( d->geometry.vertexData() );

    points[0] = rect.topLeft();
    points[1] = r0.topLeft();

    points[2] = rect.topRight();
    points[3] = r0.topRight();

    points[4] = rect.bottomLeft();
    points[5] = r0.bottomLeft();

    points[6] = rect.bottomRight();
    points[7] = r0.bottomRight();

    d->geometry.markVertexDataDirty();
    markDirty( QSGNode::DirtyGeometry );
}

QRectF BlurredTextureNode::rect() const
{
    return d_func()->rect;
}