#pragma once

#include <QSGMaterial>
#include <QSGTexture>
#include <memory>

class BlurredBoxMaterial final : public QSGMaterial
{
  public:
    BlurredBoxMaterial();

    QSGMaterialShader* createShader() const override;
    QSGMaterialType* type() const override;

    int compare( const QSGMaterial* other ) const override;

    QVector4D m_rectCornerRadii{ 0, 0, 0, 0 };
    QVector2D m_rectAspect {1,1};
    float m_opacity = 1.0;
    float m_blurDirections = 32.0;
    float m_blurQuality = 8.0;
    QVector2D m_blurRadius = {1.0,1.0};
    float m_edgeSoftness = 1.0;
    std::unique_ptr<QSGTexture> m_texture;
};
