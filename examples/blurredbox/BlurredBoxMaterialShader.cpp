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
    m_blurDirectionsId = p->uniformLocation( "blurDirections" );
    m_blurQualityId = p->uniformLocation( "blurQuality" );
    m_rectOnScreen = p->uniformLocation( "rectOnScreen" );
    m_rectOfScreen = p->uniformLocation( "rectOfScreen" );
    m_blurSizeId = p->uniformLocation( "blurSize" );
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

    if ( updateMaterial )
    {
        auto material = dynamic_cast< const BlurredBoxMaterial* >( newMaterial );

        p->setUniformValue( m_rectCornerRadiiId, material->m_rectCornerRadii );
        p->setUniformValue( m_rectOfScreen, material->m_rectOfScreen );
        p->setUniformValue( m_rectOnScreen, material->m_rectOnScreen );
        p->setUniformValue( m_blurDirectionsId, material->m_blurDirections );
        p->setUniformValue( m_blurQualityId, material->m_blurQuality );
        p->setUniformValue( m_blurSizeId, material->m_blurSize );
    }
}
