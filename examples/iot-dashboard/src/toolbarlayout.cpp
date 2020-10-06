/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 * 
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "toolbarlayout.h"

#include <unordered_map>
#include <cmath>

#include <QQmlComponent>
#include <QTimer>
#include <QDeadlineTimer>

#include "enums.h"
#include "toolbarlayoutdelegate.h"

ToolBarLayoutAttached::ToolBarLayoutAttached(QObject *parent)
    : QObject(parent)
{
}

QObject *ToolBarLayoutAttached::action() const
{
    return m_action;
}

void ToolBarLayoutAttached::setAction(QObject *action)
{
    m_action = action;
}

class ToolBarLayout::Private
{
public:
    Private(ToolBarLayout *qq) : q(qq) { }

    void performLayout();
    QVector<ToolBarLayoutDelegate*> createDelegates();
    ToolBarLayoutDelegate *createDelegate(QObject *action);
    qreal layoutStart(qreal layoutWidth);
    void maybeHideDelegate(int index, qreal &currentWidth, qreal totalWidth);

    ToolBarLayout *q;

    QVector<QObject*> actions;
    ActionsProperty actionsProperty;
    QList<QObject*> hiddenActions;
    QQmlComponent *fullDelegate = nullptr;
    QQmlComponent *iconDelegate = nullptr;
    QQmlComponent *moreButton = nullptr;
    qreal spacing = 0.0;
    Qt::Alignment alignment = Qt::AlignLeft;
    qreal visibleWidth = 0.0;
    Qt::LayoutDirection layoutDirection = Qt::LeftToRight;
    HeightMode heightMode = ConstrainIfLarger;

    bool completed = false;
    bool layoutQueued = false;
    bool actionsChanged = false;
    std::unordered_map<QObject*, std::unique_ptr<ToolBarLayoutDelegate>> delegates;
    QVector<ToolBarLayoutDelegate*> sortedDelegates;
    QQuickItem *moreButtonInstance = nullptr;
    ToolBarDelegateIncubator *moreButtonIncubator = nullptr;
    int firstHiddenIndex = -1;

    QVector<QObject*> removedActions;
    QTimer *removalTimer = nullptr;

    QElapsedTimer performanceTimer;

    static void appendAction(ToolBarLayout::ActionsProperty *list, QObject *action);
    static int actionCount(ToolBarLayout::ActionsProperty *list);
    static QObject *action(ToolBarLayout::ActionsProperty *list, int index);
    static void clearActions(ToolBarLayout::ActionsProperty *list);
};

ToolBarLayout::ToolBarLayout(QQuickItem *parent)
    : QQuickItem(parent)
    , d(new Private{this})
{
    d->actionsProperty = ActionsProperty(this, this, Private::appendAction, Private::actionCount, Private::action, Private::clearActions);

    // To prevent multiple assignments to actions from constantly recreating
    // delegates, we cache the delegates and only remove them once they are no
    // longer being used. This timer is responsible for triggering that removal.
    d->removalTimer = new QTimer{this};
    d->removalTimer->setInterval(1000);
    d->removalTimer->setSingleShot(true);
    connect(d->removalTimer, &QTimer::timeout, this, [this]() {
        for (auto action : qAsConst(d->removedActions)) {
            if (!d->actions.contains(action)) {
                d->delegates.erase(action);
            }
        }
        d->removedActions.clear();
    });
}

ToolBarLayout::~ToolBarLayout()
{
}

ToolBarLayout::ActionsProperty ToolBarLayout::actionsProperty() const
{
    return d->actionsProperty;
}

void ToolBarLayout::addAction(QObject* action)
{
    d->actions.append(action);
    d->actionsChanged = true;

    connect(action, &QObject::destroyed, this, [this](QObject *action) {
        auto itr = d->delegates.find(action);
        if (itr != d->delegates.end()) {
            d->delegates.erase(itr);
        }

        d->actions.removeOne(action);
        d->actionsChanged = true;

        relayout();
    });

    relayout();
}

void ToolBarLayout::removeAction(QObject* action)
{
    auto itr = d->delegates.find(action);
    if (itr != d->delegates.end()) {
        itr->second->hide();
    }

    d->actions.removeOne(action);
    d->removedActions.append(action);
    d->removalTimer->start();
    d->actionsChanged = true;

    relayout();
}

void ToolBarLayout::clearActions()
{
    for (auto action : qAsConst(d->actions)) {
        auto itr = d->delegates.find(action);
        if (itr != d->delegates.end()) {
            itr->second->hide();
        }
    }

    d->removedActions.append(d->actions);
    d->actions.clear();
    d->actionsChanged = true;

    relayout();
}

QList<QObject*> ToolBarLayout::hiddenActions() const
{
    return d->hiddenActions;
}

QQmlComponent * ToolBarLayout::fullDelegate() const
{
    return d->fullDelegate;
}

void ToolBarLayout::setFullDelegate(QQmlComponent *newFullDelegate)
{
    if (newFullDelegate == d->fullDelegate) {
        return;
    }

    d->fullDelegate = newFullDelegate;
    d->delegates.clear();
    relayout();
    Q_EMIT fullDelegateChanged();
}

QQmlComponent * ToolBarLayout::iconDelegate() const
{
    return d->iconDelegate;
}

void ToolBarLayout::setIconDelegate(QQmlComponent *newIconDelegate)
{
    if (newIconDelegate == d->iconDelegate) {
        return;
    }

    d->iconDelegate = newIconDelegate;
    d->delegates.clear();
    relayout();
    Q_EMIT iconDelegateChanged();
}


QQmlComponent *ToolBarLayout::moreButton() const
{
    return d->moreButton;
}

void ToolBarLayout::setMoreButton(QQmlComponent *newMoreButton)
{
    if (newMoreButton == d->moreButton) {
        return;
    }

    d->moreButton = newMoreButton;
    if (d->moreButtonInstance) {
        d->moreButtonInstance->deleteLater();
        d->moreButtonInstance = nullptr;
    }
    relayout();
    Q_EMIT moreButtonChanged();
}

qreal ToolBarLayout::spacing() const
{
    return d->spacing;
}

void ToolBarLayout::setSpacing(qreal newSpacing)
{
    if (newSpacing == d->spacing) {
        return;
    }

    d->spacing = newSpacing;
    relayout();
    Q_EMIT spacingChanged();
}

Qt::Alignment ToolBarLayout::alignment() const
{
    return d->alignment;
}

void ToolBarLayout::setAlignment(Qt::Alignment newAlignment)
{
    if (newAlignment == d->alignment) {
        return;
    }

    d->alignment = newAlignment;
    relayout();
    Q_EMIT alignmentChanged();
}

qreal ToolBarLayout::visibleWidth() const
{
    return d->visibleWidth;
}

qreal ToolBarLayout::minimumWidth() const
{
    return d->moreButtonInstance ? d->moreButtonInstance->width() : 0;
}

Qt::LayoutDirection ToolBarLayout::layoutDirection() const
{
    return d->layoutDirection;
}

void ToolBarLayout::setLayoutDirection(Qt::LayoutDirection &newLayoutDirection)
{
    if (newLayoutDirection == d->layoutDirection) {
        return;
    }

    d->layoutDirection = newLayoutDirection;
    relayout();
    Q_EMIT layoutDirectionChanged();
}

ToolBarLayout::HeightMode ToolBarLayout::heightMode() const
{
    return d->heightMode;
}

void ToolBarLayout::setHeightMode(HeightMode newHeightMode)
{
    if (newHeightMode == d->heightMode) {
        return;
    }

    d->heightMode = newHeightMode;
    relayout();
    Q_EMIT heightModeChanged();
}

void ToolBarLayout::relayout()
{
    if (d->completed) {
        polish();
    }
}

void ToolBarLayout::componentComplete()
{
    QQuickItem::componentComplete();
    d->completed = true;
    relayout();
}

void ToolBarLayout::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (newGeometry != oldGeometry) {
        relayout();
    }
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

void ToolBarLayout::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData& data)
{
    if (change == ItemVisibleHasChanged || change == ItemSceneChange) {
        relayout();
    }
    QQuickItem::itemChange(change, data);
}

void ToolBarLayout::updatePolish()
{
    d->performLayout();
}

void ToolBarLayout::Private::performLayout()
{
    if (!fullDelegate || !iconDelegate || !moreButton) {
        qWarning() << "ToolBarLayout: Unable to layout, required properties are not set";
        return;
    }

    if (actions.isEmpty()) {
        q->setImplicitWidth(0);
        q->setImplicitHeight(0);
        return;
    }

    hiddenActions.clear();
    firstHiddenIndex = -1;

    sortedDelegates = createDelegates();

    bool ready = std::all_of(delegates.cbegin(), delegates.cend(), [](const std::pair<QObject* const, std::unique_ptr<ToolBarLayoutDelegate>> &entry) {
        return entry.second->isReady();
    });
    if (!ready || !moreButtonInstance) {
        return;
    }

    qreal maxHeight = moreButtonInstance->isVisible() ? moreButtonInstance->height() : 0.0;
    qreal maxWidth = 0.0;

    // First, calculate the total width and maximum height of all delegates.
    // This will be used to determine which actions to show, which ones to
    // collapse to icon-only etc.
    for (auto entry : qAsConst(sortedDelegates)) {
        if (!entry->isActionVisible()) {
            entry->hide();
            continue;
        }

        if (entry->isHidden()) {
            entry->hide();
            hiddenActions.append(entry->action());
            continue;
        }

        if (entry->isIconOnly()) {
            entry->showIcon();
        } else {
            entry->showFull();
        }

        maxWidth += entry->width() + spacing;
        maxHeight = std::max(maxHeight, entry->maxHeight());
    }

    // The last entry also gets spacing but shouldn't, so remove that.
    maxWidth -= spacing;

    if (q->heightValid() && q->height() > 0.0) {
        maxHeight = q->height();
    }

    qreal visibleActionsWidth = 0.0;

    if (maxWidth > q->width() - (hiddenActions.isEmpty() ? 0.0 : moreButtonInstance->width() + spacing)) {
        // We have more items than fit into the view, so start hiding some.

        qreal layoutWidth = q->width() - (moreButtonInstance->width() + spacing);
        if (alignment & Qt::AlignHCenter) {
            // When centering, we need to reserve space on both sides to make sure
            // things are properly centered, otherwise we will be to the right of
            // the center.
            layoutWidth -= (moreButtonInstance->width() + spacing);
        }

        for (int i = 0; i < sortedDelegates.size(); ++i) {
            auto delegate = sortedDelegates.at(i);

            maybeHideDelegate(i, visibleActionsWidth, layoutWidth);

            if (delegate->isVisible()) {
                visibleActionsWidth += delegate->width() + spacing;
            }
        }
        if (!qFuzzyIsNull(visibleActionsWidth)) {
            // Like above, remove spacing on the last element that incorrectly gets spacing added.
            visibleActionsWidth -= spacing;
        }
    } else {
        visibleActionsWidth = maxWidth;
    }

    if (!hiddenActions.isEmpty()) {
        if (layoutDirection == Qt::LeftToRight) {
            moreButtonInstance->setX(q->width() - moreButtonInstance->width());
        } else {
            moreButtonInstance->setX(0.0);
        }

        if (heightMode == AlwaysFill || (heightMode == ConstrainIfLarger && moreButtonInstance->height() > maxHeight)) {
            moreButtonInstance->setHeight(q->height());
        }

        moreButtonInstance->setY(qRound((maxHeight - moreButtonInstance->height()) / 2.0));
        moreButtonInstance->setVisible(true);
    } else {
        moreButtonInstance->setVisible(false);
    }

    qreal currentX = layoutStart(visibleActionsWidth);
    for (auto entry : qAsConst(sortedDelegates)) {
        if (!entry->isVisible()) {
            continue;
        }

        if (heightMode == AlwaysFill || (heightMode == ConstrainIfLarger && entry->height() > maxHeight)) {
            entry->setHeight(q->height());
        }

        qreal y = qRound((maxHeight - entry->height()) / 2.0);

        if (layoutDirection == Qt::LeftToRight) {
            entry->setPosition(currentX, y);
            currentX += entry->width() + spacing;
        } else {
            entry->setPosition(currentX - entry->width(), y);
            currentX -= entry->width() + spacing;
        }

        entry->show();
    }

    q->setImplicitSize(maxWidth, maxHeight);
    Q_EMIT q->hiddenActionsChanged();

    qreal newVisibleWidth = visibleActionsWidth;
    if (moreButtonInstance->isVisible()) {
        newVisibleWidth += moreButtonInstance->width() + (newVisibleWidth > 0.0 ? spacing : 0.0);
    }
    if (!qFuzzyCompare(newVisibleWidth, visibleWidth)) {
        visibleWidth = newVisibleWidth;
        Q_EMIT q->visibleWidthChanged();
    }

    if (actionsChanged) {
        // Due to the way QQmlListProperty works, if we emit changed every time
        // an action is added/removed, we end up emitting way too often. So
        // instead only do it after everything else is done.
        Q_EMIT q->actionsChanged();
        actionsChanged = false;
    }

    sortedDelegates.clear();
}

QVector<ToolBarLayoutDelegate*> ToolBarLayout::Private::createDelegates()
{
    QVector<ToolBarLayoutDelegate*> result;
    for (auto action : qAsConst(actions)) {
        if (delegates.find(action) != delegates.end()) {
            result.append(delegates.at(action).get());
        } else {
            auto delegate = std::unique_ptr<ToolBarLayoutDelegate>(createDelegate(action));
            if (delegate) {
                result.append(delegate.get());
                delegates.emplace(action, std::move(delegate));
            }
        }
    }

    if (!moreButtonInstance && !moreButtonIncubator) {
        moreButtonIncubator = new ToolBarDelegateIncubator(moreButton, qmlContext(moreButton));
        moreButtonIncubator->setStateCallback([this](QQuickItem *item) {
            item->setParentItem(q);
        });
        moreButtonIncubator->setCompletedCallback([this](ToolBarDelegateIncubator* incubator) {
            moreButtonInstance = qobject_cast<QQuickItem*>(incubator->object());
            moreButtonInstance->setVisible(false);

            connect(moreButtonInstance, &QQuickItem::widthChanged, q, [this]() {
                Q_EMIT q->minimumWidthChanged();
            });
            q->relayout();
            Q_EMIT q->minimumWidthChanged();

            QTimer::singleShot(0, q, [this]() {
                delete moreButtonIncubator;
                moreButtonIncubator = nullptr;
            });
        });
        moreButtonIncubator->create();
    }

    return result;
}

ToolBarLayoutDelegate *ToolBarLayout::Private::createDelegate(QObject* action)
{
    QQmlComponent *fullComponent = nullptr;
    auto displayComponent = action->property("displayComponent");
    if (displayComponent.isValid()) {
        fullComponent = displayComponent.value<QQmlComponent*>();
    }

    if (!fullComponent) {
        fullComponent = fullDelegate;
    }

    auto result = new ToolBarLayoutDelegate(q);
    result->setAction(action);
    result->createItems(fullComponent, iconDelegate, [this, action](QQuickItem *newItem) {
        newItem->setParentItem(q);
        auto attached = static_cast<ToolBarLayoutAttached*>(qmlAttachedPropertiesObject<ToolBarLayout>(newItem, true));
        attached->setAction(action);
    });

    return result;
}

qreal ToolBarLayout::Private::layoutStart(qreal layoutWidth)
{
    qreal availableWidth = moreButtonInstance->isVisible() ? q->width() - (moreButtonInstance->width() + spacing) : q->width();

    if (alignment & Qt::AlignLeft) {
        return layoutDirection == Qt::LeftToRight ? 0.0 : q->width();
    } else if (alignment & Qt::AlignHCenter) {
        return (q->width() / 2) + (layoutDirection == Qt::LeftToRight ? -layoutWidth / 2.0 : layoutWidth / 2.0);
    } else if (alignment & Qt::AlignRight) {
        qreal offset = availableWidth - layoutWidth;
        return layoutDirection == Qt::LeftToRight ? offset : q->width() - offset;
    }
    return 0.0;
}

void ToolBarLayout::Private::maybeHideDelegate(int index, qreal &currentWidth, qreal totalWidth)
{
    auto delegate = sortedDelegates.at(index);

    if (!delegate->isVisible()) {
        // If the delegate isn't visible anyway, do nothing.
        return;
    }

    if (currentWidth + delegate->width() < totalWidth && (firstHiddenIndex < 0 || index < firstHiddenIndex)) {
        // If the delegate is fully visible and we have not already hidden
        // actions, do nothing.
        return;
    }

    if (delegate->isKeepVisible()) {
        // If the action is marked as KeepVisible, we need to try our best to
        // keep it in view. If the full size delegate does not fit, we try the
        // icon-only delegate. If that also does not fit, try and find other
        // actions to hide. Finally, if that also fails, we will hide the
        // delegate.
        if (currentWidth + delegate->iconWidth() > totalWidth) {
            // First, hide any earlier actions that are not marked as KeepVisible.
            for (auto currentIndex = index - 1; currentIndex >= 0; --currentIndex) {
                auto previousDelegate = sortedDelegates.at(currentIndex);
                if (!previousDelegate->isVisible() || previousDelegate->isKeepVisible()) {
                    continue;
                }

                auto width = previousDelegate->width();
                previousDelegate->hide();
                hiddenActions.append(previousDelegate->action());
                currentWidth -= (width + spacing);

                if (currentWidth + delegate->fullWidth() <= totalWidth) {
                    delegate->showFull();
                    break;
                } else if (currentWidth + delegate->iconWidth() <= totalWidth) {
                    delegate->showIcon();
                    break;
                }
            }

            if (currentWidth + delegate->width() <= totalWidth) {
                return;
            }

            // Hiding normal actions did not help enough, so go through actions
            // with KeepVisible set and try and collapse them to IconOnly.
            for (auto currentIndex = index - 1; currentIndex >= 0; --currentIndex) {
                auto previousDelegate = sortedDelegates.at(currentIndex);
                if (!previousDelegate->isVisible() || !previousDelegate->isKeepVisible()) {
                    continue;
                }

                auto extraSpace = previousDelegate->width() - previousDelegate->iconWidth();
                previousDelegate->showIcon();
                currentWidth -= extraSpace;

                if (currentWidth + delegate->fullWidth() <= totalWidth) {
                    delegate->showFull();
                    break;
                } else if (currentWidth + delegate->iconWidth() <= totalWidth) {
                    delegate->showIcon();
                    break;
                }
            }

            // If that also did not work, then hide this action after all.
            if (currentWidth + delegate->width() > totalWidth) {
                delegate->hide();
                hiddenActions.append(delegate->action());
            }
        } else {
            delegate->showIcon();
        }
    } else {
        // The action is not marked as KeepVisible and it does not fit within
        // the current layout, so hide it.
        delegate->hide();
        hiddenActions.append(delegate->action());

        // If this is the first item to be hidden, mark it so we know we should
        // also hide the following items.
        if (firstHiddenIndex < 0) {
            firstHiddenIndex = index;
        }
    }
}

void ToolBarLayout::Private::appendAction(ToolBarLayout::ActionsProperty *list, QObject *action)
{
    auto layout = reinterpret_cast<ToolBarLayout*>(list->data);
    layout->addAction(action);
}

int ToolBarLayout::Private::actionCount(ToolBarLayout::ActionsProperty *list)
{
    return reinterpret_cast<ToolBarLayout*>(list->data)->d->actions.count();
}

QObject *ToolBarLayout::Private::action(ToolBarLayout::ActionsProperty *list, int index)
{
    return reinterpret_cast<ToolBarLayout *>(list->data)->d->actions.at(index);
}

void ToolBarLayout::Private::clearActions(ToolBarLayout::ActionsProperty *list)
{
    reinterpret_cast<ToolBarLayout *>(list->data)->clearActions();
}
