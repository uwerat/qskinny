/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "TextureFilterMaterial.h"

#include <qsgmaterial.h>
#include <qsgmaterialshader.h>
#include <qsgtexture.h>


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
            QSGMaterial* newMaterial, QSGMaterial* ) override
        {
            auto p = program();

            if ( state.isMatrixDirty() )
                p->setUniformValue( m_matrixId, state.combinedMatrix() );

            if ( state.isOpacityDirty() )
                p->setUniformValue( m_opacityId, state.opacity() );

            auto material = static_cast< TextureFilterMaterial* >( newMaterial );
            if ( material->texture() )
                material->texture()->bind();
        }

      private:
        int m_matrixId = -1;
        int m_opacityId = -1;
    };
}

QSGMaterialShader* TextureFilterMaterial::createShader() const
{
    Q_ASSERT( !( flags() & QSGMaterial::RhiShaderWanted ) );

    auto shader = new ShaderGL();

    shader->setSource( QOpenGLShader::Vertex, m_shaderSourceFiles[ 0 ] );
    shader->setSource( QOpenGLShader::Fragment, m_shaderSourceFiles[ 1 ] );

    return shader;
}

#else // Qt6

namespace
{
    class Shader : public QSGMaterialShader
    {
      public:
        Shader()
        {
            setFlag( UpdatesGraphicsPipelineState, true );
        }

        void setSource( QSGMaterialShader::Stage stage, const QString& filename )
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
                txt->commitTextureOperations( state.rhi(), state.resourceUpdateBatch() );
                *texture = txt;
            }
        }
    };
}

QSGMaterialShader* TextureFilterMaterial::createShader(
    QSGRendererInterface::RenderMode ) const
{
    auto shader = new Shader();

    shader->setSource( Shader::VertexStage, m_shaderSourceFiles[ 0 ] );
    shader->setSource( Shader::FragmentStage, m_shaderSourceFiles[ 1 ] );

    return shader;
}

#endif

TextureFilterMaterial::TextureFilterMaterial(
        const QString& vertexShaderSourceFile,
        const QString& fragmentShaderSourceFile )
    : m_shaderSourceFiles{ vertexShaderSourceFile, fragmentShaderSourceFile }
{
    setFlag( Blending | RequiresFullMatrix, true );
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
