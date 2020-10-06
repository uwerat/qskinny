/*
 *  SPDX-FileCopyrightText: 2017 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "scenepositionattached.h"
#include <QQuickItem>
#include <QDebug>

ScenePositionAttached::ScenePositionAttached(QObject *parent)
    : QObject(parent)
{
    m_item = qobject_cast<QQuickItem *>(parent);
    connectAncestors(m_item);
}

ScenePositionAttached::~ScenePositionAttached()
{
}

int ScenePositionAttached::x() const
{
    qreal x = 0;
    QQuickItem *item = m_item;

    while (item) {
        x += item->x();
        item = item->parentItem();
    }

    return x;
}

int ScenePositionAttached::y() const
{
    qreal y = 0;
    QQuickItem *item = m_item;

    while (item) {
        y += item->y();
        item = item->parentItem();
    }

    return y;
}

void ScenePositionAttached::connectAncestors(QQuickItem *item)
{
    if (!item) {
        return;
    }

    QQuickItem *ancestor = item;
    while (ancestor) {
        m_ancestors << ancestor;

        connect(ancestor, &QQuickItem::xChanged, this, &ScenePositionAttached::xChanged);
        connect(ancestor, &QQuickItem::yChanged, this, &ScenePositionAttached::yChanged);
        connect(ancestor, &QQuickItem::parentChanged, this,
            [this, ancestor]() {
                do {
                    disconnect(ancestor, nullptr, this, nullptr);
                    m_ancestors.pop_back();
                } while (!m_ancestors.isEmpty() && m_ancestors.last() != ancestor);

                connectAncestors(ancestor);
                emit xChanged();
                emit yChanged();
            }
        );

        ancestor = ancestor->parentItem();
    }
}

ScenePositionAttached *ScenePositionAttached::qmlAttachedProperties(QObject *object)
{
    return new ScenePositionAttached(object);
}

#include "moc_scenepositionattached.cpp"
