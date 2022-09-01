#include "BlurredBoxMaterial.h"
#include "BlurredBoxMaterialShader.h"

BlurredBoxMaterial::BlurredBoxMaterial()
{
    setFlag( QSGMaterial::Blending, true );
    setFlag( QSGMaterial::SupportsRhiShader, true );
}

QSGMaterialShader* BlurredBoxMaterial::createShader() const
{
    return new BlurredBoxMaterialShader();
}

QSGMaterialType* BlurredBoxMaterial::type() const
{
    static QSGMaterialType staticType;
    return &staticType;
}

int BlurredBoxMaterial::compare( const QSGMaterial* other ) const
{
    auto material = dynamic_cast< const BlurredBoxMaterial* >( other );

    if ( material->m_rectOfScreen != m_rectOfScreen || material->m_rectOnScreen != m_rectOnScreen ||
         !qFuzzyCompare( material->m_rectCornerRadii, m_rectCornerRadii ) ||
         material->m_opacity != m_opacity || material->m_blurDirections != m_blurDirections ||
         material->m_blurQuality != m_blurQuality || material->m_blurSize != m_blurSize )
    {
        return 1;
    }

    return QSGMaterial::compare( other );
}
