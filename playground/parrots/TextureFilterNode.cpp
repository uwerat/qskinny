/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "TextureFilterNode.h"

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

            /*
                Using our own shaders - we do not want to add a dependency
                to the quickeffects module.
             */
            setShaderFileName( VertexStage, ":/shaders/blur.vert.qsb" );
#if 1
            setShaderFileName( FragmentStage, ":/shaders/blur.frag.qsb" );
#else
            setShaderFileName( FragmentStage, ":/shaders/rgbswap.frag.qsb" );
#endif
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

class TextureFilterNodePrivate final : public QSGGeometryNodePrivate
{
  public:
    TextureFilterNodePrivate()
        : geometry( QSGGeometry::defaultAttributes_TexturedPoint2D(), 4 )
    {
    }

    Material material;
    QSGGeometry geometry;

    QRectF rect;
};

TextureFilterNode::TextureFilterNode()
    : QSGGeometryNode( *new TextureFilterNodePrivate )
{
    Q_D( TextureFilterNode );

    setMaterial( &d->material );
    setGeometry( &d->geometry );
}

TextureFilterNode::~TextureFilterNode()
{
}

void TextureFilterNode::setTexture( QSGTexture* texture )
{
    d_func()->material.texture = texture;
}

QSGTexture* TextureFilterNode::texture()
{
    return d_func()->material.texture;
}

void TextureFilterNode::setRect( const QRectF& rect )
{
    Q_D( TextureFilterNode );

    if ( rect != d->rect )
    {
        d->rect = rect;

        QSGGeometry::updateTexturedRectGeometry(
            &d->geometry, rect, QRectF( 0, 0, 1, 1 ) );

        d->geometry.markVertexDataDirty();
        markDirty( QSGNode::DirtyGeometry );
    }
}
