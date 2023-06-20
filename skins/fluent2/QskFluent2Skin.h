/******************************************************************************
 * QSkinny - Copyright (C) 2023 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FLUENT2_SKIN_H
#define QSK_FLUENT2_SKIN_H

#include "QskFluent2Global.h"

#include <QskBoxShapeMetrics.h>
#include <QskGradient.h>
#include <QskShadowMetrics.h>
#include <QskSkin.h>

#include <array>

class QSK_FLUENT2_EXPORT QskFluent2Theme
{
  public:
    enum Theme
    {
        Light,
        Dark
    };

    enum AccentColors
    {
        AccentLight3,
        AccentLight2,
        AccentLight1,
        AccentBase,
        AccentDark1,
        AccentDark2,
        AccentDark3,

        NumAccentColors
    };

    QskFluent2Theme( Theme );
    QskFluent2Theme( Theme, const std::array< QRgb, NumAccentColors >& );

    typedef std::array< QRgb, 2 > BorderGradient;

    struct FillColor
    {
        struct
        {
            QRgb primary;
            QRgb secondary;
            QRgb tertiary;
            QRgb disabled;
        } text;

        struct
        {
            QRgb primary;
            QRgb secondary;
            QRgb tertiary;
            QRgb disabled;
        } accentText;

        struct
        {
            QRgb primary;
            QRgb secondary;
            QRgb tertiary;
            QRgb disabled;
            QRgb selectedText;
        } textOnAccent;

        struct
        {
            QRgb defaultColor;
            QRgb secondary;
            QRgb tertiary;
            QRgb inputActive;
            QRgb disabled;
        } control;

        struct
        {
            QRgb defaultColor;
            QRgb disabled;
        } controlStrong;

        struct
        {
            QRgb secondary;
            QRgb tertiary;
            QRgb disabled;
        } subtle;

        struct
        {
            QRgb defaultColor;
        } controlSolid;

        struct
        {
            QRgb secondary;
            QRgb tertiary;
            QRgb quaternary;
            QRgb disabled;
        } controlAlt;

        struct
        {
            QRgb defaultColor;
            QRgb secondary;
            QRgb tertiary;
            QRgb disabled;
            QRgb selectedTextBackground;
        } accent;
    };

    struct Elevation
    {
        struct
        {
            BorderGradient border;
        } control;

        struct
        {
            BorderGradient border;
        } circle;

        struct
        {
            BorderGradient border;
            BorderGradient borderFocused;
        } textControl;

        struct
        {
            BorderGradient border;
        } accentControl;
    };

    struct StrokeColor
    {
        struct
        {
            QRgb defaultColor;
            QRgb secondary;
            QRgb onAccentDefault;
            QRgb onAccentSecondary;
            QRgb onAccentTertiary;
            QRgb onAccentDisabled;
        } control;

        struct
        {
            QRgb defaultColor;
            QRgb disabled;
        } controlStrong;

        struct
        {
            QRgb defaultColor;
            QRgb defaultSolid;
        } card;

        struct
        {
            QRgb defaultColor;
        } divider;

        struct
        {
            QRgb defaultColor;
            QRgb flyout;
        } surface;

        struct
        {
            QRgb outer;
            QRgb inner;
        } focus;
    };

    struct Background
    {
        struct
        {
            QRgb defaultColor;
            QRgb secondary;
            QRgb tertiary;
        } card;

        struct
        {
            QRgb defaultColor;
        } overlay;

        struct
        {
            QRgb alt;
        } layer;

        struct
        {
            QRgb defaultColor;
        } flyout;

        struct
        {
            QRgb base;
            QRgb secondary;
            QRgb tertiary;
            QRgb quaternary;
        } solid;
    };

    struct
    {
        FillColor fillColor;
        Elevation elevation;
        StrokeColor strokeColor;
        Background background;
    } palette;

    struct ShadowSettings
    {
        QskShadowMetrics metrics;
        QRgb color;
    };

    struct
    {
        ShadowSettings cardRest;
        ShadowSettings cardHover;
        ShadowSettings flyout;
        ShadowSettings dialog;
    } shadow;
};

class QSK_FLUENT2_EXPORT QskFluent2Skin : public QskSkin
{
    Q_OBJECT

    using Inherited = QskSkin;

  public:
    QskFluent2Skin( const QskFluent2Theme&, QObject* parent = nullptr );
      ~QskFluent2Skin() override;

    enum GraphicRole
    {
        GraphicRoleFillColorTextDisabled,
        GraphicRoleFillColorTextOnAccentDisabled,
        GraphicRoleFillColorTextOnAccentPrimary,
        GraphicRoleFillColorTextOnAccentSecondary,
        GraphicRoleFillColorTextPrimary,
        GraphicRoleFillColorTextSecondary,
    };

    enum FontRole
    {
        Caption = TinyFont,
        Body = DefaultFont,
        BodyStrong = SmallFont,
        BodyLarge = MediumFont,
        Subtitle = LargeFont,
        Title = HugeFont,
        TitleLarge,
        Display,
    };

    static constexpr QskAspect::Variation Standard = QskAspect::NoVariation;
    static constexpr QskAspect::Variation Accent = QskAspect::Large;

  private:
    void setupFonts();
    void setupGraphicFilters( const QskFluent2Theme& palette );
    void setGraphicColor( GraphicRole, QRgb );
};

#endif
