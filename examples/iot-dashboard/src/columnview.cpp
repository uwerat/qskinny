/*
 *  SPDX-FileCopyrightText: 2019 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "columnview.h"
#include "columnview_p.h"

#include <QAbstractItemModel>
#include <QGuiApplication>
#include <QStyleHints>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QDebug>
#include <QPropertyAnimation>


QHash<QObject *, ColumnViewAttached *> ColumnView::m_attachedObjects = QHash<QObject *, ColumnViewAttached *>();

class QmlComponentsPoolSingleton
{
public:
    QmlComponentsPoolSingleton()
    {}
    static QmlComponentsPool *instance(QQmlEngine *engine);
private:
    QHash<QQmlEngine*, QmlComponentsPool*> m_instances;
};

Q_GLOBAL_STATIC(QmlComponentsPoolSingleton, privateQmlComponentsPoolSelf)


QmlComponentsPool *QmlComponentsPoolSingleton::instance(QQmlEngine *engine)
{
    Q_ASSERT(engine);
    auto componentPool = privateQmlComponentsPoolSelf->m_instances.value(engine);

    if (componentPool) {
        return componentPool;
    }

    componentPool = new QmlComponentsPool(engine);

    QObject::connect(componentPool, &QObject::destroyed, engine, [engine]() {
        if (privateQmlComponentsPoolSelf) {
            privateQmlComponentsPoolSelf->m_instances.remove(engine);
        }
    });
    privateQmlComponentsPoolSelf->m_instances[engine] = componentPool;
    return componentPool;
}

QmlComponentsPool::QmlComponentsPool(QQmlEngine *engine)
    : QObject(engine)
{
    QQmlComponent *component = new QQmlComponent(engine, this);

    component->setData(QByteArrayLiteral("import QtQuick 2.7\n"
        "import org.kde.kirigami 2.7 as Kirigami\n"
        "QtObject {\n"
            "id: root\n"
            "readonly property Kirigami.Units units: Kirigami.Units\n"
            "readonly property Component separator: Kirigami.Separator {"
                "property Item column\n"
                "visible: column.Kirigami.ColumnView.view && column.Kirigami.ColumnView.view.contentX < column.x;"
                "anchors.top: column.top;"
                "anchors.bottom: column.bottom;"
            "}\n"
            "readonly property Component rightSeparator: Kirigami.Separator {"
                "property Item column\n"
                "anchors.top: column.top;"
                "anchors.right: column.right;"
                "anchors.bottom: column.bottom;"
            "}"
        "}"), QUrl(QStringLiteral("columnview.cpp")));

    m_instance = component->create();
    //qWarning()<<component->errors();
    Q_ASSERT(m_instance);

    m_separatorComponent = m_instance->property("separator").value<QQmlComponent *>();
    Q_ASSERT(m_separatorComponent);

    m_rightSeparatorComponent = m_instance->property("rightSeparator").value<QQmlComponent *>();
    Q_ASSERT(m_rightSeparatorComponent);

    m_units = m_instance->property("units").value<QObject *>();
    Q_ASSERT(m_units);

    connect(m_units, SIGNAL(gridUnitChanged()), this, SIGNAL(gridUnitChanged()));
    connect(m_units, SIGNAL(longDurationChanged()), this, SIGNAL(longDurationChanged()));
}

QmlComponentsPool::~QmlComponentsPool()
{}


/////////

ColumnViewAttached::ColumnViewAttached(QObject *parent)
    : QObject(parent)
{}

ColumnViewAttached::~ColumnViewAttached()
{}

void ColumnViewAttached::setIndex(int index)
{
    if (!m_customFillWidth && m_view) {
        const bool oldFillWidth = m_fillWidth;
        m_fillWidth = index == m_view->count() - 1;
        if (oldFillWidth != m_fillWidth) {
            emit fillWidthChanged();
        }
    }

    if (index == m_index) {
        return;
    }

    m_index = index;
    emit indexChanged();
}

int ColumnViewAttached::index() const
{
    return m_index;
}

void ColumnViewAttached::setFillWidth(bool fill)
{
    if (m_view) {
        disconnect(m_view.data(), &ColumnView::countChanged, this, nullptr);
    }
    m_customFillWidth = true;

    if (fill == m_fillWidth) {
        return;
    }

    m_fillWidth = fill;
    emit fillWidthChanged();

    if (m_view) {
        m_view->polish();
    }
}

bool ColumnViewAttached::fillWidth() const
{
    return m_fillWidth;
}

qreal ColumnViewAttached::reservedSpace() const
{
    return m_reservedSpace;
}

void ColumnViewAttached::setReservedSpace(qreal space)
{
    if (m_view) {
        disconnect(m_view.data(), &ColumnView::columnWidthChanged, this, nullptr);
    }
    m_customReservedSpace = true;

    if (qFuzzyCompare(space, m_reservedSpace)) {
        return;
    }

    m_reservedSpace = space;
    emit reservedSpaceChanged();

    if (m_view) {
        m_view->polish();
    }
}

ColumnView *ColumnViewAttached::view()
{
    return m_view;
}

void ColumnViewAttached::setView(ColumnView *view)
{
    if (view == m_view) {
        return;
    }

    if (m_view) {
        disconnect(m_view.data(), nullptr, this, nullptr);
    }
    m_view = view;

    if (!m_customFillWidth && m_view) {
        m_fillWidth = m_index == m_view->count() - 1;
        connect(m_view.data(), &ColumnView::countChanged, this, [this]() {
            m_fillWidth = m_index == m_view->count() - 1;
            emit fillWidthChanged();
        });
    }
    if (!m_customReservedSpace && m_view) {
        m_reservedSpace = m_view->columnWidth();
        connect(m_view.data(), &ColumnView::columnWidthChanged, this, [this]() {
            m_reservedSpace = m_view->columnWidth();
            emit reservedSpaceChanged();
        });
    }

    emit viewChanged();
}

QQuickItem *ColumnViewAttached::originalParent() const
{
    return m_originalParent;
}

void ColumnViewAttached::setOriginalParent(QQuickItem *parent)
{
    m_originalParent = parent;
}

bool ColumnViewAttached::shouldDeleteOnRemove() const
{
    return m_shouldDeleteOnRemove;
}

void ColumnViewAttached::setShouldDeleteOnRemove(bool del)
{
    m_shouldDeleteOnRemove = del;
}

bool ColumnViewAttached::preventStealing() const
{
    return m_preventStealing;
}

void ColumnViewAttached::setPreventStealing(bool prevent)
{
    if (prevent == m_preventStealing) {
        return;
    }

    m_preventStealing = prevent;
    emit preventStealingChanged();
}

bool ColumnViewAttached::isPinned() const
{
    return m_pinned;
}

void ColumnViewAttached::setPinned(bool pinned)
{
    if (pinned == m_pinned) {
        return;
    }

    m_pinned = pinned;

    emit pinnedChanged();

    if (m_view) {
        m_view->polish();
    }
}



/////////

ContentItem::ContentItem(ColumnView *parent)
    : QQuickItem(parent),
      m_view(parent)
{
    setFlags(flags() | ItemIsFocusScope);
    m_slideAnim = new QPropertyAnimation(this);
    m_slideAnim->setTargetObject(this);
    m_slideAnim->setPropertyName("x");
    //NOTE: the duration will be taked from kirigami units upon classBegin
    m_slideAnim->setDuration(0);
    m_slideAnim->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
    connect(m_slideAnim, &QPropertyAnimation::finished, this, [this] () {
        if (!m_view->currentItem()) {
            m_view->setCurrentIndex(m_items.indexOf(m_viewAnchorItem));
        } else {
            QRectF mapped = m_view->currentItem()->mapRectToItem(m_view, QRectF(QPointF(0, 0), m_view->currentItem()->size()));
            if (!QRectF(QPointF(0, 0), m_view->size()).intersects(mapped)) {
                m_view->setCurrentIndex(m_items.indexOf(m_viewAnchorItem));
            }
        }
    });

    connect(this, &QQuickItem::xChanged, this, &ContentItem::layoutPinnedItems);
}

ContentItem::~ContentItem()
{}

void ContentItem::setBoundedX(qreal x)
{
    if (!parentItem()) {
        return;
    }
    m_slideAnim->stop();
    setX(qRound(qBound(qMin(0.0, -width()+parentItem()->width()), x, 0.0)));
}

void ContentItem::animateX(qreal newX)
{
    if (!parentItem()) {
        return;
    }

    const qreal to = qRound(qBound(qMin(0.0, -width()+parentItem()->width()), newX, 0.0));

    m_slideAnim->stop();
    m_slideAnim->setStartValue(x());
    m_slideAnim->setEndValue(to);
    m_slideAnim->start();
}

void ContentItem::snapToItem()
{
    QQuickItem *firstItem = childAt(viewportLeft(), 0);
    if (!firstItem) {
        return;
    }
    QQuickItem *nextItem = childAt(firstItem->x() + firstItem->width() + 1, 0);

    //need to make the last item visible?
    if (nextItem && ((m_view->dragging() && m_lastDragDelta < 0) ||
        (!m_view->dragging() && width() - (viewportRight()) < viewportLeft() - firstItem->x()))) {
        m_viewAnchorItem = nextItem;
        animateX(-nextItem->x() + m_leftPinnedSpace);

    //The first one found?
    } else if ((m_view->dragging() && m_lastDragDelta >= 0) ||
        (!m_view->dragging() && viewportLeft() <= firstItem->x() + firstItem->width()/2) ||
        !nextItem) {
        m_viewAnchorItem = firstItem;
        animateX(-firstItem->x() + m_leftPinnedSpace);

    //the second?
    } else {
        m_viewAnchorItem = nextItem;
        animateX(-nextItem->x() + m_leftPinnedSpace);
    }
}

qreal ContentItem::viewportLeft() const
{
    return -x() + m_leftPinnedSpace;
}

qreal ContentItem::viewportRight() const
{
    return -x() + m_view->width() - m_rightPinnedSpace;
}

qreal ContentItem::childWidth(QQuickItem *child)
{
    if (!parentItem()) {
        return 0.0;
    }

    ColumnViewAttached *attached = qobject_cast<ColumnViewAttached *>(qmlAttachedPropertiesObject<ColumnView>(child, true));

    if (m_columnResizeMode == ColumnView::SingleColumn) {
        return qRound(parentItem()->width());

    } else if (attached->fillWidth()) {
        return qRound(qBound(m_columnWidth, (parentItem()->width() - attached->reservedSpace()), parentItem()->width()));

    } else if (m_columnResizeMode == ColumnView::FixedColumns) {
        return qRound(qMin(parentItem()->width(), m_columnWidth));

    // DynamicColumns
    } else {
        //TODO:look for Layout size hints
        qreal width = child->implicitWidth();

        if (width < 1.0) {
            width = m_columnWidth;
        }

        return qRound(qMin(m_view->width(), width));
    }
}

void ContentItem::layoutItems()
{
    setY(m_view->topPadding());
    setHeight(m_view->height() - m_view->topPadding() - m_view->bottomPadding());

    qreal implicitWidth = 0;
    qreal implicitHeight = 0;
    qreal partialWidth = 0;
    int i = 0;
    m_leftPinnedSpace = 0;
    m_rightPinnedSpace = 0;
    for (QQuickItem *child : qAsConst(m_items)) {
        ColumnViewAttached *attached = qobject_cast<ColumnViewAttached *>(qmlAttachedPropertiesObject<ColumnView>(child, true));

        if (child->isVisible()) {
            if (attached->isPinned() && m_view->columnResizeMode() != ColumnView::SingleColumn) {
                QQuickItem *sep = nullptr;
                int sepWidth = 0;
                if (m_view->separatorVisible()) {
                    sep = ensureRightSeparator(child);
                    sepWidth = (sep ? sep->width() : 0);
                }
                const qreal width = childWidth(child);
                child->setSize(QSizeF(width + sepWidth, height()));

                child->setPosition(QPointF(qMin(qMax(-x(), partialWidth), -x() + m_view->width() - child->width() + sepWidth), 0.0));
                child->setZ(1);

                if (partialWidth <= -x()) {
                    m_leftPinnedSpace = qMax(m_leftPinnedSpace, width);
                } else if (partialWidth > -x() + m_view->width() - child->width() + sepWidth) {
                    m_rightPinnedSpace = qMax(m_rightPinnedSpace, child->width());
                }

                partialWidth += width;

            } else {
                child->setSize(QSizeF(childWidth(child), height()));

                auto it = m_rightSeparators.find(child);
                if (it != m_rightSeparators.end()) {
                    it.value()->deleteLater();
                    m_rightSeparators.erase(it);
                }
                child->setPosition(QPointF(partialWidth, 0.0));
                child->setZ(0);

                partialWidth += child->width();
            }
        }

        attached->setIndex(i++);

        implicitWidth += child->implicitWidth();

        implicitHeight = qMax(implicitHeight, child->implicitHeight());
    }

    setWidth(partialWidth);

    setImplicitWidth(implicitWidth);
    setImplicitHeight(implicitHeight);

    m_view->setImplicitWidth(implicitWidth);
    m_view->setImplicitHeight(implicitHeight + m_view->topPadding() + m_view->bottomPadding());

    const qreal newContentX = m_viewAnchorItem ? -m_viewAnchorItem->x() : 0.0;
    if (m_shouldAnimate) {
        animateX(newContentX);
    } else {
        setBoundedX(newContentX);
    }

    updateVisibleItems();
}

void ContentItem::layoutPinnedItems()
{
    if (m_view->columnResizeMode() == ColumnView::SingleColumn) {
        return;
    }

    qreal partialWidth = 0;
    m_leftPinnedSpace = 0;
    m_rightPinnedSpace = 0;

    for (QQuickItem *child : qAsConst(m_items)) {
        ColumnViewAttached *attached = qobject_cast<ColumnViewAttached *>(qmlAttachedPropertiesObject<ColumnView>(child, true));

        if (child->isVisible()) {
            if (attached->isPinned()) {
                QQuickItem *sep = nullptr;
                int sepWidth = 0;
                if (m_view->separatorVisible()) {
                    sep = ensureRightSeparator(child);
                    sepWidth = (sep ? sep->width() : 0);
                }

                child->setPosition(QPointF(qMin(qMax(-x(), partialWidth), -x() + m_view->width() - child->width() + sepWidth), 0.0));

                if (partialWidth <= -x()) {
                    m_leftPinnedSpace = qMax(m_leftPinnedSpace, child->width() - sepWidth);
                } else if (partialWidth > -x() + m_view->width() - child->width() + sepWidth) {
                    m_rightPinnedSpace = qMax(m_rightPinnedSpace, child->width());
                }
            }

            partialWidth += child->width();
        }
    }
}

void ContentItem::updateVisibleItems()
{
    QList <QObject *> newItems;

    for (auto *item : qAsConst(m_items)) {
        if (item->isVisible() && item->x() + x() < width() && item->x() + item->width() + x() > 0) {
            newItems << item;
            connect(item, &QObject::destroyed, this, [this, item] {
                m_visibleItems.removeAll(item);
            });
        }
    }

    for (auto *item : qAsConst(m_visibleItems)) {
        disconnect(item, &QObject::destroyed, this, nullptr);
    }
    const QQuickItem *oldFirstVisibleItem = m_visibleItems.isEmpty() ? nullptr : qobject_cast<QQuickItem *>(m_visibleItems.first());
    const QQuickItem *oldLastVisibleItem = m_visibleItems.isEmpty() ? nullptr : qobject_cast<QQuickItem *>(m_visibleItems.last());

    if (newItems != m_visibleItems) {
        m_visibleItems = newItems;
        emit m_view->visibleItemsChanged();
        if (!newItems.isEmpty() && m_visibleItems.first() != oldFirstVisibleItem) {
            emit m_view->firstVisibleItemChanged();
        }
        if (!newItems.isEmpty() && m_visibleItems.last() != oldLastVisibleItem) {
            emit m_view->lastVisibleItemChanged();
        }
    }
}

void ContentItem::forgetItem(QQuickItem *item)
{
    if (!m_items.contains(item)) {
        return;
    }

    ColumnViewAttached *attached = qobject_cast<ColumnViewAttached *>(qmlAttachedPropertiesObject<ColumnView>(item, true));
    attached->setView(nullptr);
    attached->setIndex(-1);

    disconnect(attached, nullptr, this, nullptr);
    disconnect(item, nullptr, this, nullptr);
    disconnect(item, nullptr, m_view, nullptr);

    QQuickItem *separatorItem = m_separators.take(item);
    if (separatorItem) {
        separatorItem->deleteLater();
    }
    separatorItem = m_rightSeparators.take(item);
    if (separatorItem) {
        separatorItem->deleteLater();
    }

    const int index = m_items.indexOf(item);
    m_items.removeAll(item);
    disconnect(item, &QObject::destroyed, this, nullptr);
    updateVisibleItems();
    m_shouldAnimate = true;
    m_view->polish();
    item->setVisible(false);

    if (index <= m_view->currentIndex()) {
        m_view->setCurrentIndex(qBound(0, index - 1, m_items.count() - 1));
    }
    emit m_view->countChanged();
}

QQuickItem *ContentItem::ensureSeparator(QQuickItem *item)
{
    QQuickItem *separatorItem = m_separators.value(item);

    if (!separatorItem) {
        separatorItem = qobject_cast<QQuickItem *>(privateQmlComponentsPoolSelf->instance(qmlEngine(item))->m_separatorComponent->beginCreate(QQmlEngine::contextForObject(item)));
        if (separatorItem) {
            separatorItem->setParentItem(item);
            separatorItem->setZ(9999);
            separatorItem->setProperty("column", QVariant::fromValue(item));
            QmlComponentsPoolSingleton::instance(qmlEngine(item))->m_separatorComponent->completeCreate();
            m_separators[item] = separatorItem;
        }
    }

    return separatorItem;
}

QQuickItem *ContentItem::ensureRightSeparator(QQuickItem *item)
{
    QQuickItem *separatorItem = m_rightSeparators.value(item);

    if (!separatorItem) {
        separatorItem = qobject_cast<QQuickItem *>(QmlComponentsPoolSingleton::instance(qmlEngine(item))->m_rightSeparatorComponent->beginCreate(QQmlEngine::contextForObject(item)));
        if (separatorItem) {
            separatorItem->setParentItem(item);
            separatorItem->setZ(9999);
            separatorItem->setProperty("column", QVariant::fromValue(item));
            QmlComponentsPoolSingleton::instance(qmlEngine(item))->m_rightSeparatorComponent->completeCreate();
            m_rightSeparators[item] = separatorItem;
        }
    }

    return separatorItem;
}

void ContentItem::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value)
{
    switch (change) {
    case QQuickItem::ItemChildAddedChange: {
        ColumnViewAttached *attached = qobject_cast<ColumnViewAttached *>(qmlAttachedPropertiesObject<ColumnView>(value.item, true));
        attached->setView(m_view);

        //connect(attached, &ColumnViewAttached::fillWidthChanged, m_view, &ColumnView::polish);
        connect(attached, &ColumnViewAttached::fillWidthChanged, this, [this] {
             m_view->polish();
        });
        connect(attached, &ColumnViewAttached::reservedSpaceChanged, m_view, &ColumnView::polish);

        value.item->setVisible(true);

        if (!m_items.contains(value.item)) {
            connect(value.item, &QQuickItem::widthChanged, m_view, &ColumnView::polish);
            QQuickItem *item = value.item;
            m_items << item;
            connect(item, &QObject::destroyed, this, [this, item]() {
                m_view->removeItem(item);
            });
        }

        if (m_view->separatorVisible()) {
            ensureSeparator(value.item);
        }

        m_shouldAnimate = true;
        m_view->polish();
        emit m_view->countChanged();
        break;
    }
    case QQuickItem::ItemChildRemovedChange: {
        forgetItem(value.item);
        break;
    }
    case QQuickItem::ItemVisibleHasChanged:
        updateVisibleItems();
        if (value.boolValue) {
            m_view->polish();
        }
        break;
    default:
        break;
    }
    QQuickItem::itemChange(change, value);
}

void ContentItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    updateVisibleItems();
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

void ContentItem::syncItemsOrder()
{
    if (m_items == childItems()) {
        return;
    }

    m_items = childItems();
    //NOTE: polish() here sometimes gets indefinitely delayed and items chaging order isn't seen
    layoutItems();
}

void ContentItem::updateRepeaterModel()
{
    if (!sender()) {
        return;
    }

    QObject *modelObj = sender()->property("model").value<QObject *>();

    if (!modelObj) {
        m_models.remove(sender());
        return;
    }

    if (m_models[sender()]) {
        disconnect(m_models[sender()], nullptr, this, nullptr);
    }

    m_models[sender()] = modelObj;

    QAbstractItemModel *qaim = qobject_cast<QAbstractItemModel *>(modelObj);

    if (qaim) {
        connect(qaim, &QAbstractItemModel::rowsMoved, this, &ContentItem::syncItemsOrder);

    } else {
        connect(modelObj, SIGNAL(childrenChanged()), this, SLOT(syncItemsOrder()));
    }
}







ColumnView::ColumnView(QQuickItem *parent)
    : QQuickItem(parent),
      m_contentItem(nullptr)
{
    //NOTE: this is to *not* trigger itemChange
    m_contentItem = new ContentItem(this);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::BackButton | Qt::ForwardButton);
    setFiltersChildMouseEvents(true);

    connect(m_contentItem->m_slideAnim, &QPropertyAnimation::finished, this, [this] () {
        m_moving = false;
        emit movingChanged();
    });
    connect(m_contentItem, &ContentItem::widthChanged, this, &ColumnView::contentWidthChanged);
    connect(m_contentItem, &ContentItem::xChanged, this, &ColumnView::contentXChanged);

    connect(this, &ColumnView::activeFocusChanged, this, [this]() {
        if (hasActiveFocus() && m_currentItem) {
            m_currentItem->forceActiveFocus();
        }
    });
    ColumnViewAttached *attached = qobject_cast<ColumnViewAttached *>(qmlAttachedPropertiesObject<ColumnView>(this, true));
    attached->setView(this);
    attached = qobject_cast<ColumnViewAttached *>(qmlAttachedPropertiesObject<ColumnView>(m_contentItem, true));
    attached->setView(this);
}

ColumnView::~ColumnView()
{
}

ColumnView::ColumnResizeMode ColumnView::columnResizeMode() const
{
    return m_contentItem->m_columnResizeMode;
}

void ColumnView::setColumnResizeMode(ColumnResizeMode mode)
{
    if (m_contentItem->m_columnResizeMode == mode) {
        return;
    }

    m_contentItem->m_columnResizeMode = mode;
    if (mode == SingleColumn && m_currentItem) {
        m_contentItem->m_viewAnchorItem = m_currentItem;
    }
    m_contentItem->m_shouldAnimate = false;
    polish();
    emit columnResizeModeChanged();
}

qreal ColumnView::columnWidth() const
{
    return m_contentItem->m_columnWidth;
}

void ColumnView::setColumnWidth(qreal width)
{
    // Always forget the internal binding when the user sets anything, even the same value
    disconnect(QmlComponentsPoolSingleton::instance(qmlEngine(this)), &QmlComponentsPool::gridUnitChanged, this, nullptr);

    if (m_contentItem->m_columnWidth == width) {
        return;
    }

    m_contentItem->m_columnWidth = width;
    m_contentItem->m_shouldAnimate = false;
    polish();
    emit columnWidthChanged();
}

int ColumnView::currentIndex() const
{
    return m_currentIndex;
}

void ColumnView::setCurrentIndex(int index)
{
    if (m_currentIndex == index || index < -1 || index >= m_contentItem->m_items.count()) {
        return;
    }

    m_currentIndex = index;

    if (index == -1) {
        m_currentItem.clear();

    } else {
        m_currentItem = m_contentItem->m_items[index];
        Q_ASSERT(m_currentItem);
        m_currentItem->forceActiveFocus();

        // If the current item is not on view, scroll
        QRectF mappedCurrent = m_currentItem->mapRectToItem(this,
                                            QRectF(QPointF(0, 0),
                                                   m_currentItem->size()));

        if (m_contentItem->m_slideAnim->state() == QAbstractAnimation::Running) {
            mappedCurrent.moveLeft(mappedCurrent.left() + m_contentItem->x() + m_contentItem->m_slideAnim->endValue().toInt());
        }

        //m_contentItem->m_slideAnim->stop();

        QRectF contentsRect(m_contentItem->m_leftPinnedSpace,
                            0,
                            width() - m_contentItem->m_rightPinnedSpace - m_contentItem->m_leftPinnedSpace,
                            height());

        m_contentItem->m_shouldAnimate = true;

        if (!m_mouseDown) {
            if (!contentsRect.contains(mappedCurrent)) {
                m_contentItem->m_viewAnchorItem = m_currentItem;
                m_contentItem->animateX(-m_currentItem->x() + m_contentItem->m_leftPinnedSpace);
            } else {
                m_contentItem->snapToItem();
            }
        }
    }

    emit currentIndexChanged();
    emit currentItemChanged();
}

QQuickItem *ColumnView::currentItem()
{
    return m_currentItem;
}

QList<QObject *>ColumnView::visibleItems() const
{
    return m_contentItem->m_visibleItems;
}

QQuickItem *ColumnView::firstVisibleItem() const
{
    if (m_contentItem->m_visibleItems.isEmpty()) {
        return nullptr;
    }

    return qobject_cast<QQuickItem *>(m_contentItem->m_visibleItems.first());
}

QQuickItem *ColumnView::lastVisibleItem() const
{
    if (m_contentItem->m_visibleItems.isEmpty()) {
        return nullptr;
    }

    return qobject_cast<QQuickItem *>(m_contentItem->m_visibleItems.last());
}

int ColumnView::count() const
{
    return m_contentItem->m_items.count();
}

qreal ColumnView::topPadding() const
{
    return m_topPadding;
}

void ColumnView::setTopPadding(qreal padding)
{
    if (padding == m_topPadding) {
        return;
    }

    m_topPadding = padding;
    polish();
    emit topPaddingChanged();
}

qreal ColumnView::bottomPadding() const
{
    return m_bottomPadding;
}

void ColumnView::setBottomPadding(qreal padding)
{
    if (padding == m_bottomPadding) {
        return;
    }

    m_bottomPadding = padding;
    polish();
    emit bottomPaddingChanged();
}


QQuickItem *ColumnView::contentItem() const
{
    return m_contentItem;
}

int ColumnView::scrollDuration() const
{
    return m_contentItem->m_slideAnim->duration();
}

void ColumnView::setScrollDuration(int duration)
{
    disconnect(QmlComponentsPoolSingleton::instance(qmlEngine(this)), &QmlComponentsPool::longDurationChanged, this, nullptr);

    if (m_contentItem->m_slideAnim->duration() == duration) {
        return;
    }

    m_contentItem->m_slideAnim->setDuration(duration);
    emit scrollDurationChanged();
}

bool ColumnView::separatorVisible() const
{
    return m_separatorVisible;
}

void ColumnView::setSeparatorVisible(bool visible)
{
    if (visible == m_separatorVisible) {
        return;
    }

    m_separatorVisible = visible;

    if (visible) {
        for (QQuickItem *item : qAsConst(m_contentItem->m_items)) {
            QQuickItem *sep = m_contentItem->ensureSeparator(item);
            if (sep) {
                sep->setVisible(true);
            }

            ColumnViewAttached *attached = qobject_cast<ColumnViewAttached *>(qmlAttachedPropertiesObject<ColumnView>(item, true));
            if (attached->isPinned()) {
                QQuickItem *sep = m_contentItem->ensureRightSeparator(item);
                if (sep) {
                    sep->setVisible(true);
                }
            }
        }

    } else {
        for (QQuickItem *sep : qAsConst(m_contentItem->m_separators)) {
            sep->setVisible(false);
        }
        for (QQuickItem *sep : qAsConst(m_contentItem->m_rightSeparators)) {
            sep->setVisible(false);
        }
    }

    emit separatorVisibleChanged();
}

bool ColumnView::dragging() const
{
    return m_dragging;
}

bool ColumnView::moving() const
{
    return m_moving;
}

qreal ColumnView::contentWidth() const
{
    return m_contentItem->width();
}

qreal ColumnView::contentX() const
{
    return -m_contentItem->x();
}

void ColumnView::setContentX(qreal x) const
{
    m_contentItem->setX(qRound(-x));
}

bool ColumnView::interactive() const
{
    return m_interactive;
}

void ColumnView::setInteractive(bool interactive)
{
    if (m_interactive == interactive) {
        return;
    }

    m_interactive = interactive;

    if (!m_interactive) {
        if (m_dragging) {
            m_dragging = false;
            emit draggingChanged();
        }

        m_contentItem->snapToItem();
        setKeepMouseGrab(false);
    }

    emit interactiveChanged();
}

void ColumnView::addItem(QQuickItem *item)
{
    insertItem(m_contentItem->m_items.length(), item);
}

void ColumnView::insertItem(int pos, QQuickItem *item)
{
    if (!item || m_contentItem->m_items.contains(item)) {
        return;
    }

    m_contentItem->m_items.insert(qBound(0, pos, m_contentItem->m_items.length()), item);

    connect(item, &QObject::destroyed, m_contentItem, [this, item]() {
        removeItem(item);
    });
    ColumnViewAttached *attached = qobject_cast<ColumnViewAttached *>(qmlAttachedPropertiesObject<ColumnView>(item, true));
    attached->setOriginalParent(item->parentItem());
    attached->setShouldDeleteOnRemove(item->parentItem() == nullptr && QQmlEngine::objectOwnership(item) == QQmlEngine::JavaScriptOwnership);
    item->setParentItem(m_contentItem);

    item->forceActiveFocus();
    // We layout immediately to be sure all geometries are final after the return of this call
    m_contentItem->m_shouldAnimate = false;
    m_contentItem->layoutItems();
    emit contentChildrenChanged();

    // In order to keep the same current item we need to increase the current index if displaced
    // NOTE: just updating m_currentIndex does *not* update currentItem (which is what we need atm) while setCurrentIndex will update also currentItem
    if (m_currentIndex >= pos) {
        ++m_currentIndex;
        emit currentIndexChanged();
    }

    emit itemInserted(pos, item);
}

void ColumnView::moveItem(int from, int to)
{
    if (m_contentItem->m_items.isEmpty()
        || from < 0 || from >= m_contentItem->m_items.length()
        || to < 0 || to >= m_contentItem->m_items.length()) {
        return;
    }

    m_contentItem->m_items.move(from, to);
    m_contentItem->m_shouldAnimate = true;

    if (from == m_currentIndex) {
        m_currentIndex = to;
        emit currentIndexChanged();
    } else if (from < m_currentIndex && to > m_currentIndex) {
        --m_currentIndex;
        emit currentIndexChanged();
    } else if (from > m_currentIndex && to <= m_currentIndex) {
        ++m_currentIndex;
        emit currentIndexChanged();
    }

    polish();
}

QQuickItem *ColumnView::removeItem(const QVariant &item)
{
    if (item.canConvert<QQuickItem *>()) {
        return removeItem(item.value<QQuickItem *>());
    } else if (item.canConvert<int>()) {
        return removeItem(item.toInt());
    } else {
        return nullptr;
    }
}

QQuickItem *ColumnView::removeItem(QQuickItem *item)
{
    if (m_contentItem->m_items.isEmpty() || !m_contentItem->m_items.contains(item)) {
        return nullptr;
    }

    const int index = m_contentItem->m_items.indexOf(item);

    // In order to keep the same current item we need to increase the current index if displaced
    if (m_currentIndex >= index) {
        setCurrentIndex(m_currentIndex - 1);
    }

    m_contentItem->forgetItem(item);

    ColumnViewAttached *attached = qobject_cast<ColumnViewAttached *>(qmlAttachedPropertiesObject<ColumnView>(item, false));

    if (attached && attached->shouldDeleteOnRemove()) {
        item->deleteLater();
    } else {
        item->setParentItem(attached ? attached->originalParent() : nullptr);
    }

    emit itemRemoved(item);

    return item;
}

QQuickItem *ColumnView::removeItem(int pos)
{
    if (m_contentItem->m_items.isEmpty()
        || pos < 0 || pos >= m_contentItem->m_items.length()) {
        return nullptr;
    }

    return removeItem(m_contentItem->m_items[pos]);
}

QQuickItem *ColumnView::pop(QQuickItem *item)
{
    QQuickItem *removed = nullptr;

    while (!m_contentItem->m_items.isEmpty() && m_contentItem->m_items.last() != item) {
        removed = removeItem(m_contentItem->m_items.last());
        // if no item has been passed, just pop one
        if (!item) {
            break;
        }
    }
    return removed;
}

void ColumnView::clear()
{
    for (QQuickItem *item : qAsConst(m_contentItem->m_items)) {
        removeItem(item);
    }
    m_contentItem->m_items.clear();
    emit contentChildrenChanged();
}

bool ColumnView::containsItem(QQuickItem *item)
{
    return m_contentItem->m_items.contains(item);
}

QQuickItem *ColumnView::itemAt(qreal x, qreal y)
{
    return m_contentItem->childAt(x, y);
}

ColumnViewAttached *ColumnView::qmlAttachedProperties(QObject *object)
{
    return new ColumnViewAttached(object);
}

void ColumnView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    m_contentItem->setY(m_topPadding);
    m_contentItem->setHeight(newGeometry.height() - m_topPadding - m_bottomPadding);
    m_contentItem->m_shouldAnimate = false;
    polish();

    m_contentItem->updateVisibleItems();
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

bool ColumnView::childMouseEventFilter(QQuickItem *item, QEvent *event)
{
    if (!m_interactive || item == m_contentItem) {
        return QQuickItem::childMouseEventFilter(item, event);
    }

    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);

        if (me->button() != Qt::LeftButton) {
            return false;
        }

        m_contentItem->m_slideAnim->stop();
        if (item->property("preventStealing").toBool()) {
            m_contentItem->snapToItem();
            return false;
        }
        m_oldMouseX = m_startMouseX = mapFromItem(item, me->localPos()).x();

        m_mouseDown = true;
        me->setAccepted(false);
        setKeepMouseGrab(false);

        // On press, we set the current index of the view to the root item
        QQuickItem *candidateItem = item;
        while (candidateItem->parentItem() && candidateItem->parentItem() != m_contentItem) {
            candidateItem = candidateItem->parentItem();
        }
        if (candidateItem->parentItem() == m_contentItem) {
            setCurrentIndex(m_contentItem->m_items.indexOf(candidateItem));
        }
        break;
    }
    case QEvent::MouseMove: {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);

        if (!(me->buttons() & Qt::LeftButton)) {
            return false;
        }

        if ((!keepMouseGrab() && item->keepMouseGrab()) || item->property("preventStealing").toBool()) {
            m_contentItem->snapToItem();
            return false;
        }

        QQuickItem *candidateItem = item;
        while (candidateItem->parentItem() && candidateItem->parentItem() != m_contentItem) {
            candidateItem = candidateItem->parentItem();
        }
        if (candidateItem->parentItem() == m_contentItem) {
            ColumnViewAttached *attached = qobject_cast<ColumnViewAttached *>(qmlAttachedPropertiesObject<ColumnView>(candidateItem, true));
            if (attached->preventStealing()) {
                return false;
            }
        }

        const QPointF pos = mapFromItem(item, me->localPos());

        const bool wasDragging = m_dragging;
        // If a drag happened, start to steal all events, use startDragDistance * 2 to give time to widgets to take the mouse grab by themselves
        m_dragging = keepMouseGrab() || qAbs(mapFromItem(item, me->localPos()).x() - m_startMouseX) > qApp->styleHints()->startDragDistance() * 3;

        if (m_dragging != wasDragging) {
            m_moving = true;
            emit movingChanged();
            emit draggingChanged();
        }

        if (m_dragging) {
            m_contentItem->setBoundedX(m_contentItem->x() + pos.x() - m_oldMouseX);
        }

        m_contentItem->m_lastDragDelta = pos.x() - m_oldMouseX;
        m_oldMouseX = pos.x();

        setKeepMouseGrab(m_dragging);
        me->setAccepted(m_dragging);
        return m_dragging;
        break;
    }
    case QEvent::MouseButtonRelease: {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);

        if (me->button() == Qt::BackButton && m_currentIndex > 0) {
            setCurrentIndex(m_currentIndex - 1);
            me->accept();
            return true;
        } else if (me->button() == Qt::ForwardButton) {
            setCurrentIndex(m_currentIndex + 1);
            me->accept();
            return true;
        }

        if (me->button() != Qt::LeftButton) {
            return false;
        }

        m_mouseDown = false;

        m_contentItem->snapToItem();
        m_contentItem->m_lastDragDelta = 0;
        if (m_dragging) {
            m_dragging = false;
            emit draggingChanged();
        }

        if (item->property("preventStealing").toBool()) {
            return false;
        }

        event->accept();

        //if a drag happened, don't pass the event
        const bool block = keepMouseGrab();
        setKeepMouseGrab(false);

        me->setAccepted(block);
        return block;
        break;
    }
    default:
        break;
    }

    return QQuickItem::childMouseEventFilter(item, event);
}

void ColumnView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::BackButton || event->button() == Qt::ForwardButton) {
        event->accept();
        return;
    }

    if (!m_interactive) {
        return;
    }

    m_contentItem->snapToItem();
    m_oldMouseX = event->localPos().x();
    m_startMouseX = event->localPos().x();
    m_mouseDown = true;
    setKeepMouseGrab(false);
    event->accept();
}

void ColumnView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::BackButton || event->buttons() & Qt::ForwardButton) {
        event->accept();
        return;
    }

    if (!m_interactive) {
        return;
    }

    const bool wasDragging = m_dragging;
    // Same startDragDistance * 2 as the event filter
    m_dragging = keepMouseGrab() || qAbs(event->localPos().x() - m_startMouseX) > qApp->styleHints()->startDragDistance() * 2;
    if (m_dragging != wasDragging) {
        m_moving = true;
        emit movingChanged();
        emit draggingChanged();
    }

    setKeepMouseGrab(m_dragging);

    if (m_dragging) {
        m_contentItem->setBoundedX(m_contentItem->x() + event->pos().x() - m_oldMouseX);
    }

    m_contentItem->m_lastDragDelta = event->pos().x() - m_oldMouseX;
    m_oldMouseX = event->pos().x();
    event->accept();
}

void ColumnView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::BackButton && m_currentIndex > 0) {
        setCurrentIndex(m_currentIndex - 1);
        event->accept();
        return;
    } else if (event->button() == Qt::ForwardButton) {
        setCurrentIndex(m_currentIndex + 1);
        event->accept();
        return;
    }

    m_mouseDown = false;

    if (!m_interactive) {
        return;
    }

    m_contentItem->snapToItem();
    m_contentItem->m_lastDragDelta = 0;

    if (m_dragging) {
        m_dragging = false;
        emit draggingChanged();
    }

    setKeepMouseGrab(false);
    event->accept();
}

void ColumnView::mouseUngrabEvent()
{
    m_mouseDown = false;

    if (m_contentItem->m_slideAnim->state() != QAbstractAnimation::Running) {
        m_contentItem->snapToItem();
    }
    m_contentItem->m_lastDragDelta = 0;

    if (m_dragging) {
        m_dragging = false;
        emit draggingChanged();
    }

    setKeepMouseGrab(false);
}

void ColumnView::classBegin()
{
    auto syncColumnWidth = [this]() {
        m_contentItem->m_columnWidth = privateQmlComponentsPoolSelf->instance(qmlEngine(this))->m_units->property("gridUnit").toInt() * 20;
        emit columnWidthChanged();
    };

    connect(QmlComponentsPoolSingleton::instance(qmlEngine(this)), &QmlComponentsPool::gridUnitChanged, this, syncColumnWidth);
    syncColumnWidth();

    auto syncDuration = [this]() {
        m_contentItem->m_slideAnim->setDuration(QmlComponentsPoolSingleton::instance(qmlEngine(this))->m_units->property("longDuration").toInt());
        emit scrollDurationChanged();
    };

    connect(QmlComponentsPoolSingleton::instance(qmlEngine(this)), &QmlComponentsPool::longDurationChanged, this, syncDuration);
    syncDuration();

    QQuickItem::classBegin();
}

void ColumnView::componentComplete()
{
    m_complete = true;
    QQuickItem::componentComplete();
}

void ColumnView::updatePolish()
{
    m_contentItem->layoutItems();
}

void ColumnView::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value)
{
    switch (change) {
    case QQuickItem::ItemChildAddedChange:
        if (m_contentItem && value.item != m_contentItem && !value.item->inherits("QQuickRepeater")) {
            addItem(value.item);
        }
        break;
    default:
        break;
    }
    QQuickItem::itemChange(change, value);
}

void ColumnView::contentChildren_append(QQmlListProperty<QQuickItem> *prop, QQuickItem *item)
{
    // This can only be called from QML
    ColumnView *view = static_cast<ColumnView *>(prop->object);
    if (!view) {
        return;
    }

    view->m_contentItem->m_items.append(item);
    connect(item, &QObject::destroyed, view->m_contentItem, [view, item]() {
        view->removeItem(item);
    });

    ColumnViewAttached *attached = qobject_cast<ColumnViewAttached *>(qmlAttachedPropertiesObject<ColumnView>(item, true));
    attached->setOriginalParent(item->parentItem());
    attached->setShouldDeleteOnRemove(item->parentItem() == nullptr && QQmlEngine::objectOwnership(item) == QQmlEngine::JavaScriptOwnership);

    item->setParentItem(view->m_contentItem);
}

int ColumnView::contentChildren_count(QQmlListProperty<QQuickItem> *prop)
{
    ColumnView *view = static_cast<ColumnView *>(prop->object);
    if (!view) {
        return 0;
    }

    return view->m_contentItem->m_items.count();
}

QQuickItem *ColumnView::contentChildren_at(QQmlListProperty<QQuickItem> *prop, int index)
{
    ColumnView *view = static_cast<ColumnView *>(prop->object);
    if (!view) {
        return nullptr;
    }

    if (index < 0 || index >= view->m_contentItem->m_items.count()) {
        return nullptr;
    }
    return view->m_contentItem->m_items.value(index);
}

void ColumnView::contentChildren_clear(QQmlListProperty<QQuickItem> *prop)
{
    ColumnView *view = static_cast<ColumnView *>(prop->object);
    if (!view) {
        return;
    }

    return view->m_contentItem->m_items.clear();
}

QQmlListProperty<QQuickItem> ColumnView::contentChildren()
{
    return QQmlListProperty<QQuickItem>(this, nullptr,
                                     contentChildren_append,
                                     contentChildren_count,
                                     contentChildren_at,
                                     contentChildren_clear);
}

void ColumnView::contentData_append(QQmlListProperty<QObject> *prop, QObject *object)
{
    ColumnView *view = static_cast<ColumnView *>(prop->object);
    if (!view) {
        return;
    }

    view->m_contentData.append(object);
    QQuickItem *item = qobject_cast<QQuickItem *>(object);
    //exclude repeaters from layout
    if (item && item->inherits("QQuickRepeater")) {
        item->setParentItem(view);

        connect(item, SIGNAL(modelChanged()), view->m_contentItem, SLOT(updateRepeaterModel()));

    } else if (item) {
        view->m_contentItem->m_items.append(item);
        connect(item, &QObject::destroyed, view->m_contentItem, [view, item]() {
            view->removeItem(item);
        });

        ColumnViewAttached *attached = qobject_cast<ColumnViewAttached *>(qmlAttachedPropertiesObject<ColumnView>(item, true));
        attached->setOriginalParent(item->parentItem());
        attached->setShouldDeleteOnRemove(view->m_complete && !item->parentItem() && QQmlEngine::objectOwnership(item) == QQmlEngine::JavaScriptOwnership);

        item->setParentItem(view->m_contentItem);

    } else {
        object->setParent(view);
    }
}

int ColumnView::contentData_count(QQmlListProperty<QObject> *prop)
{
    ColumnView *view = static_cast<ColumnView *>(prop->object);
    if (!view) {
        return 0;
    }

    return view->m_contentData.count();
}

QObject *ColumnView::contentData_at(QQmlListProperty<QObject> *prop, int index)
{
    ColumnView *view = static_cast<ColumnView *>(prop->object);
    if (!view) {
        return nullptr;
    }

    if (index < 0 || index >= view->m_contentData.count()) {
        return nullptr;
    }
    return view->m_contentData.value(index);
}

void ColumnView::contentData_clear(QQmlListProperty<QObject> *prop)
{
    ColumnView *view = static_cast<ColumnView *>(prop->object);
    if (!view) {
        return;
    }

    return view->m_contentData.clear();
}

QQmlListProperty<QObject> ColumnView::contentData()
{
    return QQmlListProperty<QObject>(this, nullptr,
                                     contentData_append,
                                     contentData_count,
                                     contentData_at,
                                     contentData_clear);
}

#include "moc_columnview.cpp"
