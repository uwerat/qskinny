/*
 *  SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "shadowedtexturematerial.h"

#include <QOpenGLContext>

QSGMaterialType ShadowedTextureMaterial::staticType;

ShadowedTextureMaterial::ShadowedTextureMaterial()
    : ShadowedRectangleMaterial()
{
    setFlag(QSGMaterial::Blending, true);
}

QSGMaterialShader* ShadowedTextureMaterial::createShader() const
{
    return new ShadowedTextureShader{shaderType};
}

QSGMaterialType* ShadowedTextureMaterial::type() const
{
    return &staticType;
}

int ShadowedTextureMaterial::compare(const QSGMaterial *other) const
{
    auto material = static_cast<const ShadowedTextureMaterial *>(other);

    auto result = ShadowedRectangleMaterial::compare(other);
    if (result == 0) {
        if (material->textureSource == textureSource) {
            return 0;
        } else {
            return (material->textureSource < textureSource) ? 1 : -1;
        }
    }

    return result;
}

ShadowedTextureShader::ShadowedTextureShader(ShadowedRectangleMaterial::ShaderType shaderType)
    : ShadowedRectangleShader(shaderType)
{
    setShader(shaderType, QStringLiteral("shadowedtexture"));
}

void ShadowedTextureShader::initialize()
{
    ShadowedRectangleShader::initialize();
    program()->setUniformValue("textureSource", 0);
}

void ShadowedTextureShader::updateState(const QSGMaterialShader::RenderState& state, QSGMaterial* newMaterial, QSGMaterial* oldMaterial)
{
    ShadowedRectangleShader::updateState(state, newMaterial, oldMaterial);

    auto texture = static_cast<ShadowedTextureMaterial*>(newMaterial)->textureSource;
    if (texture) {
        texture->bind();
    }
}
