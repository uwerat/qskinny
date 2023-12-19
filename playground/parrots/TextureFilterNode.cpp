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

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
        QSGMaterialShader* createShader() const override;
#else
        QSGMaterialShader* createShader(
            QSGRendererInterface::RenderMode ) const override;
#endif

        QSGTexture* texture = nullptr;
    };
}

namespace
{
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    class MaterialShaderGL : public QSGMaterialShader
    {
      public:
        MaterialShaderGL()
        {
            setShaderSourceFile( QOpenGLShader::Vertex, ":/shaders/blur.vert" );
            setShaderSourceFile( QOpenGLShader::Fragment, ":/shaders/blur.frag" );
        }

        char const* const* attributeNames() const override
        {
            static char const* const names[] = { "in_vertex", "in_coord", nullptr };
            return names;
        }

       void initialize() override
        {
            QSGMaterialShader::initialize();

            auto p = program();

            m_matrixId = p->uniformLocation( "matrix" );
            m_opacityId = p->uniformLocation( "opacity" );
        }

       void updateState( const QSGMaterialShader::RenderState& state,
            QSGMaterial* newMaterial, QSGMaterial* ) override
        {
            auto p = program();

            if ( state.isMatrixDirty() )
                p->setUniformValue( m_matrixId, state.combinedMatrix() );

            if ( state.isOpacityDirty() )
                p->setUniformValue( m_opacityId, state.opacity() );

            auto material = static_cast< Material* >( newMaterial );
            if ( material->texture )
                material->texture->bind();
        }

      private:
        int m_matrixId = -1;
        int m_opacityId = -1;
    };
#else
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
#endif
}

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
QSGMaterialShader* Material::createShader() const
{
    Q_ASSERT( !( flags() & QSGMaterial::RhiShaderWanted ) );
    return new MaterialShaderGL();
}

#else

QSGMaterialShader* Material::createShader(
    QSGRendererInterface::RenderMode ) const
{
    return new MaterialShader();
}

#endif

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
    markDirty( QSGNode::DirtyMaterial );
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
