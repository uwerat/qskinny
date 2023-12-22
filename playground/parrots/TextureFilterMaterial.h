/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <qsgmaterial.h>
#include <qstring.h>

class QSGTexture;

class TextureFilterMaterial : public QSGMaterial
{
  public:
    TextureFilterMaterial( const QString& vertexShaderSourceFile,
        const QString& fragmentShaderSourceFile );

    ~TextureFilterMaterial() override;

    int compare( const QSGMaterial* other ) const override;

    void setTexture( QSGTexture* texture ) { m_texture = texture; }
    QSGTexture* texture() const { return m_texture; }

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    QSGMaterialShader* createShader(
        QSGRendererInterface::RenderMode ) const override final;
#else
    QSGMaterialShader* createShader() const override final;
#endif

  private:
    QSGTexture* m_texture = nullptr;
    const QString m_shaderFiles[ 2 ];
};
