/*
 *  SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "shadowedtexturenode.h"

#include <QSGTextureProvider>
#include "shadowedtexturematerial.h"
#include "shadowedbordertexturematerial.h"

template <typename T>
inline void preprocessTexture(QSGMaterial *material, QSGTextureProvider *provider)
{
    auto m = static_cast<T*>(material);
    // Since we handle texture coordinates differently in the shader, we
    // need to remove the texture from the atlas for now.
    if (provider->texture()->isAtlasTexture()) {
        // Blegh, I have no idea why "removedFromAtlas" doesn't just return
        // the texture when it's not an atlas.
        m->textureSource = provider->texture()->removedFromAtlas();
    } else {
        m->textureSource = provider->texture();
    }
    if (QSGDynamicTexture *dynamic_texture = qobject_cast<QSGDynamicTexture *>(m->textureSource)) {
        dynamic_texture->updateTexture();
    }
}

ShadowedTextureNode::ShadowedTextureNode()
    : ShadowedRectangleNode()
{
    setFlag(QSGNode::UsePreprocess);
}

void ShadowedTextureNode::setTextureSource(QSGTextureProvider *source)
{
    if (m_textureSource == source) {
        return;
    }

    if (m_textureSource) {
        m_textureSource->disconnect();
    }

    m_textureSource = source;
    QObject::connect(m_textureSource.data(), &QSGTextureProvider::textureChanged, nullptr, [this] { markDirty(QSGNode::DirtyMaterial); });
    markDirty(QSGNode::DirtyMaterial);
}

void ShadowedTextureNode::preprocess()
{
    if (m_textureSource && m_material && m_textureSource->texture()) {
        if (m_material->type() == borderlessMaterialType()) {
            preprocessTexture<ShadowedTextureMaterial>(m_material, m_textureSource);
        } else {
            preprocessTexture<ShadowedBorderTextureMaterial>(m_material, m_textureSource);
        }
    }
}

ShadowedRectangleMaterial *ShadowedTextureNode::createBorderlessMaterial()
{
    return new ShadowedTextureMaterial{};
}

ShadowedBorderRectangleMaterial *ShadowedTextureNode::createBorderMaterial()
{
    return new ShadowedBorderTextureMaterial{};
}

QSGMaterialType *ShadowedTextureNode::borderlessMaterialType()
{
    return &ShadowedTextureMaterial::staticType;
}

QSGMaterialType *ShadowedTextureNode::borderMaterialType()
{
    return &ShadowedBorderTextureMaterial::staticType;
}
