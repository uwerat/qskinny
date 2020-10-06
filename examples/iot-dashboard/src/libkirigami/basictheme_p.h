/*
 *  SPDX-FileCopyrightText: 2017 by Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef BASICTHEME_H
#define BASICTHEME_H

#include "platformtheme.h"
#include <QObject>
#include <QQuickItem>
#include <QColor>
#include <QPointer>

namespace Kirigami {

class BasicTheme;

class BasicThemeDeclarative
{
public:
    BasicThemeDeclarative();
    virtual ~BasicThemeDeclarative();

    QObject *instance(const BasicTheme *theme);

    QTimer *m_colorSyncTimer;

private:
    QUrl m_qmlPath;
    //The instance can die when and if the engine dies
    QPointer<QObject> m_declarativeBasicTheme;
};

class BasicTheme : public PlatformTheme
{
    Q_OBJECT

    // colors
    Q_PROPERTY(QColor buttonTextColor READ buttonTextColor NOTIFY colorsChanged)
    Q_PROPERTY(QColor buttonBackgroundColor READ buttonBackgroundColor NOTIFY colorsChanged)
    Q_PROPERTY(QColor buttonHoverColor READ buttonHoverColor NOTIFY colorsChanged)
    Q_PROPERTY(QColor buttonFocusColor READ buttonFocusColor NOTIFY colorsChanged)

    Q_PROPERTY(QColor viewTextColor READ viewTextColor NOTIFY colorsChanged)
    Q_PROPERTY(QColor viewBackgroundColor READ viewBackgroundColor NOTIFY colorsChanged)
    Q_PROPERTY(QColor viewHoverColor READ viewHoverColor NOTIFY colorsChanged)
    Q_PROPERTY(QColor viewFocusColor READ viewFocusColor NOTIFY colorsChanged)

public:
    explicit BasicTheme(QObject *parent = nullptr);
    ~BasicTheme();

    void syncColors();

    QColor buttonTextColor() const;
    QColor buttonBackgroundColor() const;
    QColor buttonHoverColor() const;
    QColor buttonFocusColor() const;

    QColor viewTextColor() const;
    QColor viewBackgroundColor() const;
    QColor viewHoverColor() const;
    QColor viewFocusColor() const;

    static BasicThemeDeclarative *basicThemeDeclarative();

private:
    inline void syncCustomColorsToQML();
    //legacy colors
    QColor m_buttonTextColor;
    QColor m_buttonBackgroundColor;
    QColor m_buttonHoverColor;
    QColor m_buttonFocusColor;
    QColor m_viewTextColor;
    QColor m_viewBackgroundColor;
    QColor m_viewHoverColor;
    QColor m_viewFocusColor;
};

}

#endif // BASICTHEME_H
