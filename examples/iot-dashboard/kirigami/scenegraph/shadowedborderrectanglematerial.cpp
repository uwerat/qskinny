/*
 *  SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "shadowedborderrectanglematerial.h"

#include <QOpenGLContext>

QSGMaterialType ShadowedBorderRectangleMaterial::staticType;

ShadowedBorderRectangleMaterial::ShadowedBorderRectangleMaterial()
{
    setFlag(QSGMaterial::Blending, true);
}

QSGMaterialShader* ShadowedBorderRectangleMaterial::createShader() const
{
    return new ShadowedBorderRectangleShader{shaderType};
}

QSGMaterialType* ShadowedBorderRectangleMaterial::type() const
{
    return &staticType;
}

int ShadowedBorderRectangleMaterial::compare(const QSGMaterial *other) const
{
    auto material = static_cast<const ShadowedBorderRectangleMaterial *>(other);

    auto result = ShadowedRectangleMaterial::compare(other);
    if (result == 0
        && material->borderColor == borderColor
        && qFuzzyCompare(material->borderWidth, borderWidth)
    ) {
        return 0;
    }

    return result;
}

ShadowedBorderRectangleShader::ShadowedBorderRectangleShader(ShadowedRectangleMaterial::ShaderType shaderType)
    : ShadowedRectangleShader(shaderType)
{
    setShader(shaderType, QStringLiteral("shadowedborderrectangle"));
}

void ShadowedBorderRectangleShader::initialize()
{
    ShadowedRectangleShader::initialize();
    m_borderWidthLocation = program()->uniformLocation("borderWidth");
    m_borderColorLocation = program()->uniformLocation("borderColor");
}

void ShadowedBorderRectangleShader::updateState(const QSGMaterialShader::RenderState& state, QSGMaterial* newMaterial, QSGMaterial* oldMaterial)
{
    ShadowedRectangleShader::updateState(state, newMaterial, oldMaterial);

    auto p = program();

    if (!oldMaterial || newMaterial->compare(oldMaterial) != 0 ) {
        auto material = static_cast<ShadowedBorderRectangleMaterial *>(newMaterial);
        p->setUniformValue(m_borderWidthLocation, material->borderWidth);
        p->setUniformValue(m_borderColorLocation, material->borderColor);
    }
}
