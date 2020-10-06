/*
 *  SPDX-FileCopyrightText: 2011 Marco Martin <mart@kde.org>
 *  SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *  SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once
#include <QQuickWindow>
#include <QImage>
#include <QSGSimpleTextureNode>
#include <QSGTexture>
#include <QSharedPointer>

class ManagedTextureNode : public QSGSimpleTextureNode
{
Q_DISABLE_COPY(ManagedTextureNode)
public:
    ManagedTextureNode();

    void setTexture(QSharedPointer<QSGTexture> texture);

private:
    QSharedPointer<QSGTexture> m_texture;
};

typedef QHash<qint64, QHash<QWindow*, QWeakPointer<QSGTexture> > > TexturesCache;

struct ImageTexturesCachePrivate
{
    TexturesCache cache;
};

class ImageTexturesCache
{
public:
    ImageTexturesCache();
    ~ImageTexturesCache();

    /**
     * @returns the texture for a given @p window and @p image.
     *
     * If an @p image id is the same as one already provided before, we won't create
     * a new texture and return a shared pointer to the existing texture.
     */
    QSharedPointer<QSGTexture> loadTexture(QQuickWindow *window, const QImage &image, QQuickWindow::CreateTextureOptions options);

    QSharedPointer<QSGTexture> loadTexture(QQuickWindow *window, const QImage &image);


private:
    QScopedPointer<ImageTexturesCachePrivate> d;
};

