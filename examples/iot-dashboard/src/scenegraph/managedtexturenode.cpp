/*
 *  SPDX-FileCopyrightText: 2011 Marco Martin <mart@kde.org>
 *  SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *  SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "managedtexturenode.h"

ManagedTextureNode::ManagedTextureNode()
{}

void ManagedTextureNode::setTexture(QSharedPointer<QSGTexture> texture)
{
    m_texture = texture;
    QSGSimpleTextureNode::setTexture(texture.data());
}

ImageTexturesCache::ImageTexturesCache()
    : d(new ImageTexturesCachePrivate)
{
}

ImageTexturesCache::~ImageTexturesCache()
{
}

QSharedPointer<QSGTexture> ImageTexturesCache::loadTexture(QQuickWindow *window, const QImage &image, QQuickWindow::CreateTextureOptions options)
{
    qint64 id = image.cacheKey();
    QSharedPointer<QSGTexture> texture = d->cache.value(id).value(window).toStrongRef();

    if (!texture) {
        auto cleanAndDelete = [this, window, id](QSGTexture* texture) {
            QHash<QWindow*, QWeakPointer<QSGTexture> >& textures = (d->cache)[id];
            textures.remove(window);
            if (textures.isEmpty())
                d->cache.remove(id);
            delete texture;
        };
        texture = QSharedPointer<QSGTexture>(window->createTextureFromImage(image, options), cleanAndDelete);
        (d->cache)[id][window] = texture.toWeakRef();
    }

    //if we have a cache in an atlas but our request cannot use an atlassed texture
    //create a new texture and use that
    //don't use removedFromAtlas() as that requires keeping a reference to the non atlased version
    if (!(options & QQuickWindow::TextureCanUseAtlas) && texture->isAtlasTexture()) {
        texture = QSharedPointer<QSGTexture>(window->createTextureFromImage(image, options));
    }

    return texture;
}

QSharedPointer<QSGTexture> ImageTexturesCache::loadTexture(QQuickWindow *window, const QImage &image)
{
    return loadTexture(window, image, {});
}

