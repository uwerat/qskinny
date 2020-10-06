/*
 *  SPDX-FileCopyrightText: 2017 by Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "platformtheme.h"
#include "kirigamipluginfactory.h"
#include "basictheme_p.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QGuiApplication>
#include <QDebug>
#include <QPointer>
#include <QQuickWindow>
#include <QPluginLoader>
#include <QDir>
#include <QQuickStyle>

namespace Kirigami {

class PlatformThemePrivate {
public:
    PlatformThemePrivate(PlatformTheme *q);
    ~PlatformThemePrivate();

    inline void syncCustomPalette();
    void emitCompressedColorChanged();
    void findParentStyle();
    static QColor tint(const QColor &c1, const QColor &c2, qreal ratio);


    PlatformTheme *q;
    PlatformTheme::ColorSet m_colorSet = PlatformTheme::Window;
    PlatformTheme::ColorGroup m_colorGroup = PlatformTheme::Active;
    QSet<PlatformTheme *> m_childThemes;
    QPointer<PlatformTheme> m_parentTheme;

    //ordinary colors
    QColor textColor;
    QColor disabledTextColor;
    QColor highlightedTextColor;
    QColor activeTextColor;
    QColor linkColor;
    QColor visitedLinkColor;
    QColor negativeTextColor;
    QColor neutralTextColor;
    QColor positiveTextColor;

    QColor backgroundColor;
    QColor alternateBackgroundColor;
    QColor highlightColor;
    QColor activeBackgroundColor;
    QColor linkBackgroundColor;
    QColor visitedLinkBackgroundColor;
    QColor negativeBackgroundColor;
    QColor neutralBackgroundColor;
    QColor positiveBackgroundColor;

    QColor focusColor;
    QColor hoverColor;

    QPalette palette;

    //custom colors
    QColor customTextColor;
    QColor customDisabledTextColor;
    QColor customHighlightedTextColor;
    QColor customActiveTextColor;
    QColor customLinkColor;
    QColor customVisitedLinkColor;
    QColor customNegativeTextColor;
    QColor customNeutralTextColor;
    QColor customPositiveTextColor;

    QColor customBackgroundColor;
    QColor customAlternateBackgroundColor;
    QColor customHighlightColor;
    QColor customActiveBackgroundColor;
    QColor customLinkBackgroundColor;
    QColor customVisitedLinkBackgroundColor;
    QColor customNegativeBackgroundColor;
    QColor customNeutralBackgroundColor;
    QColor customPositiveBackgroundColor;

    QColor customFocusColor;
    QColor customHoverColor;

    QPalette customPalette;

    QFont font;
    QFont smallFont;

    bool m_inherit = true;
    bool m_init = true;
    bool m_supportsIconColoring = false;
    bool m_pendingColorChange = false;

    static KirigamiPluginFactory *s_pluginFactory;
};

KirigamiPluginFactory *PlatformThemePrivate::s_pluginFactory = nullptr;

PlatformThemePrivate::PlatformThemePrivate(PlatformTheme *q)
    : q(q)
{
}

PlatformThemePrivate::~PlatformThemePrivate()
{}

void setPaletteColor(QPalette& customPalette, QPalette::ColorGroup cg, QPalette::ColorRole cr, const QColor &color, bool *changed)
{
    if (customPalette.color(cg, cr) != color) {
        *changed = true;
        customPalette.setColor(cg, cr, color);
    }
}

void PlatformThemePrivate::syncCustomPalette()
{
    bool changed = false;
    for (auto state : { QPalette::Active, QPalette::Inactive, QPalette::Disabled }) {
        setPaletteColor(customPalette, state, QPalette::WindowText, q->textColor(), &changed);
        setPaletteColor(customPalette, state, QPalette::Window, q->backgroundColor(), &changed);
        setPaletteColor(customPalette, state, QPalette::Base, q->backgroundColor(), &changed);
        setPaletteColor(customPalette, state, QPalette::Text, q->textColor(), &changed);
        setPaletteColor(customPalette, state, QPalette::Button, q->backgroundColor(), &changed);
        setPaletteColor(customPalette, state, QPalette::ButtonText, q->textColor(), &changed);
        setPaletteColor(customPalette, state, QPalette::Highlight, q->highlightColor(), &changed);
        setPaletteColor(customPalette, state, QPalette::HighlightedText, q->highlightedTextColor(), &changed);
        setPaletteColor(customPalette, state, QPalette::ToolTipBase, q->backgroundColor(), &changed);
        setPaletteColor(customPalette, state, QPalette::ToolTipText, q->textColor(), &changed);
        setPaletteColor(customPalette, state, QPalette::Link, q->linkColor(), &changed);
        setPaletteColor(customPalette, state, QPalette::LinkVisited, q->visitedLinkColor(), &changed);
    }
    if (changed) {
        emit q->paletteChanged(customPalette);
    }
}

void PlatformThemePrivate::findParentStyle()
{
    if (m_parentTheme) {
        m_parentTheme->d->m_childThemes.remove(q);
    }
    QQuickItem *candidate = qobject_cast<QQuickItem *>(q->parent());
    while (candidate) {
        candidate = candidate->parentItem();
        PlatformTheme *t = static_cast<PlatformTheme *>(qmlAttachedPropertiesObject<PlatformTheme>(candidate, false));
        if (t) {
            t->d->m_childThemes.insert(q);
            m_parentTheme = t;
            if (m_inherit) {
                q->setColorSet(t->colorSet());

                q->setCustomTextColor(t->d->customTextColor);
                q->setCustomDisabledTextColor(t->d->customDisabledTextColor);
                q->setCustomHighlightedTextColor(t->d->customHighlightedTextColor);
                q->setCustomActiveTextColor(t->d->customActiveTextColor);
                q->setCustomLinkColor(t->d->customLinkColor);
                q->setCustomVisitedLinkColor(t->d->customVisitedLinkColor);
                q->setCustomNegativeTextColor(t->d->customNegativeTextColor);
                q->setCustomNeutralTextColor(t->d->customNeutralTextColor);
                q->setCustomPositiveTextColor(t->d->customPositiveTextColor);
                q->setCustomBackgroundColor(t->d->customBackgroundColor);
                q->setCustomAlternateBackgroundColor(t->d->customAlternateBackgroundColor);
                q->setCustomHighlightColor(t->d->customHighlightColor);
                q->setCustomActiveBackgroundColor(t->d->customActiveBackgroundColor);
                q->setCustomLinkBackgroundColor(t->d->customLinkBackgroundColor);
                q->setCustomVisitedLinkBackgroundColor(t->d->customVisitedLinkBackgroundColor);
                q->setCustomNegativeBackgroundColor(t->d->customNegativeBackgroundColor);
                q->setCustomNeutralBackgroundColor(t->d->customNeutralBackgroundColor);
                q->setCustomPositiveBackgroundColor(t->d->customPositiveBackgroundColor);
                q->setCustomFocusColor(t->d->customFocusColor);
                q->setCustomHoverColor(t->d->customHoverColor);
            }
            break;
        }

    }
}

QColor PlatformThemePrivate::tint(const QColor &c1, const QColor &c2, qreal ratio)
{
    qreal r = c1.redF() + (c2.redF() - c1.redF()) * ratio;
    qreal g = c1.greenF() + (c2.greenF() - c1.greenF()) * ratio;
    qreal b = c1.blueF() + (c2.blueF() - c1.blueF()) * ratio;

    return QColor::fromRgbF(r, g, b, 1);
}

void PlatformThemePrivate::emitCompressedColorChanged()
{
    if (m_pendingColorChange) {
        return;
    }

    m_pendingColorChange = true;
    QMetaObject::invokeMethod(q, [this]() {
        syncCustomPalette();
        emit q->colorsChanged();
        m_pendingColorChange = false;
    }, Qt::QueuedConnection);
}



PlatformTheme::PlatformTheme(QObject *parent)
    : QObject(parent),
      d(new PlatformThemePrivate(this))
{
    d->findParentStyle();

    if (QQuickItem *item = qobject_cast<QQuickItem *>(parent)) {
        connect(item, &QQuickItem::windowChanged, this, [this]() {
            d->findParentStyle();
        });
        connect(item, &QQuickItem::parentChanged, this, [this]() {
            d->findParentStyle();
        });
    }
    d->m_init = false;
    //TODO: needs https://codereview.qt-project.org/#/c/206889/ for font changes
}

PlatformTheme::~PlatformTheme()
{
    if (d->m_parentTheme) {
        d->m_parentTheme->d->m_childThemes.remove(this);
    }
    delete d;
}

void PlatformTheme::setColorSet(PlatformTheme::ColorSet colorSet)
{
    if (d->m_colorSet == colorSet) {
        return;
    }

    d->m_colorSet = colorSet;

    for (PlatformTheme *t : qAsConst(d->m_childThemes)) {
        if (t->inherit()) {
            t->setColorSet(colorSet);
        }
    }

    if (!d->m_init) {
        emit colorSetChanged(colorSet);
        d->emitCompressedColorChanged();
    }
}

PlatformTheme::ColorSet PlatformTheme::colorSet() const
{
    return d->m_colorSet;
}

void PlatformTheme::setColorGroup(PlatformTheme::ColorGroup colorGroup)
{
    if (d->m_colorGroup == colorGroup) {
        return;
    }

    d->m_colorGroup = colorGroup;

    for (PlatformTheme *t : qAsConst(d->m_childThemes)) {
        if (t->inherit()) {
            t->setColorGroup(colorGroup);
        }
    }

    if (!d->m_init) {
        emit colorGroupChanged(colorGroup);
        d->emitCompressedColorChanged();
    }
}

PlatformTheme::ColorGroup PlatformTheme::colorGroup() const
{
    return d->m_colorGroup;
}

bool PlatformTheme::inherit() const
{
    return d->m_inherit;
}

void PlatformTheme::setInherit(bool inherit)
{
    if (d->m_inherit == inherit) {
        return;
    }

    d->m_inherit = inherit;
    if (inherit && d->m_parentTheme) {
        setColorSet(d->m_parentTheme->colorSet());
    }
    emit inheritChanged(inherit);
}

QColor PlatformTheme::textColor() const
{
    return d->customTextColor.isValid() ? d->customTextColor : d->textColor;
}

QColor PlatformTheme::disabledTextColor() const
{
    return d->customDisabledTextColor.isValid() ? d->customDisabledTextColor : d->disabledTextColor;
}

QColor PlatformTheme::highlightColor() const
{
    return d->customHighlightColor.isValid() ? d->customHighlightColor : d->highlightColor;
}

QColor PlatformTheme::highlightedTextColor() const
{
    return d->customHighlightedTextColor.isValid() ? d->customHighlightedTextColor : d->highlightedTextColor;
}

QColor PlatformTheme::backgroundColor() const
{
    return d->customBackgroundColor.isValid() ? d->customBackgroundColor : d->backgroundColor;
}

QColor PlatformTheme::alternateBackgroundColor() const
{
    return d->customAlternateBackgroundColor.isValid() ? d->customAlternateBackgroundColor : d->alternateBackgroundColor;
}

QColor PlatformTheme::activeTextColor() const
{
    return d->customActiveTextColor.isValid() ? d->customActiveTextColor : d->activeTextColor;
}

QColor PlatformTheme::activeBackgroundColor() const
{
    return d->customActiveBackgroundColor.isValid() ? d->customActiveBackgroundColor : d->activeBackgroundColor;
}

QColor PlatformTheme::linkColor() const
{
    return d->customLinkColor.isValid() ? d->customLinkColor : d->linkColor;
}

QColor PlatformTheme::linkBackgroundColor() const
{
    return d->customLinkBackgroundColor.isValid() ? d->customLinkBackgroundColor : d->linkBackgroundColor;
}

QColor PlatformTheme::visitedLinkColor() const
{
    return d->customVisitedLinkColor.isValid() ? d->customVisitedLinkColor : d->visitedLinkColor;
}

QColor PlatformTheme::visitedLinkBackgroundColor() const
{
    return d->customVisitedLinkBackgroundColor.isValid() ? d->customVisitedLinkBackgroundColor : d->visitedLinkBackgroundColor;
}

QColor PlatformTheme::negativeTextColor() const
{
    return d->customNegativeTextColor.isValid() ? d->customNegativeTextColor : d->negativeTextColor;
}

QColor PlatformTheme::negativeBackgroundColor() const
{
    return d->customNegativeBackgroundColor.isValid() ? d->customNegativeBackgroundColor : d->negativeBackgroundColor;
}

QColor PlatformTheme::neutralTextColor() const
{
    return d->customNeutralTextColor.isValid() ? d->customNeutralTextColor : d->neutralTextColor;
}

QColor PlatformTheme::neutralBackgroundColor() const
{
    return d->customNeutralBackgroundColor.isValid() ? d->customNeutralBackgroundColor : d->neutralBackgroundColor;
}

QColor PlatformTheme::positiveTextColor() const
{
    return d->customPositiveTextColor.isValid() ? d->customPositiveTextColor : d->positiveTextColor;
}

QColor PlatformTheme::positiveBackgroundColor() const
{
    return d->customPositiveBackgroundColor.isValid() ? d->customPositiveBackgroundColor : d->positiveBackgroundColor;
}

QColor PlatformTheme::focusColor() const
{
    return d->customFocusColor.isValid() ? d->customFocusColor : d->focusColor;
}

QColor PlatformTheme::hoverColor() const
{
    return d->customHoverColor.isValid() ? d->customHoverColor : d->hoverColor;
}

//setters for theme implementations
void PlatformTheme::setTextColor(const QColor &color)
{
    if (d->textColor == color) {
        return;
    }

    d->textColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setDisabledTextColor(const QColor &color)
{
    if (d->disabledTextColor == color) {
        return;
    }

    d->disabledTextColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setBackgroundColor(const QColor &color)
{
    if (d->backgroundColor == color) {
        return;
    }

    d->backgroundColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setAlternateBackgroundColor(const QColor &color)
{
    if (d->alternateBackgroundColor == color) {
        return;
    }

    d->alternateBackgroundColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setHighlightColor(const QColor &color)
{
    if (d->highlightColor == color) {
        return;
    }

    d->highlightColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setHighlightedTextColor(const QColor &color)
{
    if (d->highlightedTextColor == color) {
        return;
    }

    d->highlightedTextColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setActiveTextColor(const QColor &color)
{
    if (d->activeTextColor == color) {
        return;
    }

    d->activeTextColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setActiveBackgroundColor(const QColor &color)
{
    if (d->activeBackgroundColor == color) {
        return;
    }

    d->activeBackgroundColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setLinkColor(const QColor &color)
{
    if (d->linkColor == color) {
        return;
    }

    d->linkColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setLinkBackgroundColor(const QColor &color)
{
    if (d->linkBackgroundColor == color) {
        return;
    }

    d->linkBackgroundColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setVisitedLinkColor(const QColor &color)
{
    if (d->visitedLinkColor == color) {
        return;
    }

    d->visitedLinkColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setVisitedLinkBackgroundColor(const QColor &color)
{
    if (d->visitedLinkBackgroundColor == color) {
        return;
    }

    d->visitedLinkBackgroundColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setNegativeTextColor(const QColor &color)
{
    if (d->negativeTextColor == color) {
        return;
    }

    d->negativeTextColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setNegativeBackgroundColor(const QColor &color)
{
    if (d->negativeBackgroundColor == color) {
        return;
    }

    d->negativeBackgroundColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setNeutralTextColor(const QColor &color)
{
    if (d->neutralTextColor == color) {
        return;
    }

    d->neutralTextColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setNeutralBackgroundColor(const QColor &color)
{
    if (d->neutralBackgroundColor == color) {
        return;
    }

    d->neutralBackgroundColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setPositiveTextColor(const QColor &color)
{
    if (d->positiveTextColor == color) {
        return;
    }

    d->positiveTextColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setPositiveBackgroundColor(const QColor &color)
{
    if (d->positiveBackgroundColor == color) {
        return;
    }

    d->positiveBackgroundColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setHoverColor(const QColor &color)
{
    if (d->hoverColor == color) {
        return;
    }

    d->hoverColor = color;
    d->emitCompressedColorChanged();
}

void PlatformTheme::setFocusColor(const QColor &color)
{
    if (d->focusColor == color) {
        return;
    }

    d->focusColor = color;
    d->emitCompressedColorChanged();
}

QFont PlatformTheme::defaultFont() const
{
    return d->font;
}

void PlatformTheme::setDefaultFont(const QFont &font)
{
    if (d->font == font) {
        return;
    }

    d->font = font;
    emit defaultFontChanged(font);
}

QFont PlatformTheme::smallFont() const
{
    return d->smallFont;
}

void PlatformTheme::setSmallFont(const QFont &font)
{
    if (d->smallFont != font) {
        d->smallFont = font;
        emit smallFontChanged(font);
    }
}

#define PROPAGATECUSTOMCOLOR(colorName, color)\
            for (PlatformTheme *t : qAsConst(d->m_childThemes)) {\
                if (t->inherit()) {\
                    t->set##colorName(color);\
                }\
            }


//setters for QML clients
void PlatformTheme::setCustomTextColor(const QColor &color)
{
    if (d->customTextColor == color) {
        return;
    }

    d->customTextColor = color;
    PROPAGATECUSTOMCOLOR(CustomTextColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomDisabledTextColor(const QColor &color)
{
    if (d->customDisabledTextColor == color) {
        return;
    }

    d->customDisabledTextColor = color;
    PROPAGATECUSTOMCOLOR(CustomDisabledTextColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomBackgroundColor(const QColor &color)
{
    if (d->customBackgroundColor == color) {
        return;
    }

    d->customBackgroundColor = color;
    PROPAGATECUSTOMCOLOR(CustomBackgroundColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomAlternateBackgroundColor(const QColor &color)
{
    if (d->customAlternateBackgroundColor == color) {
        return;
    }

    d->customAlternateBackgroundColor = color;
    PROPAGATECUSTOMCOLOR(CustomAlternateBackgroundColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomHighlightColor(const QColor &color)
{
    if (d->customHighlightColor == color) {
        return;
    }

    d->customHighlightColor = color;
    PROPAGATECUSTOMCOLOR(CustomHighlightColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomHighlightedTextColor(const QColor &color)
{
    if (d->customHighlightedTextColor == color) {
        return;
    }

    d->customHighlightedTextColor = color;
    PROPAGATECUSTOMCOLOR(CustomHighlightedTextColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomActiveTextColor(const QColor &color)
{
    if (d->customActiveTextColor == color) {
        return;
    }

    d->customActiveTextColor = color;
    PROPAGATECUSTOMCOLOR(CustomActiveTextColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomActiveBackgroundColor(const QColor &color)
{
    if (d->customActiveBackgroundColor == color) {
        return;
    }

    d->customActiveBackgroundColor = color;
    PROPAGATECUSTOMCOLOR(CustomActiveBackgroundColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomLinkColor(const QColor &color)
{
    if (d->customLinkColor == color) {
        return;
    }

    d->customLinkColor = color;
    PROPAGATECUSTOMCOLOR(CustomLinkColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomLinkBackgroundColor(const QColor &color)
{
    if (d->customLinkBackgroundColor == color) {
        return;
    }

    d->customLinkBackgroundColor = color;
    PROPAGATECUSTOMCOLOR(CustomLinkBackgroundColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomVisitedLinkColor(const QColor &color)
{
    if (d->customVisitedLinkColor == color) {
        return;
    }

    d->customVisitedLinkColor = color;
    PROPAGATECUSTOMCOLOR(CustomVisitedLinkColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomVisitedLinkBackgroundColor(const QColor &color)
{
    if (d->customVisitedLinkBackgroundColor == color) {
        return;
    }

    d->customVisitedLinkBackgroundColor = color;
    PROPAGATECUSTOMCOLOR(CustomVisitedLinkBackgroundColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomNegativeTextColor(const QColor &color)
{
    if (d->customNegativeTextColor == color) {
        return;
    }

    d->customNegativeTextColor = color;
    PROPAGATECUSTOMCOLOR(CustomNegativeTextColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomNegativeBackgroundColor(const QColor &color)
{
    if (d->customNegativeBackgroundColor == color) {
        return;
    }

    d->customNegativeBackgroundColor = color;
    PROPAGATECUSTOMCOLOR(CustomNegativeBackgroundColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomNeutralTextColor(const QColor &color)
{
    if (d->customNeutralTextColor == color) {
        return;
    }

    d->customNeutralTextColor = color;
    PROPAGATECUSTOMCOLOR(CustomNeutralTextColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomNeutralBackgroundColor(const QColor &color)
{
    if (d->customNeutralBackgroundColor == color) {
        return;
    }

    d->customNeutralBackgroundColor = color;
    PROPAGATECUSTOMCOLOR(CustomNeutralBackgroundColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomPositiveTextColor(const QColor &color)
{
    if (d->customPositiveTextColor == color) {
        return;
    }

    d->customPositiveTextColor = color;
    PROPAGATECUSTOMCOLOR(CustomPositiveTextColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomPositiveBackgroundColor(const QColor &color)
{
    if (d->customPositiveBackgroundColor == color) {
        return;
    }

    d->customPositiveBackgroundColor = color;
    PROPAGATECUSTOMCOLOR(CustomPositiveBackgroundColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomHoverColor(const QColor &color)
{
    if (d->customHoverColor == color) {
        return;
    }

    d->customHoverColor = color;
    PROPAGATECUSTOMCOLOR(CustomHoverColor, color)
    d->emitCompressedColorChanged();
}

void PlatformTheme::setCustomFocusColor(const QColor &color)
{
    if (d->customFocusColor == color) {
        return;
    }

    d->customFocusColor = color;
    PROPAGATECUSTOMCOLOR(CustomFocusColor, color)
    d->emitCompressedColorChanged();
}


QPalette PlatformTheme::palette() const
{
    //check the most important custom colors to decide to return a custom palette
    return d->customTextColor.isValid() || d->customBackgroundColor.isValid() || d->customHighlightColor.isValid() ? d->customPalette : d->palette;
}

void PlatformTheme::setPalette(const QPalette &palette)
{
    if (d->palette == palette) {
        return;
    }

    d->palette = palette;
    PROPAGATECUSTOMCOLOR(Palette, palette)
    emit paletteChanged(palette);
}

QIcon PlatformTheme::iconFromTheme(const QString &name, const QColor &customColor)
{
    Q_UNUSED(customColor);
    QIcon icon = QIcon::fromTheme(name);
    return icon;
}

bool PlatformTheme::supportsIconColoring() const
{
    return d->m_supportsIconColoring;
}

void PlatformTheme::setSupportsIconColoring(bool support)
{
    d->m_supportsIconColoring = support;
}


PlatformTheme *PlatformTheme::qmlAttachedProperties(QObject *object)
{
    static bool s_factoryChecked = false;

    //check for the plugin only once: it's an heavy operation
    if (PlatformThemePrivate::s_pluginFactory) {
        return PlatformThemePrivate::s_pluginFactory->createPlatformTheme(object);
    } else if (!s_factoryChecked) {
        s_factoryChecked = true;

#ifdef KIRIGAMI_BUILD_TYPE_STATIC
        for (QObject* staticPlugin : QPluginLoader::staticInstances()) {
            KirigamiPluginFactory *factory = qobject_cast<KirigamiPluginFactory *>(staticPlugin);
            if (factory) {
                PlatformThemePrivate::s_pluginFactory = factory;
                return factory->createPlatformTheme(object);
            }
        }
#else
        const auto libraryPaths = QCoreApplication::libraryPaths();
        for (const QString &path : libraryPaths) {
            QDir dir(path + QStringLiteral("/kf5/kirigami"));
            const auto fileNames = dir.entryList(QDir::Files);
            for (const QString &fileName : fileNames) {
                //TODO: env variable?
                if (!QQuickStyle::name().isEmpty() && fileName.startsWith(QQuickStyle::name())) {
                    QPluginLoader loader(dir.absoluteFilePath(fileName));
                    QObject *plugin = loader.instance();
                    //TODO: load actually a factory as plugin

                    KirigamiPluginFactory *factory = qobject_cast<KirigamiPluginFactory *>(plugin);
                    if (factory) {
                        PlatformThemePrivate::s_pluginFactory = factory;
                        return factory->createPlatformTheme(object);
                    }
                }
            }
        }
#endif
    }

    return new BasicTheme(object);
}

}

#include "moc_platformtheme.cpp"
