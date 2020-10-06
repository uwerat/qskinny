/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 * 
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "toolbarlayoutdelegate.h"

#include <QQmlIncubator>

#include "toolbarlayout.h"

ToolBarDelegateIncubator::ToolBarDelegateIncubator(QQmlComponent* component, QQmlContext *context)
    : QQmlIncubator(QQmlIncubator::Asynchronous)
    , m_component(component)
    , m_context(context)
{
}

void ToolBarDelegateIncubator::setStateCallback(std::function<void (QQuickItem *)> callback)
{
    m_stateCallback = callback;
}

void ToolBarDelegateIncubator::setCompletedCallback(std::function<void (ToolBarDelegateIncubator *)> callback)
{
    m_completedCallback = callback;
}

void ToolBarDelegateIncubator::create()
{
    m_component->create(*this, m_context);
}

bool ToolBarDelegateIncubator::isFinished()
{
    return m_finished;
}

void ToolBarDelegateIncubator::setInitialState(QObject* object)
{
    auto item = qobject_cast<QQuickItem*>(object);
    if (item) {
        m_stateCallback(item);
    }
}

void ToolBarDelegateIncubator::statusChanged(QQmlIncubator::Status status)
{
    if (status == QQmlIncubator::Error) {
        qWarning() << "Could not create delegate for ToolBarLayout";
        const auto e = errors();
        for (const auto &error : e) {
            qWarning() << error;
        }
        m_finished = true;
    }

    if (status == QQmlIncubator::Ready) {
        m_completedCallback(this);
        m_finished = true;
    }
}

ToolBarLayoutDelegate::ToolBarLayoutDelegate(ToolBarLayout* parent)
    : QObject() // Note: delegates are managed by unique_ptr, so don't parent
    , m_parent(parent)
{
}

ToolBarLayoutDelegate::~ToolBarLayoutDelegate()
{
    if (m_fullIncubator) {
        m_fullIncubator->clear();
        delete m_fullIncubator;
    }
    if (m_iconIncubator) {
        m_iconIncubator->clear();
        delete m_iconIncubator;
    }
    if (m_full) {
        delete m_full;
    }
    if (m_icon) {
        delete m_icon;
    }
}

QObject * ToolBarLayoutDelegate::action() const
{
    return m_action;
}

void ToolBarLayoutDelegate::setAction(QObject* action)
{
    if (action == m_action) {
        return;
    }

    if (m_action) {
        QObject::disconnect(m_action, SIGNAL(visibleChanged()), this, SLOT(actionVisibleChanged()));
        QObject::disconnect(m_action, SIGNAL(displayHintChanged()), this, SLOT(displayHintChanged()));
    }

    m_action = action;
    if (m_action) {
        if (m_action->property("visible").isValid()) {
            QObject::connect(m_action, SIGNAL(visibleChanged()), this, SLOT(actionVisibleChanged()));
            m_actionVisible = m_action->property("visible").toBool();
        }

        if (m_action->property("displayHint").isValid()) {
            QObject::connect(m_action, SIGNAL(displayHintChanged()), this, SLOT(displayHintChanged()));
            m_displayHint = DisplayHint::DisplayHints{m_action->property("displayHint").toInt()};
        }
    }
}

void ToolBarLayoutDelegate::createItems(QQmlComponent *fullComponent, QQmlComponent *iconComponent, std::function<void(QQuickItem*)> callback)
{
    m_fullIncubator = new ToolBarDelegateIncubator(fullComponent, qmlContext(fullComponent));
    m_fullIncubator->setStateCallback(callback);
    m_fullIncubator->setCompletedCallback([this](ToolBarDelegateIncubator *incubator) {
        if (incubator->isError()) {
            qWarning() << "Could not create delegate for ToolBarLayout";
            const auto errors = incubator->errors();
            for (const auto &error : errors) {
                qWarning() << error;
            }
            return;
        }

        m_full = qobject_cast<QQuickItem*>(incubator->object());
        m_full->setVisible(false);
        connect(m_full, &QQuickItem::widthChanged, this, [this]() { m_parent->relayout(); });
        connect(m_full, &QQuickItem::heightChanged, this, [this]() { m_parent->relayout(); });

        if (m_icon) {
            m_ready = true;
        }

        m_parent->relayout();

        QMetaObject::invokeMethod(this, &ToolBarLayoutDelegate::cleanupIncubators, Qt::QueuedConnection);
    });
    m_iconIncubator = new ToolBarDelegateIncubator(iconComponent, qmlContext(iconComponent));
    m_iconIncubator->setStateCallback(callback);
    m_iconIncubator->setCompletedCallback([this](ToolBarDelegateIncubator *incubator) {
        if (incubator->isError()) {
            qWarning() << "Could not create delegate for ToolBarLayout";
            const auto errors = incubator->errors();
            for (const auto &error : errors) {
                qWarning() << error;
            }
            return;
        }

        m_icon = qobject_cast<QQuickItem*>(incubator->object());
        m_icon->setVisible(false);
        connect(m_icon, &QQuickItem::widthChanged, this, [this]() { m_parent->relayout(); });
        connect(m_icon, &QQuickItem::heightChanged, this, [this]() { m_parent->relayout(); });

        if (m_full) {
            m_ready = true;
        }

        m_parent->relayout();

        QMetaObject::invokeMethod(this, &ToolBarLayoutDelegate::cleanupIncubators, Qt::QueuedConnection);
    });

    m_fullIncubator->create();
    m_iconIncubator->create();
}

bool ToolBarLayoutDelegate::isReady() const
{
    return m_ready;
}

bool ToolBarLayoutDelegate::isActionVisible() const
{
    return m_actionVisible;
}

bool ToolBarLayoutDelegate::isHidden() const
{
    return DisplayHint::isDisplayHintSet(m_displayHint, DisplayHint::AlwaysHide);
}

bool ToolBarLayoutDelegate::isIconOnly() const
{
    return DisplayHint::isDisplayHintSet(m_displayHint, DisplayHint::IconOnly);
}

bool ToolBarLayoutDelegate::isKeepVisible() const
{
    return DisplayHint::isDisplayHintSet(m_displayHint, DisplayHint::KeepVisible);
}

bool ToolBarLayoutDelegate::isVisible() const
{
    return m_iconVisible || m_fullVisible;
}

void ToolBarLayoutDelegate::hide()
{
    m_iconVisible = false;
    m_fullVisible = false;
    ensureItemVisibility();
}

void ToolBarLayoutDelegate::showFull()
{
    m_iconVisible = false;
    m_fullVisible = true;
}

void ToolBarLayoutDelegate::showIcon()
{
    m_iconVisible = true;
    m_fullVisible = false;
}

void ToolBarLayoutDelegate::show()
{
    ensureItemVisibility();
}

void ToolBarLayoutDelegate::setPosition(qreal x, qreal y)
{
    m_full->setX(x);
    m_icon->setX(x);
    m_full->setY(y);
    m_icon->setY(y);
}

void ToolBarLayoutDelegate::setHeight(qreal height)
{
    m_full->setHeight(height);
    m_icon->setHeight(height);
}

qreal ToolBarLayoutDelegate::width() const
{
    if (m_iconVisible) {
        return m_icon->width();
    }
    return m_full->width();
}

qreal ToolBarLayoutDelegate::height() const
{
    if (m_iconVisible) {
        return m_icon->height();
    }
    return m_full->height();
}

qreal ToolBarLayoutDelegate::maxHeight() const
{
    return std::max(m_full->height(), m_icon->height());
}

qreal ToolBarLayoutDelegate::iconWidth() const
{
    return m_icon->width();
}

qreal ToolBarLayoutDelegate::fullWidth() const
{
    return m_full->width();
}

void ToolBarLayoutDelegate::actionVisibleChanged()
{
    m_actionVisible = m_action->property("visible").toBool();
    m_parent->relayout();
}

void ToolBarLayoutDelegate::displayHintChanged()
{
    m_displayHint = DisplayHint::DisplayHints{m_action->property("displayHint").toInt()};
    m_parent->relayout();
}

void ToolBarLayoutDelegate::cleanupIncubators()
{
    if (m_fullIncubator && m_fullIncubator->isFinished()) {
        delete m_fullIncubator;
        m_fullIncubator = nullptr;
    }

    if (m_iconIncubator && m_iconIncubator->isFinished()) {
        delete m_iconIncubator;
        m_iconIncubator = nullptr;
    }
}
