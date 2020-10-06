/*
 *  SPDX-FileCopyrightText: 2011 Marco Martin <mart@kde.org>
 *  SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "icon.h"
#include "libkirigami/platformtheme.h"
#include "scenegraph/managedtexturenode.h"

#include <QSGSimpleTextureNode>
#include <QQuickWindow>
#include <QIcon>
#include <QBitmap>
#include <QSGTexture>
#include <QDebug>
#include <QSharedPointer>
#include <QtQml>
#include <QQuickImageProvider>
#include <QGuiApplication>
#include <QPointer>
#include <QPainter>
#include <QScreen>



Q_GLOBAL_STATIC(ImageTexturesCache, s_iconImageCache)

Icon::Icon(QQuickItem *parent)
    : QQuickItem(parent),
      m_changed(false),
      m_active(false),
      m_selected(false),
      m_isMask(false)
{
    setFlag(ItemHasContents, true);
    //FIXME: not necessary anymore
    connect(qApp, &QGuiApplication::paletteChanged, this, &QQuickItem::polish);
    connect(this, &QQuickItem::enabledChanged, this, &QQuickItem::polish);
    connect(this, &QQuickItem::smoothChanged, this, &QQuickItem::polish);
}


Icon::~Icon()
{
}

void Icon::setSource(const QVariant &icon)
{
    if (m_source == icon) {
        return;
    }
    m_source = icon;
    m_monochromeHeuristics.clear();

    if (!m_theme) {
        m_theme = static_cast<Kirigami::PlatformTheme *>(qmlAttachedPropertiesObject<Kirigami::PlatformTheme>(this, true));
        Q_ASSERT(m_theme);

        connect(m_theme, &Kirigami::PlatformTheme::colorsChanged, this, &QQuickItem::polish);
    }

    if (icon.type() == QVariant::String) {
        const QString iconSource = icon.toString();
        m_isMaskHeuristic = (iconSource.endsWith(QLatin1String("-symbolic"))
                            || iconSource.endsWith(QLatin1String("-symbolic-rtl"))
                            || iconSource.endsWith(QLatin1String("-symbolic-ltr")));
        emit isMaskChanged();
    }

    if (m_networkReply) {
        //if there was a network query going on, interrupt it
        m_networkReply->close();
    }
    m_loadedImage = QImage();

    polish();
    emit sourceChanged();
    emit validChanged();
}

QVariant Icon::source() const
{
    return m_source;
}

void Icon::setActive(const bool active)
{
    if (active == m_active) {
        return;
    }
    m_active = active;
    polish();
    emit activeChanged();
}

bool Icon::active() const
{
    return m_active;
}

bool Icon::valid() const
{
    return !m_source.isNull();
}

void Icon::setSelected(const bool selected)
{
    if (selected == m_selected) {
        return;
    }
    m_selected = selected;
    polish();
    emit selectedChanged();
}

bool Icon::selected() const
{
    return m_selected;
}

void Icon::setIsMask(bool mask)
{
    if (m_isMask == mask) {
        return;
    }

    m_isMask = mask;
    m_isMaskHeuristic = mask;
    polish();
    emit isMaskChanged();
}

bool Icon::isMask() const
{
    return m_isMask || m_isMaskHeuristic;
}

void Icon::setColor(const QColor &color)
{
    if (m_color == color) {
        return;
    }

    m_color = color;
    polish();
    emit colorChanged();
}

QColor Icon::color() const
{
    return m_color;
}


int Icon::implicitWidth() const
{
    return 32;
}

int Icon::implicitHeight() const
{
    return 32;
}

QSGNode* Icon::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData* /*data*/)
{
    if (m_source.isNull() || qFuzzyIsNull(width()) || qFuzzyIsNull(height())) {
        delete node;
        return Q_NULLPTR;
    }

    if (m_changed || node == nullptr) {
        const QSize itemSize(width(), height());
        QRect nodeRect(QPoint(0,0), itemSize);

        ManagedTextureNode* mNode = dynamic_cast<ManagedTextureNode*>(node);
        if (!mNode) {
            delete node;
            mNode = new ManagedTextureNode;
        }
        if (itemSize.width() != 0 && itemSize.height() != 0) {
            const auto multiplier = QCoreApplication::instance()->testAttribute(Qt::AA_UseHighDpiPixmaps) ? 1 : (window() ? window()->devicePixelRatio() : qGuiApp->devicePixelRatio());
            const QSize size = itemSize * multiplier;
            mNode->setTexture(s_iconImageCache->loadTexture(window(), m_icon));
            if (m_icon.size() != size) {
                // At this point, the image will already be scaled, but we need to output it in
                // the correct aspect ratio, painted centered in the viewport. So:
                QRect destination(QPoint(0, 0), m_icon.size().scaled(itemSize, Qt::KeepAspectRatio));
                destination.moveCenter(nodeRect.center());
                nodeRect = destination;
            }
        }
        mNode->setRect(nodeRect);
        node = mNode;
        if (smooth()) {
            mNode->setFiltering(QSGTexture::Linear);
        }
        m_changed = false;
    }

    return node;
}

void Icon::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    if (newGeometry.size() != oldGeometry.size()) {
        polish();
    }
}

void Icon::handleRedirect(QNetworkReply* reply)
{
    QNetworkAccessManager* qnam = reply->manager();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }
    const QUrl possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (!possibleRedirectUrl.isEmpty()) {
        const QUrl redirectUrl = reply->url().resolved(possibleRedirectUrl);
        if (redirectUrl == reply->url()) {
            // no infinite redirections thank you very much
            reply->deleteLater();
            return;
        }
        reply->deleteLater();
        QNetworkRequest request(possibleRedirectUrl);
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
        m_networkReply = qnam->get(request);
        connect(m_networkReply.data(), &QNetworkReply::finished, this, [this](){handleFinished(m_networkReply); });
    }
}

void Icon::handleFinished(QNetworkReply* reply)
{
    reply->deleteLater();
    if (!reply->attribute(QNetworkRequest::RedirectionTargetAttribute).isNull()) {
        handleRedirect(reply);
        return;
    }

    m_loadedImage = QImage();

    const QString filename = reply->url().fileName();
    if (!m_loadedImage.load(reply, filename.mid(filename.indexOf(QLatin1Char('.'))).toLatin1().constData())) {
        qWarning() << "received broken image" << reply->url();

        // broken image from data, inform the user of this with some useful broken-image thing...
        m_loadedImage = QIcon::fromTheme(m_fallback).pixmap(window(), QSize(width(), height()), iconMode(), QIcon::On).toImage();
    }
    polish();
}

void Icon::updatePolish()
{
    QQuickItem::updatePolish();

    if (m_source.isNull()) {
        return;
    }

    const QSize itemSize(width(), height());
    if (itemSize.width() != 0 && itemSize.height() != 0) {
        const auto multiplier = QCoreApplication::instance()->testAttribute(Qt::AA_UseHighDpiPixmaps) ? 1 : (window() ? window()->devicePixelRatio() : qGuiApp->devicePixelRatio());
        const QSize size = itemSize * multiplier;

        switch(m_source.type()){
        case QVariant::Pixmap:
            m_icon = m_source.value<QPixmap>().toImage();
            break;
        case QVariant::Image:
            m_icon = m_source.value<QImage>();
            break;
        case QVariant::Bitmap:
            m_icon = m_source.value<QBitmap>().toImage();
            break;
        case QVariant::Icon:
            m_icon = m_source.value<QIcon>().pixmap(window(), itemSize, iconMode(), QIcon::On).toImage();
            break;
        case QVariant::Url:
        case QVariant::String:
            m_icon = findIcon(size);
            break;
        case QVariant::Brush:
            //todo: fill here too?
        case QVariant::Color:
            m_icon = QImage(size, QImage::Format_Alpha8);
            m_icon.fill(m_source.value<QColor>());
            break;
        default:
            break;
        }

        if (m_icon.isNull()){
            m_icon = QImage(size, QImage::Format_Alpha8);
            m_icon.fill(Qt::transparent);
        }

        const QColor tintColor = !m_color.isValid() || m_color == Qt::transparent ? (m_selected ? m_theme->highlightedTextColor() : m_theme->textColor()) : m_color;

        //TODO: initialize m_isMask with icon.isMask()
        if (tintColor.alpha() > 0 && (isMask() || guessMonochrome(m_icon))) {
            QPainter p(&m_icon);
            p.setCompositionMode(QPainter::CompositionMode_SourceIn);
            p.fillRect(m_icon.rect(), tintColor);
            p.end();
        }
    }
    m_changed = true;
    update();
}

QImage Icon::findIcon(const QSize &size)
{
    QImage img;
    QString iconSource = m_source.toString();

    if (iconSource.startsWith(QLatin1String("image://"))) {
        const auto multiplier = QCoreApplication::instance()->testAttribute(Qt::AA_UseHighDpiPixmaps) ? (window() ? window()->devicePixelRatio() : qGuiApp->devicePixelRatio()) : 1;
        QUrl iconUrl(iconSource);
        QString iconProviderId = iconUrl.host();
        QString iconId = iconUrl.path();

        // QRC paths are not correctly handled by .path()
        if (iconId.size() >=2 && iconId.startsWith(QLatin1String("/:"))) {
            iconId.remove(0, 1);
        }

        QSize actualSize;
        QQuickImageProvider* imageProvider = dynamic_cast<QQuickImageProvider*>(
                    qmlEngine(this)->imageProvider(iconProviderId));
        if (!imageProvider)
            return img;
        switch(imageProvider->imageType()){
        case QQmlImageProviderBase::Image:
            img = imageProvider->requestImage(iconId, &actualSize, size * multiplier);
            break;
        case QQmlImageProviderBase::Pixmap:
            img = imageProvider->requestPixmap(iconId, &actualSize, size * multiplier).toImage();
            break;
        case QQmlImageProviderBase::Texture:
        case QQmlImageProviderBase::Invalid:
        case QQmlImageProviderBase::ImageResponse:
            //will have to investigate this more
            break;
        }
    } else if(iconSource.startsWith(QLatin1String("http://")) || iconSource.startsWith(QLatin1String("https://"))) {
        if(!m_loadedImage.isNull()) {
            return m_loadedImage.scaled(size, Qt::KeepAspectRatio, smooth() ? Qt::SmoothTransformation : Qt::FastTransformation );
        }
        const auto url = m_source.toUrl();
        QQmlEngine* engine = qmlEngine(this);
        QNetworkAccessManager* qnam;
        if (engine && (qnam = engine->networkAccessManager()) && (!m_networkReply || m_networkReply->url() != url)) {
            QNetworkRequest request(url);
            request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
            m_networkReply = qnam->get(request);
            connect(m_networkReply.data(), &QNetworkReply::finished, this, [this](){ handleFinished(m_networkReply); });
        }
        // Temporary icon while we wait for the real image to load...
        img = QIcon::fromTheme(QStringLiteral("image-x-icon")).pixmap(window(), size, iconMode(), QIcon::On).toImage();
    } else {
        if (iconSource.startsWith(QLatin1String("qrc:/"))) {
            iconSource = iconSource.mid(3);
        } else if (iconSource.startsWith(QLatin1String("file:/"))) {
            iconSource = QUrl(iconSource).path();
        }

        QIcon icon;
        const bool isPath = iconSource.contains(QLatin1String("/"));
        if (isPath) {
            icon = QIcon(iconSource);
        } else {
            if (icon.isNull()) {
                icon = m_theme->iconFromTheme(iconSource, m_color);
            }
        }
        if (!icon.isNull()) {
            img = icon.pixmap(window(), size, iconMode(), QIcon::On).toImage();

            /*const QColor tintColor = !m_color.isValid() || m_color == Qt::transparent ? (m_selected ? m_theme->highlightedTextColor() : m_theme->textColor()) : m_color;

            if (m_isMask || icon.isMask() || iconSource.endsWith(QLatin1String("-symbolic")) || iconSource.endsWith(QLatin1String("-symbolic-rtl")) || iconSource.endsWith(QLatin1String("-symbolic-ltr")) || guessMonochrome(img)) {
                QPainter p(&img);
                p.setCompositionMode(QPainter::CompositionMode_SourceIn);
                p.fillRect(img.rect(), tintColor);
                p.end();
            }*/
        }
    }

    if (!iconSource.isEmpty() && img.isNull()) {
        img = QIcon::fromTheme(m_fallback).pixmap(window(), size, iconMode(), QIcon::On).toImage();
    }
    return img;
}

QIcon::Mode Icon::iconMode() const
{
    if (!isEnabled()) {
        return QIcon::Disabled;
    } else if (m_selected) {
        return QIcon::Selected;
    } else if (m_active) {
        return QIcon::Active;
    }
    return QIcon::Normal;
}

bool Icon::guessMonochrome(const QImage &img)
{
    //don't try for too big images
    if (img.width() >= 256 || m_theme->supportsIconColoring()) {
        return false;
    }
    // round size to a standard size. hardcode as we can't use KIconLoader
    int stdSize;
    if (img.width() <= 16) {
        stdSize = 16;
    } else if (img.width() <= 22) {
        stdSize = 22;
    } else if (img.width() <= 24) {
        stdSize = 24;
    } else if (img.width() <= 32) {
        stdSize = 32;
    } else if (img.width() <= 48) {
        stdSize = 48;
    } else if (img.width() <= 64) {
        stdSize = 64;
    } else {
        stdSize = 128;
    }

    auto findIt = m_monochromeHeuristics.constFind(stdSize);
    if (findIt != m_monochromeHeuristics.constEnd()) {
        return findIt.value();
    }

    QHash<int, int> dist;
    int transparentPixels = 0;
    int saturatedPixels = 0;
    for(int x=0; x < img.width(); x++) {
        for(int y=0; y < img.height(); y++) {
            QColor color = QColor::fromRgba(qUnpremultiply(img.pixel(x, y)));
            if (color.alpha() < 100) {
                ++transparentPixels;
                continue;
            } else if (color.saturation() > 84) {
                ++saturatedPixels;
            }
            dist[qGray(color.rgb())]++;
        }
    }

    QMultiMap<int, int> reverseDist;
    auto it = dist.constBegin();
    qreal entropy = 0;
    while (it != dist.constEnd()) {
        reverseDist.insert(it.value(), it.key());
        qreal probability = qreal(it.value()) / qreal(img.size().width() * img.size().height() - transparentPixels);
        entropy -= probability * log(probability) / log(255);
        ++it;
    }

    // Arbitrarly low values of entropy and colored pixels
    m_monochromeHeuristics[stdSize] = saturatedPixels <= (img.size().width()*img.size().height() - transparentPixels) * 0.3 && entropy <= 0.3;
    return m_monochromeHeuristics[stdSize];
}

QString Icon::fallback() const
{
    return m_fallback;
}

void Icon::setFallback(const QString& fallback)
{
    if (m_fallback != fallback) {
        m_fallback = fallback;
        Q_EMIT fallbackChanged(fallback);
    }
}
