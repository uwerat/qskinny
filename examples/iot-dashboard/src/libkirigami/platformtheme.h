/*
 *  SPDX-FileCopyrightText: 2017 by Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PLATFORMTHEME_H
#define PLATFORMTHEME_H

#include <QObject>
#include <QQuickItem>
#include <QColor>
#include <QPalette>
#include <QIcon>

#ifndef KIRIGAMI_BUILD_TYPE_STATIC
#include <kirigami2_export.h>
#endif

namespace Kirigami {

class PlatformThemePrivate;

/**
 * @class PlatformTheme platformtheme.h PlatformTheme
 *
 * This class is the base for color management in Kirigami,
 * different platforms can reimplement this class to integrate with
 * system platform colors of a given platform
 */
#ifdef KIRIGAMI_BUILD_TYPE_STATIC
class PlatformTheme : public QObject
#else
class KIRIGAMI2_EXPORT PlatformTheme : public QObject
#endif
{
    Q_OBJECT

    /**
     * This enumeration describes the color set for which a color is being selected.
     *
     * Color sets define a color "environment", suitable for drawing all parts of a
     * given region. Colors from different sets should not be combined.
     */
    Q_PROPERTY(ColorSet colorSet READ colorSet WRITE setColorSet NOTIFY colorSetChanged)

    /**
     * This enumeration describes the color group used to generate the colors.
     * The enum value is based upon QPalette::CpolorGroup and has the same values.
     * It's redefined here in order to make it work with QML
     * @since 4.43
     */
    Q_PROPERTY(ColorGroup colorGroup READ colorGroup WRITE setColorGroup NOTIFY colorGroupChanged)

    /**
     * If true, the colorSet will be inherited from the colorset of a theme of one
     * of the ancestor items
     * default: true
     */
    Q_PROPERTY(bool inherit READ inherit WRITE setInherit NOTIFY inheritChanged)

    // foreground colors
    /**
     * Color for normal foregrounds, usually text, but not limited to it,
     * anything that should be painted with a clear contrast should use this color
     */
    Q_PROPERTY(QColor textColor READ textColor WRITE setCustomTextColor RESET setCustomTextColor NOTIFY colorsChanged)

    /**
     * Foreground color for disabled areas, usually a mid-gray
     */
    Q_PROPERTY(QColor disabledTextColor READ disabledTextColor WRITE setCustomDisabledTextColor RESET setCustomDisabledTextColor NOTIFY colorsChanged)

    /**
     * Color for text that has been highlighted, often is a light color while normal text is dark
     */
    Q_PROPERTY(QColor highlightedTextColor READ highlightedTextColor WRITE setCustomHighlightedTextColor RESET setCustomHighlightedTextColor NOTIFY colorsChanged)

    /**
     * Foreground for areas that are active or requesting attention
     */
    Q_PROPERTY(QColor activeTextColor READ activeTextColor WRITE setCustomActiveTextColor RESET setCustomActiveTextColor NOTIFY colorsChanged)

    /**
     * Color for links
     */
    Q_PROPERTY(QColor linkColor READ linkColor WRITE setCustomLinkColor RESET setCustomLinkColor NOTIFY colorsChanged)

    /**
     * Color for visited links, usually a bit darker than linkColor
     */
    Q_PROPERTY(QColor visitedLinkColor READ visitedLinkColor WRITE setCustomVisitedLinkColor RESET setCustomVisitedLinkColor NOTIFY colorsChanged)

    /**
     * Foreground color for negative areas, such as critical error text
     */
    Q_PROPERTY(QColor negativeTextColor READ negativeTextColor WRITE setCustomNegativeTextColor RESET setCustomNegativeTextColor NOTIFY colorsChanged)

    /**
     * Foreground color for neutral areas, such as warning texts (but not critical)
     */
    Q_PROPERTY(QColor neutralTextColor READ neutralTextColor WRITE setCustomNeutralTextColor RESET setCustomNeutralTextColor NOTIFY colorsChanged)

    /**
     * Success messages, trusted content
     */
    Q_PROPERTY(QColor positiveTextColor READ positiveTextColor WRITE setCustomPositiveTextColor RESET setCustomPositiveTextColor NOTIFY colorsChanged)

    //background colors
    /**
     * The generic background color
     */
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setCustomBackgroundColor RESET setCustomBackgroundColor NOTIFY colorsChanged)

    /**
     * The generic background color
     * Alternate background; for example, for use in lists.
     * This color may be the same as BackgroundNormal,
     * especially in sets other than View and Window.
     */
    Q_PROPERTY(QColor alternateBackgroundColor READ alternateBackgroundColor WRITE setCustomAlternateBackgroundColor RESET setCustomAlternateBackgroundColor NOTIFY colorsChanged)

    /**
     * The background color for selected areas
     */
    Q_PROPERTY(QColor highlightColor READ highlightColor WRITE setCustomHighlightColor RESET setCustomHighlightColor NOTIFY colorsChanged)

    /**
     * Background for areas that are active or requesting attention
     */
    Q_PROPERTY(QColor activeBackgroundColor READ activeBackgroundColor WRITE setCustomActiveBackgroundColor RESET setCustomActiveBackgroundColor NOTIFY colorsChanged)

    /**
     * Background color for links
     */
    Q_PROPERTY(QColor linkBackgroundColor READ linkBackgroundColor WRITE setCustomLinkBackgroundColor RESET setCustomLinkBackgroundColor NOTIFY colorsChanged)

    /**
     * Background color for visited links, usually a bit darker than linkBackgroundColor
     */
    Q_PROPERTY(QColor visitedLinkBackgroundColor READ visitedLinkBackgroundColor WRITE setCustomVisitedLinkBackgroundColor RESET setCustomVisitedLinkBackgroundColor NOTIFY colorsChanged)

    /**
     * Background color for negative areas, such as critical errors and destructive actions
     */
    Q_PROPERTY(QColor negativeBackgroundColor READ negativeBackgroundColor WRITE setCustomNegativeBackgroundColor RESET setCustomNegativeBackgroundColor NOTIFY colorsChanged)

    /**
     * Background color for neutral areas, such as warnings (but not critical)
     */
    Q_PROPERTY(QColor neutralBackgroundColor READ neutralBackgroundColor WRITE setCustomNeutralBackgroundColor RESET setCustomNeutralBackgroundColor NOTIFY colorsChanged)

    /**
     * Background color for positive areas, such as success messages and trusted content
     */
    Q_PROPERTY(QColor positiveBackgroundColor READ positiveBackgroundColor WRITE setCustomPositiveBackgroundColor RESET setCustomPositiveBackgroundColor NOTIFY colorsChanged)

    //decoration colors
    /**
     * A decoration color that indicates active focus
     */
    Q_PROPERTY(QColor focusColor READ focusColor WRITE setCustomFocusColor RESET setCustomFocusColor NOTIFY colorsChanged)

    /**
     * A decoration color that indicates mouse hovering
     */
    Q_PROPERTY(QColor hoverColor READ hoverColor WRITE setCustomHoverColor RESET setCustomHoverColor NOTIFY colorsChanged)

    // font and palette
    Q_PROPERTY(QFont defaultFont READ defaultFont NOTIFY defaultFontChanged)

    // small font
    Q_PROPERTY(QFont smallFont READ smallFont NOTIFY defaultFontChanged)

    //Active palette
    Q_PROPERTY(QPalette palette READ palette NOTIFY paletteChanged)

public:

    enum ColorSet {
        View = 0, /** Color set for item views, usually the lightest of all */
        Window, /** Default Color set for windows and "chrome" areas */
        Button, /** Color set used by buttons */
        Selection, /** Color set used by selectged areas */
        Tooltip, /** Color set used by tooltips */
        Complementary, /** Color set meant to be complementary to Window: usually is a dark theme for light themes */
        Header /** Color set to be used by heading areas of applications, such as toolbars */
    };
    Q_ENUM(ColorSet)

    enum ColorGroup {
        Disabled = QPalette::Disabled,
        Active = QPalette::Active,
        Inactive = QPalette::Inactive,
        Normal = QPalette::Normal
    };
    Q_ENUM(ColorGroup)

    explicit PlatformTheme(QObject *parent = nullptr);
    ~PlatformTheme();

    void setColorSet(PlatformTheme::ColorSet);
    PlatformTheme::ColorSet colorSet() const;

    void setColorGroup(PlatformTheme::ColorGroup);
    PlatformTheme::ColorGroup colorGroup() const;

    bool inherit() const;
    void setInherit(bool inherit);

    //foreground colors
    QColor textColor() const;
    QColor disabledTextColor() const;
    QColor highlightedTextColor() const;
    QColor activeTextColor() const;
    QColor linkColor() const;
    QColor visitedLinkColor() const;
    QColor negativeTextColor() const;
    QColor neutralTextColor() const;
    QColor positiveTextColor() const;

    //background colors
    QColor backgroundColor() const;
    QColor alternateBackgroundColor() const;
    QColor highlightColor() const;
    QColor activeBackgroundColor() const;
    QColor linkBackgroundColor() const;
    QColor visitedLinkBackgroundColor() const;
    QColor negativeBackgroundColor() const;
    QColor neutralBackgroundColor() const;
    QColor positiveBackgroundColor() const;

    //decoration colors
    QColor focusColor() const;
    QColor hoverColor() const;

    QFont defaultFont() const;
    QFont smallFont() const;

    //this may is used by the desktop QQC2 to set the styleoption palettes
    QPalette palette() const;

    //this will be used by desktopicon to fetch icons with KIconLoader
    virtual Q_INVOKABLE QIcon iconFromTheme(const QString &name, const QColor &customColor = Qt::transparent);

    bool supportsIconColoring() const;

    //foreground colors
    void setCustomTextColor(const QColor &color = QColor());
    void setCustomDisabledTextColor(const QColor &color = QColor());
    void setCustomHighlightedTextColor(const QColor &color = QColor());
    void setCustomActiveTextColor(const QColor &color = QColor());
    void setCustomLinkColor(const QColor &color = QColor());
    void setCustomVisitedLinkColor(const QColor &color = QColor());
    void setCustomNegativeTextColor(const QColor &color = QColor());
    void setCustomNeutralTextColor(const QColor &color = QColor());
    void setCustomPositiveTextColor(const QColor &color = QColor());
    //background colors
    void setCustomBackgroundColor(const QColor &color = QColor());
    void setCustomAlternateBackgroundColor(const QColor &color = QColor());
    void setCustomHighlightColor(const QColor &color = QColor());
    void setCustomActiveBackgroundColor(const QColor &color = QColor());
    void setCustomLinkBackgroundColor(const QColor &color = QColor());
    void setCustomVisitedLinkBackgroundColor(const QColor &color = QColor());
    void setCustomNegativeBackgroundColor(const QColor &color = QColor());
    void setCustomNeutralBackgroundColor(const QColor &color = QColor());
    void setCustomPositiveBackgroundColor(const QColor &color = QColor());
    //decoration colors
    void setCustomFocusColor(const QColor &color = QColor());
    void setCustomHoverColor(const QColor &color = QColor());

    //QML attached property
    static PlatformTheme *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    //TODO: parameters to signals as this is also a c++ api
    void colorsChanged();
    void defaultFontChanged(const QFont &font);
    void smallFontChanged(const QFont &font);
    void colorSetChanged(Kirigami::PlatformTheme::ColorSet colorSet);
    void colorGroupChanged(Kirigami::PlatformTheme::ColorGroup colorGroup);
    void paletteChanged(const QPalette &pal);
    void inheritChanged(bool inherit);

protected:
    //Setters, not accessible from QML but from implementations
    void setSupportsIconColoring(bool support);

    //foreground colors
    void setTextColor(const QColor &color);
    void setDisabledTextColor(const QColor &color);
    void setHighlightedTextColor(const QColor &color);
    void setActiveTextColor(const QColor &color);
    void setLinkColor(const QColor &color);
    void setVisitedLinkColor(const QColor &color);
    void setNegativeTextColor(const QColor &color);
    void setNeutralTextColor(const QColor &color);
    void setPositiveTextColor(const QColor &color);

    //background colors
    void setBackgroundColor(const QColor &color);
    void setAlternateBackgroundColor(const QColor &color);
    void setHighlightColor(const QColor &color);
    void setActiveBackgroundColor(const QColor &color);
    void setLinkBackgroundColor(const QColor &color);
    void setVisitedLinkBackgroundColor(const QColor &color);
    void setNegativeBackgroundColor(const QColor &color);
    void setNeutralBackgroundColor(const QColor &color);
    void setPositiveBackgroundColor(const QColor &color);

    //decoration colors
    void setFocusColor(const QColor &color);
    void setHoverColor(const QColor &color);

    void setDefaultFont(const QFont &defaultFont);
    void setSmallFont(const QFont &smallFont);
    void setPalette(const QPalette &palette);
private:
    PlatformThemePrivate *d;
    friend class PlatformThemePrivate;
};

}

QML_DECLARE_TYPEINFO(Kirigami::PlatformTheme, QML_HAS_ATTACHED_PROPERTIES)

#endif // PLATFORMTHEME_H
