/*
 *  SPDX-FileCopyrightText: 2019 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "pagepool.h"

#include <QDebug>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlProperty>

PagePool::PagePool(QObject *parent)
    : QObject(parent)
{
}

PagePool::~PagePool()
{
}

QUrl PagePool::lastLoadedUrl() const
{
    return m_lastLoadedUrl;
}

QQuickItem *PagePool::lastLoadedItem() const
{
    return m_lastLoadedItem;
}

void PagePool::setCachePages(bool cache)
{
    if (cache == m_cachePages) {
        return;
    }

    if (cache) {
        clear();
    }

    m_cachePages = cache;
    emit cachePagesChanged();
}

bool PagePool::cachePages() const
{
    return m_cachePages;
}

QQuickItem *PagePool::loadPage(const QString &url, QJSValue callback)
{
    return loadPageWithProperties(url, QVariantMap(), callback);
}

QQuickItem *PagePool::loadPageWithProperties(
        const QString &url, const QVariantMap &properties, QJSValue callback)
{
    Q_ASSERT(qmlEngine(this));
    QQmlContext *ctx = QQmlEngine::contextForObject(this);
    Q_ASSERT(ctx);

    const QUrl actualUrl = resolvedUrl(url);

    auto found = m_itemForUrl.find(actualUrl);
    if (found != m_itemForUrl.end()) {
        m_lastLoadedUrl = found.key();
        m_lastLoadedItem = found.value();

        if (callback.isCallable()) {
            QJSValueList args = {qmlEngine(this)->newQObject(found.value())};
            callback.call(args);
            emit lastLoadedUrlChanged();
            emit lastLoadedItemChanged();
            // We could return the item, but for api coherence return null
            return nullptr;

        } else {
            emit lastLoadedUrlChanged();
            emit lastLoadedItemChanged();
            return found.value();
        }
    }

    QQmlComponent *component = m_componentForUrl.value(actualUrl);

    if (!component) {
        component = new QQmlComponent(qmlEngine(this), actualUrl, QQmlComponent::PreferSynchronous);
    }

    if (component->status() == QQmlComponent::Loading) {
        if (!callback.isCallable()) {
            component->deleteLater();
            m_componentForUrl.remove(actualUrl);
            return nullptr;
        }

        connect(component, &QQmlComponent::statusChanged, this,
                [this, component, callback, properties] (QQmlComponent::Status status) mutable {
            if (status != QQmlComponent::Ready) {
                qWarning() << component->errors();
                m_componentForUrl.remove(component->url());
                component->deleteLater();
                return;
            }
            QQuickItem *item = createFromComponent(component, properties);
            if (item) {
                QJSValueList args = {qmlEngine(this)->newQObject(item)};
                callback.call(args);
            }

            if (m_cachePages) {
                component->deleteLater();
            } else {
                m_componentForUrl[component->url()] = component;
            }
        });

        return nullptr;

    } else if (component->status() != QQmlComponent::Ready) {
        qWarning() << component->errors();
        return nullptr;
    }

    QQuickItem *item = createFromComponent(component, properties);
    if (m_cachePages) {
        component->deleteLater();
    } else {
        m_componentForUrl[component->url()] = component;
    }

    if (callback.isCallable()) {
        QJSValueList args = {qmlEngine(this)->newQObject(item)};
        callback.call(args);
        m_lastLoadedUrl = actualUrl;
        emit lastLoadedUrlChanged();
        // We could return the item, but for api coherence return null
        return nullptr;
    } else {
        m_lastLoadedUrl = actualUrl;
        emit lastLoadedUrlChanged();
        return item;
    }
}

QQuickItem *PagePool::createFromComponent(QQmlComponent *component, const QVariantMap &properties)
{
    QQmlContext *ctx = QQmlEngine::contextForObject(this);
    Q_ASSERT(ctx);

    //TODO:  As soon as we can depend on Qt 5.14, use QQmlComponent::createWithInitialProperties
    QObject *obj = component->beginCreate(ctx);

    // Error?
    if (!obj) {
        return nullptr;
    }

    for (auto it = properties.constBegin(); it != properties.constEnd(); ++it) {

        QQmlProperty p(obj, it.key(), ctx);
        if (!p.isValid()) {
            qWarning() << "Invalid property " << it.key();
            continue;
        }
        if (!p.write(it.value())) {
            qWarning() << "Could not set property " << it.key();
            continue;
        }
    }

    component->completeCreate();

    QQuickItem *item = qobject_cast<QQuickItem *>(obj);
    if (!item) {
        obj->deleteLater();
        return nullptr;
    }

    // Always cache just the last one
    m_lastLoadedItem = item;

    if (m_cachePages) {
        QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        m_itemForUrl[component->url()] = item;
    } else {
        QQmlEngine::setObjectOwnership(item, QQmlEngine::JavaScriptOwnership);
    }

    emit lastLoadedItemChanged();

    return item;
}

QUrl PagePool::resolvedUrl(const QString &stringUrl) const
{
    Q_ASSERT(qmlEngine(this));
    QQmlContext *ctx = QQmlEngine::contextForObject(this);
    Q_ASSERT(ctx);

    QUrl actualUrl(stringUrl);
    if (actualUrl.scheme().isEmpty()) {
        actualUrl = ctx->resolvedUrl(actualUrl);
    }
    return actualUrl;
}

bool PagePool::isLocalUrl(const QUrl &url)
{
    return url.isLocalFile() || url.scheme().isEmpty() || url.scheme() == QStringLiteral("qrc");
}

QUrl PagePool::urlForPage(QQuickItem *item) const
{
    return m_urlForItem.value(item);
}

QQuickItem *PagePool::pageForUrl(const QUrl &url) const
{
    return m_itemForUrl.value(resolvedUrl(url.toString()), nullptr);
}

bool PagePool::contains(const QVariant &page) const
{
    if (page.canConvert<QQuickItem *>()) {
        return m_urlForItem.contains(page.value<QQuickItem *>());
    } else if (page.canConvert<QString>()) {
        const QUrl actualUrl = resolvedUrl(page.value<QString>());
        return m_itemForUrl.contains(actualUrl);
    } else {
        return false;
    }
}

void PagePool::deletePage(const QVariant &page)
{
    if (!contains(page)) {
        return;
    }

    QQuickItem *item;
    if (page.canConvert<QQuickItem *>()) {
        item = page.value<QQuickItem *>();
    } else if (page.canConvert<QString>()) {
        QString url = page.value<QString>();
        if (url.isEmpty()) {
            return;
        }
        const QUrl actualUrl = resolvedUrl(page.value<QString>());

        item = m_itemForUrl.value(actualUrl);
    } else {
        return;
    }

    if (!item) {
        return;
    }

    const QUrl url = m_urlForItem.value(item);

    if (url.isEmpty()) {
        return;
    }

    m_itemForUrl.remove(url);
    m_urlForItem.remove(item);
    item->deleteLater();
}

void PagePool::clear()
{
    for (auto *c : qAsConst(m_componentForUrl)) {
        c->deleteLater();
    }
    m_componentForUrl.clear();

    for (auto *i : qAsConst(m_itemForUrl)) {
        // items that had been deparented are safe to delete
        if (!i->parentItem()) {
            i->deleteLater();
        }
        QQmlEngine::setObjectOwnership(i, QQmlEngine::JavaScriptOwnership);
    }
    m_itemForUrl.clear();

    m_urlForItem.clear();
    m_lastLoadedUrl = QUrl();
    m_lastLoadedItem = nullptr;
    
    emit lastLoadedUrlChanged();
    emit lastLoadedItemChanged();
}

#include "moc_pagepool.cpp"
