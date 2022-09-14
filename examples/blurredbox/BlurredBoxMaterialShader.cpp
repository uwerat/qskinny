#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QImage>
#include <QSGTexture>

#include "BlurredBoxMaterialShader.h"
#include "BlurredBoxMaterial.h"

BlurredBoxMaterialShader::BlurredBoxMaterialShader()
{
    setShaderSourceFile( QOpenGLShader::Vertex, ":/shaders/blurredbox.vert" );
    setShaderSourceFile( QOpenGLShader::Fragment, ":/shaders/blurredbox.frag" );
}

const char* const* BlurredBoxMaterialShader::attributeNames() const
{
    static char const* const names[] = { "in_vertex", "in_coord", nullptr };
    return names;
}

void BlurredBoxMaterialShader::initialize()
{
    QSGMaterialShader::initialize();

    auto p = program();

    m_matrixId = p->uniformLocation( "matrix" );
    m_rectOpacityId = p->uniformLocation( "rectOpacity" );
    m_rectCornerRadiiId = p->uniformLocation( "rectCornerRadii" );
    m_rectAspect = p->uniformLocation( "rectAspect" );
    m_blurDirectionsId = p->uniformLocation( "blurDirections" );
    m_blurQualityId = p->uniformLocation( "blurQuality" );    
    m_blurRadiusId = p->uniformLocation( "blurRadius" );
    m_textureId = p->uniformLocation( "txr" );
    m_edgeSoftnessId = p->uniformLocation( "edgeSoftness" );
}

void BlurredBoxMaterialShader::updateState(
    const RenderState& state, QSGMaterial* newMaterial, QSGMaterial* oldMaterial )
{
    auto p = program();

    if ( state.isMatrixDirty() )
    {
        p->setUniformValue( m_matrixId, state.combinedMatrix() );
    }

    if ( state.isOpacityDirty() )
    {
        p->setUniformValue( m_rectOpacityId, state.opacity() );
    }

    bool updateMaterial = ( oldMaterial == nullptr ) || newMaterial->compare( oldMaterial ) != 0;
    updateMaterial |= state.isCachedMaterialDataDirty();

    if ( !updateMaterial )
    {
        return;
    }

    if(const auto* const material = dynamic_cast< const BlurredBoxMaterial* >( newMaterial ))
    {
        p->setUniformValue( m_rectCornerRadiiId, material->m_rectCornerRadii );
        p->setUniformValue( m_rectAspect, material->m_rectAspect );
        p->setUniformValue( m_blurDirectionsId, material->m_blurDirections );
        p->setUniformValue( m_blurQualityId, material->m_blurQuality );
        p->setUniformValue( m_blurRadiusId, material->m_blurRadius );
        p->setUniformValue( m_edgeSoftnessId, material->m_edgeSoftness );
        p->setUniformValue( m_textureId, 0 /* GL_TEXTURE0 */);

        auto* const f = QOpenGLContext::currentContext()->functions();
        f->glActiveTexture(GL_TEXTURE0);
        material->m_texture->bind();
    }
}
