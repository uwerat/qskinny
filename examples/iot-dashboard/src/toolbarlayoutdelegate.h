/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 * 
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef TOOLBARLAYOUTDELEGATE_H
#define TOOLBARLAYOUTDELEGATE_H

#include <QQuickItem>
#include <QQmlIncubator>
#include "enums.h"

class ToolBarLayout;

/*
 * A helper subclass of QQmlIncubator that allows us to do some things more
 * easily.
 */
class ToolBarDelegateIncubator : public QQmlIncubator
{
public:
    ToolBarDelegateIncubator(QQmlComponent *component, QQmlContext *context);

    void setStateCallback(std::function<void(QQuickItem*)> callback);
    void setCompletedCallback(std::function<void(ToolBarDelegateIncubator*)> callback);

    void create();

    bool isFinished();

private:
    void setInitialState(QObject *object) override;
    void statusChanged(QQmlIncubator::Status status) override;

    QQmlComponent *m_component;
    QQmlContext *m_context;
    std::function<void(QQuickItem*)> m_stateCallback;
    std::function<void(ToolBarDelegateIncubator*)> m_completedCallback;
    bool m_finished = false;
};

/*
 * A helper class to encapsulate some of the delegate functionality used by
 * ToolBarLayout. Primarily, this hides some of the difference that delegates
 * are two items instead of one.
 */
class ToolBarLayoutDelegate : public QObject
{
    Q_OBJECT
public:
    ToolBarLayoutDelegate(ToolBarLayout *parent);
    ~ToolBarLayoutDelegate();

    QObject *action() const;
    void setAction(QObject *action);
    void createItems(QQmlComponent *fullComponent, QQmlComponent *iconComponent, std::function<void(QQuickItem*)> callback);

    bool isReady() const;
    bool isActionVisible() const;
    bool isHidden() const;
    bool isIconOnly() const;
    bool isKeepVisible() const;

    bool isVisible() const;

    void hide();
    void showIcon();
    void showFull();
    void show();

    void setPosition(qreal x, qreal y);
    void setHeight(qreal height);

    qreal width() const;
    qreal height() const;
    qreal maxHeight() const;
    qreal iconWidth() const;
    qreal fullWidth() const;

private:
    Q_SLOT void actionVisibleChanged();
    Q_SLOT void displayHintChanged();
    inline void ensureItemVisibility()
    {
        if (m_full) {
            m_full->setVisible(m_fullVisible);
        }
        if (m_icon) {
            m_icon->setVisible(m_iconVisible);
        }
    }
    void cleanupIncubators();

    ToolBarLayout *m_parent = nullptr;
    QObject *m_action = nullptr;
    QQuickItem *m_full = nullptr;
    QQuickItem *m_icon = nullptr;
    ToolBarDelegateIncubator *m_fullIncubator = nullptr;
    ToolBarDelegateIncubator *m_iconIncubator = nullptr;

    DisplayHint::DisplayHints m_displayHint = DisplayHint::NoPreference;
    bool m_ready = false;
    bool m_actionVisible = true;
    bool m_fullVisible = false;
    bool m_iconVisible = false;
};

#endif // TOOLBARLAYOUTDELEGATE_H
