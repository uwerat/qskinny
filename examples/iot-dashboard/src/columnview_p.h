/*
 *  SPDX-FileCopyrightText: 2019 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include "columnview.h"

#include <QQuickItem>
#include <QPointer>

class QPropertyAnimation;
class QQmlComponent;

class QmlComponentsPool: public QObject
{
    Q_OBJECT

public:
    QmlComponentsPool(QQmlEngine *engine);
    ~QmlComponentsPool();

    QQmlComponent *m_separatorComponent = nullptr;
    QQmlComponent *m_rightSeparatorComponent = nullptr;
    QObject *m_units = nullptr;

Q_SIGNALS:
    void gridUnitChanged();
    void longDurationChanged();

private:
    QObject *m_instance = nullptr;
};

class ContentItem : public QQuickItem
{
    Q_OBJECT

public:
    ContentItem(ColumnView *parent = nullptr);
    ~ContentItem();

    void layoutItems();
    void layoutPinnedItems();
    qreal childWidth(QQuickItem *child);
    void updateVisibleItems();
    void forgetItem(QQuickItem *item);
    QQuickItem *ensureSeparator(QQuickItem *item);
    QQuickItem *ensureRightSeparator(QQuickItem *item);

    void setBoundedX(qreal x);
    void animateX(qreal x);
    void snapToItem();

    inline qreal viewportLeft() const;
    inline qreal viewportRight() const;

protected:
    void itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

private Q_SLOTS:
    void syncItemsOrder();
    void updateRepeaterModel();

private:
    ColumnView *m_view;
    QPropertyAnimation *m_slideAnim;
    QList<QQuickItem *> m_items;
    QList<QObject *> m_visibleItems;
    QPointer<QQuickItem> m_viewAnchorItem;
    QHash<QQuickItem *, QQuickItem *> m_separators;
    QHash<QQuickItem *, QQuickItem *> m_rightSeparators;
    QHash<QObject *, QObject*> m_models;

    qreal m_leftPinnedSpace = 361;
    qreal m_rightPinnedSpace = 0;

    qreal m_columnWidth = 0;
    qreal m_lastDragDelta = 0;
    ColumnView::ColumnResizeMode m_columnResizeMode = ColumnView::FixedColumns;
    bool m_shouldAnimate = false;
    friend class ColumnView;
};

