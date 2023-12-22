/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "TextureFilterMaterial.h"

#include <qsgmaterial.h>
#include <qsgmaterialshader.h>
#include <qsgtexture.h>

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    #include <qsgmaterialrhishader.h>
    using RhiShader = QSGMaterialRhiShader;
#else
    using RhiShader = QSGMaterialShader;
#endif

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )

namespace
{
    class ShaderGL : public QSGMaterialShader
    {
      public:
        void setSource( QOpenGLShader::ShaderType type, const QString& fileName )
        {
            setShaderSourceFile( type, fileName );
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
            QSGMaterial* newMaterial, QSGMaterial* oldMaterial ) override
        {
            auto p = program();

            if ( state.isMatrixDirty() )
                p->setUniformValue( m_matrixId, state.combinedMatrix() );

            if ( state.isOpacityDirty() )
                p->setUniformValue( m_opacityId, state.opacity() );

            auto material = static_cast< TextureFilterMaterial* >( newMaterial );

            if ( auto texture = material->texture() )
            {
                auto textureId = -1;

                if ( auto oldMat = static_cast< TextureFilterMaterial* >( oldMaterial ) )
                {
                    if ( oldMat->texture() )
                        textureId = oldMat->texture()->textureId();
                }

                if ( texture->textureId() != textureId )
                    texture->bind();
                else
                    texture->updateBindOptions();
            }
        }

      private:
        int m_matrixId = -1;
        int m_opacityId = -1;
    };
}

#endif

namespace
{
    class ShaderRhi : public RhiShader
    {
      public:
        ShaderRhi()
        {
            setFlag( UpdatesGraphicsPipelineState, true );
        }

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
        void setSource( QOpenGLShader::ShaderType type, const QString& fileName )
        {
            setShaderSourceFile( type, fileName );
        }
#endif

        void setSource( Stage stage, const QString& filename )
        {
            setShaderFileName( stage, filename );
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

            auto mat = dynamic_cast< TextureFilterMaterial* >( newMaterial );
            if ( auto txt = mat->texture() )
            {
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
                txt->updateRhiTexture( state.rhi(), state.resourceUpdateBatch() );
#else
                txt->commitTextureOperations( state.rhi(), state.resourceUpdateBatch() );
#endif
                *texture = txt;
            }
        }
    };
}

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )

QSGMaterialShader* TextureFilterMaterial::createShader() const
{
    if ( flags() & QSGMaterial::RhiShaderWanted )
    {
        auto shader = new ShaderRhi();

        shader->setSource( ShaderRhi::VertexStage, m_shaderFiles[ 0 ] );
        shader->setSource( ShaderRhi::FragmentStage, m_shaderFiles[ 1 ] );

        return shader;
    }
    else
    {
        auto shader = new ShaderGL();

        shader->setSource( QOpenGLShader::Vertex, m_shaderFiles[ 0 ] );
        shader->setSource( QOpenGLShader::Fragment, m_shaderFiles[ 1 ] );

        return shader;
    }
}

#else

QSGMaterialShader* TextureFilterMaterial::createShader(
    QSGRendererInterface::RenderMode ) const
{
    auto shader = new ShaderRhi();

    shader->setSource( ShaderRhi::VertexStage, m_shaderFiles[ 0 ] );
    shader->setSource( ShaderRhi::FragmentStage, m_shaderFiles[ 1 ] );

    return shader;
}

#endif

TextureFilterMaterial::TextureFilterMaterial(
        const QString& vertexShaderFile, const QString& fragmentShaderFile )
    : m_shaderFiles{ vertexShaderFile, fragmentShaderFile }
{
    setFlag( Blending | RequiresFullMatrix, true );

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    setFlag( SupportsRhiShader, true );
#endif
}

TextureFilterMaterial::~TextureFilterMaterial()
{
}

int TextureFilterMaterial::compare( const QSGMaterial* other ) const
{
    auto material = static_cast< const TextureFilterMaterial* >( other );

    const auto key1 = texture()->comparisonKey();
    const auto key2 = material->texture()->comparisonKey();

    return ( key1 == key2 ) ? 0 : ( ( key1 > key2 ) ? 1 : -1 );
}
