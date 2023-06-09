/******************************************************************************
 * QSkinny - Copyright (C) 2023 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_WINDOWS_SKIN_H
#define QSK_WINDOWS_SKIN_H

#include "QskWindowsGlobal.h"

#include <QskBoxShapeMetrics.h>
#include <QskGradient.h>
#include <QskShadowMetrics.h>
#include <QskSkin.h>

#include <array>

class QSK_WINDOWS_EXPORT QskWindowsTheme
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

    QskWindowsTheme( Theme );
    QskWindowsTheme( Theme, std::array< QRgb, NumAccentColors > );

    typedef std::array< QRgb, 2 > BorderGradient;

    struct FillColor
    {
        struct Text
        {
            QRgb primary;
            QRgb secondary;
            QRgb tertiary;
            QRgb disabled;
        };

        struct AccentText
        {
            QRgb primary;
            QRgb secondary;
            QRgb tertiary;
            QRgb disabled;
        };

        struct TextOnAccent
        {
            QRgb primary;
            QRgb secondary;
            QRgb tertiary;
            QRgb disabled;
            QRgb selectedText;
        };

        struct Control
        {
            QRgb transparent;
            QRgb defaultColor;
            QRgb secondary;
            QRgb tertiary;
            QRgb inputActive;
            QRgb disabled;
        };

        struct ControlStrong
        {
            QRgb defaultColor;
            QRgb disabled;
        };

        struct Subtle
        {
            QRgb transparent;
            QRgb secondary;
            QRgb tertiary;
            QRgb disabled;
        };

        struct ControlSolid
        {
            QRgb defaultColor;
        };

        struct ControlAlt
        {
            QRgb transparent;
            QRgb secondary;
            QRgb tertiary;
            QRgb quaternary;
            QRgb disabled;
        };

        struct Accent
        {
            QRgb defaultColor;
            QRgb secondary;
            QRgb tertiary;
            QRgb disabled;
            QRgb selectedTextBackground;
        };

        struct System
        {
            QRgb critical;
            QRgb success;
            QRgb attention;
            QRgb caution;
            QRgb attentionBackground;
            QRgb successBackground;
            QRgb cautionBackground;
            QRgb criticalBackground;
            QRgb neutral;
            QRgb neutralBackground;
            QRgb solidNeutral;
            QRgb solidAttentionBackground;
            QRgb solidNeutralBackground;
        };

        struct ControlOnImage
        {
            QRgb defaultColor;
            QRgb secondary;
            QRgb tertiary;
            QRgb disabled;
        };

        Text text;
        AccentText accentText;
        TextOnAccent textOnAccent;
        Control control;
        ControlStrong controlStrong;
        Subtle subtle;
        ControlSolid controlSolid;
        ControlAlt controlAlt;
        Accent accent;
        System system;
        ControlOnImage controlOnImage;
    };

    struct Elevation
    {
        struct Control
        {
            BorderGradient border;
        };

        struct Circle
        {
            BorderGradient border;
        };

        struct TextControl
        {
            BorderGradient border;
            BorderGradient borderFocused;
        };

        struct AccentControl
        {
            BorderGradient border;
        };

        Control control;
        Circle circle;
        TextControl textControl;
        AccentControl accentControl;
    };

    struct StrokeColor
    {
        struct ControlStroke
        {
            QRgb defaultColor;
            QRgb secondary;
            QRgb onAccentDefault;
            QRgb onAccentSecondary;
            QRgb onAccentTertiary;
            QRgb onAccentDisabled;
            QRgb forStrongFillWhenOnImage;
        };

        struct ControlStrongStroke
        {
            QRgb defaultColor;
            QRgb disabled;
        };

        struct CardStroke
        {
            QRgb defaultColor;
            QRgb defaultSolid;
        };

        struct DividerStroke
        {
            QRgb defaultColor;
        };

        struct SurfaceStroke
        {
            QRgb defaultColor;
            QRgb flyout;
        };

        struct FocusStroke
        {
            QRgb outer;
            QRgb inner;
        };

        ControlStroke controlStroke;
        ControlStrongStroke controlStrongStroke;
        CardStroke cardStroke;
        DividerStroke dividerStroke;
        SurfaceStroke surfaceStroke;
        FocusStroke focusStroke;
    };

    struct Background
    {
        struct FillColor
        {
            struct CardBackground
            {
                QRgb defaultColor;
                QRgb secondary;
                QRgb tertiary;
            };

            struct Smoke
            {
                QRgb defaultColor;
            };

            struct Layer
            {
                QRgb defaultColor;
                QRgb alt;
            };

            struct LayerOnAcrylic
            {
                QRgb defaultColor;
            };

            struct LayerOnAccentAcrylic
            {
                QRgb defaultColor;
            };

            struct AcrylicBackground
            {
                QRgb defaultColor;
                QRgb base;
            };

            struct AccentAcrylicBackground
            {
                QRgb base;
                QRgb defaultColor;
            };

            struct MicaBackground
            {
                QRgb base;
            };

            struct SolidBackground
            {
                QRgb base;
                QRgb secondary;
                QRgb tertiary;
                QRgb quaternary;
            };

            CardBackground cardBackground;
            Smoke smoke;
            Layer layer;
            LayerOnAcrylic layerOnAcrylic;
            LayerOnAccentAcrylic layerOnAccentAcrylic;
            AcrylicBackground acrylicBackground;
            AccentAcrylicBackground accentAcrylicBackground;
            MicaBackground micaBackground;
            SolidBackground solidBackground;
        };

        FillColor fillColor;
    };


    struct Palette
    {
        FillColor fillColor;
        Elevation elevation;
        StrokeColor strokeColor;
        Background background;
    };

    Palette palette;

    typedef QPair< QskShadowMetrics, QRgb > ShadowSettings;

    struct Shadow
    {
        ShadowSettings cardRest;
        ShadowSettings cardHover;
        ShadowSettings tooltip;
        ShadowSettings flyout;
        ShadowSettings dialog;
    };

    Shadow shadow;
};

class QSK_WINDOWS_EXPORT QskWindowsSkin : public QskSkin
{
    Q_OBJECT

    using Inherited = QskSkin;

  public:
    QskWindowsSkin( const QskWindowsTheme&, QObject* parent = nullptr );
    ~QskWindowsSkin() override;

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
    void setupGraphicFilters( const QskWindowsTheme& palette );
    void setGraphicColor( GraphicRole, QRgb );
};

#endif
