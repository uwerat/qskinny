#pragma once

#include <QSGMaterialShader>

class BlurredBoxMaterialShader final : public QSGMaterialShader
{
  public:
    BlurredBoxMaterialShader();
    char const* const* attributeNames() const override;
    void initialize() override;
    void updateState( const QSGMaterialShader::RenderState& state, QSGMaterial* newMaterial,
        QSGMaterial* oldMaterial ) override;

  private:
    int m_matrixId = -1;
    int m_rectOpacityId = -1;
    int m_rectCornerRadiiId = -1;
    int m_rectAspect = -1;
    int m_blurDirectionsId = -1;
    int m_blurQualityId = -1;
    int m_blurRadiusId = -1;
    int m_textureId = -1;
    int m_edgeSoftnessId = -1;
};
