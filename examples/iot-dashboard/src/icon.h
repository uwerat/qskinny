/*
 *  SPDX-FileCopyrightText: 2011 Marco Martin <mart@kde.org>
 *  SPDX-FileCopyrightText: 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *  SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QIcon>
#include <QQuickItem>
#include <QVariant>
#include <QPointer>

class QNetworkReply;

namespace Kirigami {
    class PlatformTheme;
}

/**
 * Class for rendering an icon in UI.
 */
class Icon : public QQuickItem
{
    Q_OBJECT

    /**
     * The source of this icon. An `Icon` can pull from:
     * 
     * * The icon theme:
     * @include icon/IconThemeSource.qml
     * * The filesystem:
     * @include icon/FilesystemSource.qml
     * * Remote URIs:
     * @include icon/InternetSource.qml
     * * Custom providers:
     * @include icon/CustomSource.qml
     * * Your application's bundled resources:
     * @include icon/ResourceSource.qml
     * 
     * @note See https://doc.qt.io/qt-5/qtquickcontrols2-icons.html for how to
     * bundle icon themes in your application to refer to them by name instead of
     * by resource URL.
     * 
     * @note Use `fallback` to provide a fallback theme name for icons.
     */
    Q_PROPERTY(QVariant source READ source WRITE setSource NOTIFY sourceChanged)

    /**
     * The name of a fallback icon to load from the icon theme when the `source`
     * cannot be found. The default fallback icon is `"unknown"`.
     * 
     * @include icon/Fallback.qml
     * 
     * @note This will only be loaded if source is unavailable (e.g. it doesn't exist, or network issues have prevented loading).
     */
    Q_PROPERTY(QString fallback READ fallback WRITE setFallback NOTIFY fallbackChanged)

    /**
     * The `implicitWidth` of this item, derived from the `source` image.
     */
    Q_PROPERTY(int implicitWidth READ implicitWidth CONSTANT)

    /**
     * The `implicitHeight` of this item, derived from the `source` image.
     */
    Q_PROPERTY(int implicitHeight READ implicitHeight CONSTANT)

    /**
     * Whether this icon will use the QIcon::Active mode when drawing the icon,
     * resulting in a graphical effect being applied to the icon to indicate that
     * it is currently active.
     * 
     * This is typically used to indicate when an item is being hovered or pressed.
     * 
     * @image html icon/active.png
     * 
     * The color differences under the default KDE color palette, Breeze. Note
     * that a dull highlight background is typically displayed behind active icons and
     * it is recommended to add one if you are creating a custom component.
     */
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

    /**
     * Whether this icon's `source` is valid and it is being used.
     */
    Q_PROPERTY(bool valid READ valid NOTIFY validChanged)

    /**
     * Whether this icon will use the QIcon::Selected mode when drawing the icon,
     * resulting in a graphical effect being applied to the icon to indicate that
     * it is currently selected.
     * 
     * This is typically used to indicate when a list item is currently selected.
     * 
     * @image html icon/selected.png
     * 
     * The color differences under the default KDE color palette, Breeze. Note
     * that a blue background is typically displayed behind selected elements.
     */
    Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectedChanged)

    /**
     * Whether this icon will be treated as a mask. When an icon is being used
     * as a mask, all non-transparent colors are replaced with the color provided in the Icon's
     * @link Icon::color color @endlink property.
     * 
     * @see color
     */
    Q_PROPERTY(bool isMask READ isMask WRITE setIsMask NOTIFY isMaskChanged)

    /**
     * The color to use when drawing this icon when `isMask` is enabled.
     * If this property is not set or is `Qt::transparent`, the icon will use
     * the text or the selected text color, depending on if `selected` is set to
     * true.
     */
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    Icon(QQuickItem *parent = nullptr);
    ~Icon();

    void setSource(const QVariant &source);
    QVariant source() const;

    int implicitWidth() const;
    int implicitHeight() const;

    void setActive(bool active = true);
    bool active() const;

    bool valid() const;

    void setSelected(bool selected = true);
    bool selected() const;

    void setIsMask(bool mask);
    bool isMask() const;

    void setColor(const QColor &color);
    QColor color() const;

    QString fallback() const;
    void setFallback(const QString &fallback);

    QSGNode* updatePaintNode(QSGNode* node, UpdatePaintNodeData* data) override;

Q_SIGNALS:
    void sourceChanged();
    void activeChanged();
    void validChanged();
    void selectedChanged();
    void isMaskChanged();
    void colorChanged();
    void fallbackChanged(const QString &fallback);

protected:
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    QImage findIcon(const QSize& size);
    void handleFinished(QNetworkReply* reply);
    void handleRedirect(QNetworkReply* reply);
    QIcon::Mode iconMode() const;
    bool guessMonochrome(const QImage &img);
    void updatePolish() override;

private:
    Kirigami::PlatformTheme *m_theme = nullptr;
    QPointer<QNetworkReply> m_networkReply;
    QHash<int, bool> m_monochromeHeuristics;
    QVariant m_source;
    bool m_changed;
    bool m_active;
    bool m_selected;
    bool m_isMask;
    bool m_isMaskHeuristic = false;
    QImage m_loadedImage;
    QColor m_color = Qt::transparent;
    QString m_fallback = QStringLiteral("unknown");

    QImage m_icon;
};

