/******************************************************************************
 * QSkinny - Copyright (C) 2023 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FLUENT2_THEME_H
#define QSK_FLUENT2_THEME_H

#include "QskFluent2Global.h"

#include <QskSkin.h>
#include <QskShadowMetrics.h>

#include <qcolor.h>
#include <array>

class QSK_FLUENT2_EXPORT QskFluent2Theme
{
  public:
    struct BaseColors
    {
        QRgb primary;
        QRgb secondary;
        QRgb tertiary;
    };

    struct AccentColors
    {
        QRgb primary;
        QRgb secondary;
        QRgb tertiary;
        QRgb quaternary;
    };

    QskFluent2Theme( QskSkin::ColorScheme, const BaseColors& baseColors,
        const AccentColors& accentColors );

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

        struct
        {
            QRgb background;
        } acrylic;
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
        ShadowSettings flyout;
        ShadowSettings dialog;
    } shadow;
};

#endif
