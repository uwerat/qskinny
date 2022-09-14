#include "BlurredBoxMaterial.h"
#include "BlurredBoxMaterialShader.h"

BlurredBoxMaterial::BlurredBoxMaterial()
{
    setFlag( QSGMaterial::Blending, true );
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
    const auto* const material = dynamic_cast< const BlurredBoxMaterial* >( other );

    if ( material->m_rectCornerRadii != m_rectCornerRadii ||
         material->m_rectAspect != m_rectAspect ||
         material->m_opacity != m_opacity ||
         material->m_blurDirections != m_blurDirections ||
         material->m_blurQuality != m_blurQuality ||
         material->m_blurRadius != m_blurRadius  ||
         material->m_edgeSoftness != m_edgeSoftness ||
         material->m_texture.get() != m_texture.get())
    {
        return 1;
    }

    return QSGMaterial::compare( other );
}
