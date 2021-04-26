/*
 *  SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "shadowedrectanglematerial.h"

#include <QOpenGLContext>

QSGMaterialType ShadowedRectangleMaterial::staticType;

ShadowedRectangleMaterial::ShadowedRectangleMaterial()
{
    setFlag(QSGMaterial::Blending, true);
}

QSGMaterialShader* ShadowedRectangleMaterial::createShader() const
{
    return new ShadowedRectangleShader{shaderType};
}

QSGMaterialType* ShadowedRectangleMaterial::type() const
{
    return &staticType;
}

int ShadowedRectangleMaterial::compare(const QSGMaterial *other) const
{
    auto material = static_cast<const ShadowedRectangleMaterial *>(other);

    if (material->color == color
        && material->shadowColor == shadowColor
        && material->offset == offset
        && material->aspect == aspect
        && qFuzzyCompare(material->size, size)
        && qFuzzyCompare(material->radius, radius)) {
        return 0;
    }

    return QSGMaterial::compare(other);
}

ShadowedRectangleShader::ShadowedRectangleShader(ShadowedRectangleMaterial::ShaderType shaderType)
{
    setShader(shaderType, QStringLiteral("shadowedrectangle"));
}

const char *const * ShadowedRectangleShader::attributeNames() const
{
    static char const *const names[] = {"in_vertex", "in_uv", nullptr};
    return names;
}

void ShadowedRectangleShader::initialize()
{
    QSGMaterialShader::initialize();
    m_matrixLocation = program()->uniformLocation("matrix");
    m_aspectLocation = program()->uniformLocation("aspect");
    m_opacityLocation = program()->uniformLocation("opacity");
    m_sizeLocation = program()->uniformLocation("size");
    m_radiusLocation = program()->uniformLocation("radius");
    m_colorLocation = program()->uniformLocation("color");
    m_shadowColorLocation = program()->uniformLocation("shadowColor");
    m_offsetLocation = program()->uniformLocation("offset");
}

void ShadowedRectangleShader::updateState(const QSGMaterialShader::RenderState& state, QSGMaterial* newMaterial, QSGMaterial* oldMaterial)
{
    auto p = program();

    if (state.isMatrixDirty()) {
        p->setUniformValue(m_matrixLocation, state.combinedMatrix());
    }

    if (state.isOpacityDirty()) {
        p->setUniformValue(m_opacityLocation, state.opacity());
    }

    if (!oldMaterial || newMaterial->compare(oldMaterial) != 0 ) {
        auto material = static_cast<ShadowedRectangleMaterial *>(newMaterial);
        p->setUniformValue(m_aspectLocation, material->aspect);
        p->setUniformValue(m_sizeLocation, material->size);
        p->setUniformValue(m_radiusLocation, material->radius);
        p->setUniformValue(m_colorLocation, material->color);
        p->setUniformValue(m_shadowColorLocation, material->shadowColor);
        p->setUniformValue(m_offsetLocation, material->offset);
    }
}

void ShadowedRectangleShader::setShader(ShadowedRectangleMaterial::ShaderType shaderType, const QString& shader)
{
    auto header = QOpenGLContext::currentContext()->isOpenGLES() ? QStringLiteral("header_es.glsl") : QStringLiteral("header_desktop.glsl");

    auto shaderRoot = QStringLiteral(":/org/kde/kirigami/shaders/");

    setShaderSourceFiles(QOpenGLShader::Vertex, {
        shaderRoot + header,
        shaderRoot + QStringLiteral("shadowedrectangle.vert")
    });

    QString shaderFile = shader + QStringLiteral(".frag");
    auto sdfFile = QStringLiteral("sdf.glsl");
    if (shaderType == ShadowedRectangleMaterial::ShaderType::LowPower) {
        shaderFile = shader + QStringLiteral("_lowpower.frag");
        sdfFile = QStringLiteral("sdf_lowpower.glsl");
    }

    setShaderSourceFiles(QOpenGLShader::Fragment, {
        shaderRoot + header,
        shaderRoot + sdfFile,
        shaderRoot + shaderFile
    });
}
