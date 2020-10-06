/*
 *  SPDX-FileCopyrightText: 2011 Marco Martin <mart@kde.org>
 *  SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "delegaterecycler.h"

#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QDebug>

DelegateRecyclerAttached::DelegateRecyclerAttached(QObject *parent)
    : QObject(parent)
{
}

DelegateRecyclerAttached::~DelegateRecyclerAttached()
{}
/*
void setRecycler(DelegateRecycler *recycler)
{
    m_recycler = recycler;
}

DelegateRecycler *recycler() const
{
    return m_recycler;
}
*/



class DelegateCache
{
public:
    DelegateCache();
    ~DelegateCache();

    void ref(QQmlComponent *);
    void deref(QQmlComponent *);

    void insert(QQmlComponent *, QQuickItem *);
    QQuickItem *take(QQmlComponent *);

private:
    static const int s_cacheSize = 40;
    QHash<QQmlComponent *, int> m_refs;
    QHash<QQmlComponent *, QList<QQuickItem *> > m_unusedItems;
};

Q_GLOBAL_STATIC(DelegateCache, s_delegateCache)

DelegateCache::DelegateCache()
{
}

DelegateCache::~DelegateCache()
{
    for (auto& item : qAsConst(m_unusedItems)) {
        qDeleteAll(item);
    }
}

void DelegateCache::ref(QQmlComponent *component)
{
    m_refs[component]++;
}

void DelegateCache::deref(QQmlComponent *component)
{
    auto itRef = m_refs.find(component);
    if (itRef == m_refs.end()) {
        return;
    }

    (*itRef)--;
    if (*itRef <= 0) {
        m_refs.erase(itRef);

        qDeleteAll(m_unusedItems.take(component));
    }
}

void DelegateCache::insert(QQmlComponent *component, QQuickItem *item)
{
    auto& items = m_unusedItems[component];
    if (items.length() >= s_cacheSize) {
        item->deleteLater();
        return;
    }

    DelegateRecyclerAttached *attached = qobject_cast<DelegateRecyclerAttached *>(qmlAttachedPropertiesObject<DelegateRecycler>(item, false));
    if (attached) {
        emit attached->pooled();
    }

    item->setParentItem(nullptr);
    items.append(item);
}

QQuickItem *DelegateCache::take(QQmlComponent *component)
{
    auto it = m_unusedItems.find(component);
    if (it != m_unusedItems.end() && !it->isEmpty()) {
        return it->takeFirst();
    }
    return nullptr;
}





DelegateRecycler::DelegateRecycler(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlags(QQuickItem::ItemIsFocusScope);
}

DelegateRecycler::~DelegateRecycler()
{
    if (m_sourceComponent) {
        s_delegateCache->insert(m_sourceComponent, m_item);
        s_delegateCache->deref(m_sourceComponent);
    }
}

void DelegateRecycler::syncIndex()
{
    const QVariant newIndex = m_propertiesTracker->property("trackedIndex");
    if (!newIndex.isValid()) {
        return;
    }
    QQmlContext *ctx = QQmlEngine::contextForObject(m_item)->parentContext();
    ctx->setContextProperty(QStringLiteral("index"), newIndex);
}

void DelegateRecycler::syncModel()
{
    const QVariant newModel = m_propertiesTracker->property("trackedModel");
    if (!newModel.isValid()) {
        return;
    }
    QQmlContext *ctx = QQmlEngine::contextForObject(m_item)->parentContext();
    ctx->setContextProperty(QStringLiteral("model"), newModel);

    //try to bind all properties
    QObject *modelObj = newModel.value<QObject *>();
    if (modelObj) {
        const QMetaObject *metaObj = modelObj->metaObject();
        for (int i = metaObj->propertyOffset(); i < metaObj->propertyCount(); ++i) {
            ctx->setContextProperty(QString::fromUtf8(metaObj->property(i).name()), metaObj->property(i).read(modelObj));
        }
    }
}

void DelegateRecycler::syncModelProperties()
{
    const QVariant model = m_propertiesTracker->property("trackedModel");
    if (!model.isValid()) {
        return;
    }
    QQmlContext *ctx = QQmlEngine::contextForObject(m_item)->parentContext();

    //try to bind all properties
    QObject *modelObj = model.value<QObject *>();
    if (modelObj) {
        const QMetaObject *metaObj = modelObj->metaObject();
        for (int i = metaObj->propertyOffset(); i < metaObj->propertyCount(); ++i) {
            ctx->setContextProperty(QString::fromUtf8(metaObj->property(i).name()), metaObj->property(i).read(modelObj));
        }
    }
}

void DelegateRecycler::syncModelData()
{
    const QVariant newModelData = m_propertiesTracker->property("trackedModelData");
    if (!newModelData.isValid()) {
        return;
    }
    QQmlContext *ctx = QQmlEngine::contextForObject(m_item)->parentContext();
    ctx->setContextProperty(QStringLiteral("modelData"), newModelData);
}

QQmlComponent *DelegateRecycler::sourceComponent() const
{
    return m_sourceComponent;
}

void DelegateRecycler::setSourceComponent(QQmlComponent *component)
{
    if (component && component->parent() == this) {
        qWarning() << "Error: source components cannot be declared inside DelegateRecycler";
        return;
    }
    if (m_sourceComponent == component) {
        return;
    }

    if (!m_propertiesTracker) {
        static QHash<QQmlEngine*, QQmlComponent*> propertiesTrackerComponent;
        auto engine = qmlEngine(this);
        auto it = propertiesTrackerComponent.find(engine);
        if (it == propertiesTrackerComponent.end()) {
            connect(engine, &QObject::destroyed, engine, [engine] { propertiesTrackerComponent.remove(engine); });
            it = propertiesTrackerComponent.insert(engine, new QQmlComponent(engine, engine));

            (*it)->setData(QByteArrayLiteral("import QtQuick 2.3\nQtObject{property int trackedIndex: index; property var trackedModel: typeof model != 'undefined' ? model : null; property var trackedModelData: typeof modelData != 'undefined' ? modelData : null}"), QUrl(QStringLiteral("delegaterecycler.cpp")));
        }
        m_propertiesTracker = (*it)->create(QQmlEngine::contextForObject(this));

        connect(m_propertiesTracker, SIGNAL(trackedIndexChanged()), this, SLOT(syncIndex()));
        connect(m_propertiesTracker, SIGNAL(trackedModelChanged()), this, SLOT(syncModel()));
        connect(m_propertiesTracker, SIGNAL(trackedModelDataChanged()), this, SLOT(syncModelData()));
    }

    if (m_sourceComponent) {
        if (m_item) {
            disconnect(m_item.data(), &QQuickItem::implicitWidthChanged, this, &DelegateRecycler::updateHints);
            disconnect(m_item.data(), &QQuickItem::implicitHeightChanged, this, &DelegateRecycler::updateHints);
            s_delegateCache->insert(component, m_item);
        }
        s_delegateCache->deref(component);
    }

    m_sourceComponent = component;
    s_delegateCache->ref(component);

    m_item = s_delegateCache->take(component);

    if (!m_item) {
        QQuickItem *candidate = parentItem();
        QQmlContext *ctx = nullptr;
        while (candidate) {
            QQmlContext *parentCtx = QQmlEngine::contextForObject(candidate);
            if (parentCtx) {
                ctx = new QQmlContext(parentCtx, candidate);
                break;
            } else {
                candidate = candidate->parentItem();
            }
        }

        Q_ASSERT(ctx);

        QObject *contextObjectToSet = nullptr;
        {
            // Find the first parent that has a context object with a valid translationDomain property, i.e. is a KLocalizedContext
            QQmlContext *auxCtx = ctx;
            while (auxCtx != nullptr) {
                QObject *auxCtxObj = auxCtx->contextObject();
                if (auxCtxObj && auxCtxObj->property("translationDomain").isValid()) {
                    contextObjectToSet = auxCtxObj;
                    break;
                }
                auxCtx = auxCtx->parentContext();
            }
        }
        if (contextObjectToSet) {
            ctx->setContextObject(contextObjectToSet);
        }

        QObject *modelObj = m_propertiesTracker->property("trackedModel").value<QObject *>();
        if (modelObj) {
            const QMetaObject *metaObj = modelObj->metaObject();
            for (int i = metaObj->propertyOffset(); i < metaObj->propertyCount(); ++i) {
                QMetaProperty prop = metaObj->property(i);
                ctx->setContextProperty(QString::fromUtf8(prop.name()), prop.read(modelObj));
                if (prop.hasNotifySignal()) {
                    QMetaMethod updateSlot = metaObject()->method(
                    metaObject()->indexOfSlot("syncModelProperties()"));
                    connect(modelObj, prop.notifySignal(), this, updateSlot);
                }
            }
        }

        ctx->setContextProperty(QStringLiteral("model"), m_propertiesTracker->property("trackedModel"));
        ctx->setContextProperty(QStringLiteral("modelData"), m_propertiesTracker->property("trackedModelData"));
        ctx->setContextProperty(QStringLiteral("index"), m_propertiesTracker->property("trackedIndex"));
        ctx->setContextProperty(QStringLiteral("delegateRecycler"), this);

        QObject * obj = component->create(ctx);
        m_item = qobject_cast<QQuickItem *>(obj);
        if (!m_item) {
            obj->deleteLater();
        } else {
            connect(m_item.data(), &QObject::destroyed, ctx, &QObject::deleteLater);
            //if the user binded an explicit width, consider it, otherwise base upon implicit
            m_widthFromItem = m_item->width() > 0 && m_item->width() != m_item->implicitWidth();
            m_heightFromItem = m_item->height() > 0 && m_item->height() != m_item->implicitHeight();

            if (m_widthFromItem && m_heightFromItem) {
                connect(m_item.data(), &QQuickItem::heightChanged, this, [this]() {
                    updateSize(false);
                });
            }
        }
    } else {
        syncModel();

        QQmlContext *ctx = QQmlEngine::contextForObject(m_item)->parentContext();
        ctx->setContextProperties({ QQmlContext::PropertyPair{ QStringLiteral("modelData"), m_propertiesTracker->property("trackedModelData") },
                                    QQmlContext::PropertyPair{ QStringLiteral("index"), m_propertiesTracker->property("trackedIndex")},
                                    QQmlContext::PropertyPair{ QStringLiteral("delegateRecycler"), QVariant::fromValue<QObject*>(this) }
                                 });

        DelegateRecyclerAttached *attached = qobject_cast<DelegateRecyclerAttached *>(qmlAttachedPropertiesObject<DelegateRecycler>(m_item, false));
        if (attached) {
            emit attached->reused();
        }
    }

    if (m_item) {
        m_item->setParentItem(this);
        connect(m_item.data(), &QQuickItem::implicitWidthChanged, this, &DelegateRecycler::updateHints);
        connect(m_item.data(), &QQuickItem::implicitHeightChanged, this, &DelegateRecycler::updateHints);

        updateSize(true);
    }

    emit sourceComponentChanged();
}

void DelegateRecycler::resetSourceComponent()
{
    s_delegateCache->deref(m_sourceComponent);
    m_sourceComponent = nullptr;
}

DelegateRecyclerAttached *DelegateRecycler::qmlAttachedProperties(QObject *object)
{
    return new DelegateRecyclerAttached(object);
}

void DelegateRecycler::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (m_item && newGeometry.size() != oldGeometry.size()) {
        updateSize(true);
    }
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

void DelegateRecycler::focusInEvent(QFocusEvent *event)
{
    QQuickItem::focusInEvent(event);
    if (!m_item) {
        return;
    }

    m_item->setFocus(event->reason());
}

void DelegateRecycler::updateHints()
{
    updateSize(false);
}

void DelegateRecycler::updateSize(bool parentResized)
{
    if (!m_item) {
        return;
    }

    const bool needToUpdateWidth = !m_widthFromItem && parentResized && widthValid();
    const bool needToUpdateHeight = !m_heightFromItem && parentResized && heightValid();

    if (parentResized) {
        m_item->setPosition(QPoint(0,0));
    }
    if (needToUpdateWidth && needToUpdateHeight) {
        m_item->setSize(QSizeF(width(), height()));
    } else if (needToUpdateWidth) {
        m_item->setWidth(width());
    } else if (needToUpdateHeight) {
        m_item->setHeight(height());
    }

    if (m_updatingSize) {
        return;
    }

    m_updatingSize = true;

    if (m_heightFromItem) {
        setHeight(m_item->height());
    }
    if (m_widthFromItem) {
        setWidth(m_item->width());
    }

    setImplicitSize(m_item->implicitWidth() >= 0 ? m_item->implicitWidth() : m_item->width(),
                m_item->implicitHeight() >= 0 ? m_item->implicitHeight() : m_item->height());


    m_updatingSize = false;
}
