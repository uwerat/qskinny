#pragma once

#include <QSGMaterial>

class BlurredBoxMaterial final : public QSGMaterial
{
  public:
    BlurredBoxMaterial();

    QSGMaterialShader* createShader() const override;
    QSGMaterialType* type() const override;

    int compare( const QSGMaterial* other ) const override;

    QVector4D m_rectOfScreen{ 0, 0, 0, 0 };
    QVector4D m_rectOnScreen{ 0, 0, 0, 0 };
    QVector4D m_rectCornerRadii{ 0, 0, 0, 0 };
    float m_opacity = 1.0;
    float m_blurDirections = 32.0;
    float m_blurQuality = 8.0;
    float m_blurSize = 8.0;
};
