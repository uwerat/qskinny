/*
 *  SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <QQmlProperty>

#include "sizegroup.h"

#define pThis (static_cast<SizeGroup *>(prop->object))

void SizeGroup::appendItem(QQmlListProperty<QQuickItem> *prop, QQuickItem *value)
{
    pThis->m_items << value;
    pThis->connectItem(value);
}

int SizeGroup::itemCount(QQmlListProperty<QQuickItem> *prop)
{
    return pThis->m_items.count();
}

QQuickItem* SizeGroup::itemAt(QQmlListProperty<QQuickItem> *prop, int index)
{
    return pThis->m_items[index];
}

void SizeGroup::clearItems(QQmlListProperty<QQuickItem> *prop)
{
    for (const auto &item : qAsConst(pThis->m_items)) {
        QObject::disconnect(pThis->m_connections[item].first);
        QObject::disconnect(pThis->m_connections[item].second);
    }
    pThis->m_items.clear();
}

void SizeGroup::connectItem(QQuickItem* item)
{
    auto conn1 = connect(item, &QQuickItem::implicitWidthChanged, this, [this](){ adjustItems(Mode::Width); });
    auto conn2 = connect(item, &QQuickItem::implicitHeightChanged, this, [this](){ adjustItems(Mode::Height); });
    m_connections[item] = qMakePair(conn1, conn2);
    adjustItems(m_mode);
}

QQmlListProperty<QQuickItem> SizeGroup::items()
{
    return QQmlListProperty<QQuickItem>(
        this, nullptr,
        appendItem, itemCount,
        itemAt, clearItems
    );
}

void SizeGroup::relayout()
{
    adjustItems(Mode::Both);
}

void SizeGroup::componentComplete()
{
    adjustItems(Mode::Both);
}

void SizeGroup::adjustItems(Mode whatChanged)
{
    if (m_mode == Mode::Width && whatChanged == Mode::Height)
        return;
    if (m_mode == Mode::Height && whatChanged == Mode::Width)
        return;

    qreal maxHeight = 0.0;
    qreal maxWidth = 0.0;

    for (const auto &item : qAsConst(m_items)) {
        if (item == nullptr) {
            continue;
        }

        switch (m_mode) {
        case Mode::Width:
            maxWidth = qMax(maxWidth, item->implicitWidth());
            break;
        case Mode::Height:
            maxHeight = qMax(maxHeight, item->implicitHeight());
            break;
        case Mode::Both:
            maxWidth = qMax(maxWidth, item->implicitWidth());
            maxHeight = qMax(maxHeight, item->implicitHeight());
            break;
        case Mode::None:
            break;
        }
    }

    for (const auto &item : qAsConst(m_items)) {
        if (item == nullptr) {
            continue;
        }

        switch (m_mode) {
        case Mode::Width:
            QQmlProperty(item, QStringLiteral("Layout.preferredWidth"), qmlContext(item)).write(maxWidth);
            break;
        case Mode::Height:
            QQmlProperty(item, QStringLiteral("Layout.preferredHeight"), qmlContext(item)).write(maxHeight);
            break;
        case Mode::Both:
            QQmlProperty(item, QStringLiteral("Layout.preferredWidth"), qmlContext(item)).write(maxWidth);
            QQmlProperty(item, QStringLiteral("Layout.preferredHeight"), qmlContext(item)).write(maxHeight);
            break;
        case Mode::None:
            break;
        }
    }
}
