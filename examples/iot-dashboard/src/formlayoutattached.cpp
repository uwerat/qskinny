/*
 *  SPDX-FileCopyrightText: 2017 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "formlayoutattached.h"
#include <QQuickItem>
#include <QDebug>

FormLayoutAttached::FormLayoutAttached(QObject *parent)
    : QObject(parent)
{
    m_buddyFor = qobject_cast<QQuickItem *>(parent);
}

FormLayoutAttached::~FormLayoutAttached()
{
}

void FormLayoutAttached::setLabel(const QString &text)
{
    if (m_label == text) {
        return;
    }

    m_label = text;
    emit labelChanged();
}

QString FormLayoutAttached::label() const
{
    return m_label;
}

void FormLayoutAttached::setIsSection(bool section)
{
    if (m_isSection == section) {
        return;
    }

    m_isSection = section;
    emit isSectionChanged();
}

bool FormLayoutAttached::isSection() const
{
    return m_isSection;
}

void FormLayoutAttached::setCheckable(bool checkable)
{
    if (checkable == m_checkable) {
        return;
    }

    m_checkable = checkable;
    emit checkableChanged();
}

bool FormLayoutAttached::checkable() const
{
    return m_checkable;
}

void FormLayoutAttached::setChecked(bool checked)
{
    if (checked == m_checked) {
        return;
    }

    m_checked = checked;
    emit checkedChanged();
}

bool FormLayoutAttached::checked() const
{
    return m_checked;
}

void FormLayoutAttached::setEnabled(bool enabled)
{
    if (enabled == m_enabled) {
        return;
    }

    m_enabled = enabled;
    emit enabledChanged();
}

bool FormLayoutAttached::enabled() const
{
    return m_enabled;
}

QQuickItem *FormLayoutAttached::buddyFor() const
{
    return m_buddyFor;
}

void FormLayoutAttached::setBuddyFor(QQuickItem *buddyfor)
{
    if (m_buddyFor == buddyfor || !m_buddyFor->isAncestorOf(buddyfor)) {
        return;
    }

    m_buddyFor = buddyfor;
    emit buddyForChanged();
}

FormLayoutAttached *FormLayoutAttached::qmlAttachedProperties(QObject *object)
{
    return new FormLayoutAttached(object);
}

#include "moc_formlayoutattached.cpp"
