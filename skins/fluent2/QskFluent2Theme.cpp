/******************************************************************************
 * QSkinny - Copyright (C) 2023 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFluent2Theme.h"
#include <QskRgbValue.h>

namespace
{
    inline constexpr QRgb rgbGray( int value, qreal opacity = 1.0 )
    {
        return qRgba( value, value, value, qRound( opacity * 255 ) );
    }
}

QskFluent2Theme::QskFluent2Theme( QskSkin::ColorScheme colorScheme )
    : QskFluent2Theme( colorScheme,
                        { // default Fluent accent colors:
                          0xff98ecfe,
                          0xff60ccfe,
                          0xff0093f9,
                          0xff0078d4,
                          0xff005eb7,
                          0xff003d92,
                          0xff001968
                        } )
{
}

QskFluent2Theme::QskFluent2Theme( QskSkin::ColorScheme colorScheme,
    const std::array< QRgb, NumAccentColors >& accentColors )
{
    using namespace QskRgb;

    if( colorScheme != QskSkin::DarkScheme )
    {
        {
            auto& colors = palette.fillColor;

            colors.text.primary = rgbGray( 0, 0.8956 );
            colors.text.secondary = rgbGray( 0, 0.6063 );
            colors.text.tertiary = rgbGray( 0, 0.4458 );
            colors.text.disabled = rgbGray( 0, 0.3614 );

            colors.accentText.primary = accentColors[ AccentDark2 ];
            colors.accentText.secondary = accentColors[ AccentDark3 ];
            colors.accentText.tertiary = accentColors[ AccentDark1 ];
            colors.accentText.disabled = rgbGray( 0, 0.3614 );

            colors.textOnAccent.primary = rgbGray( 255 );
            colors.textOnAccent.secondary = rgbGray( 255, 0.70 );
            colors.textOnAccent.disabled = rgbGray( 255 );
            colors.textOnAccent.selectedText = rgbGray( 255 );

            colors.control.defaultColor = rgbGray( 255, 0.70 );
            colors.control.secondary = rgbGray( 249, 0.50 );
            colors.control.tertiary = rgbGray( 249, 0.30 );
            colors.control.inputActive = rgbGray( 255 );
            colors.control.disabled = rgbGray( 249, 0.30 );

            colors.controlStrong.defaultColor = rgbGray( 0, 0.4458 );
            colors.controlStrong.disabled = rgbGray( 0, 0.3173 );

            colors.subtle.secondary = rgbGray( 0, 0.0373 );
            colors.subtle.tertiary = rgbGray( 0, 0.0241 );
            colors.subtle.disabled = Qt::transparent;

            colors.controlSolid.defaultColor = rgbGray( 255 );

            colors.controlAlt.secondary = rgbGray( 0, 0.0241 );
            colors.controlAlt.tertiary = rgbGray( 0, 0.0578 );
            colors.controlAlt.quaternary = rgbGray( 0, 0.0924 );
            colors.controlAlt.disabled = Qt::transparent;

            colors.accent.defaultColor = accentColors[ AccentDark1 ];
            colors.accent.secondary = toTransparentF( accentColors[ AccentDark1 ], 0.90 );
            colors.accent.tertiary = toTransparentF( accentColors[ AccentDark1 ], 0.80 );
            colors.accent.disabled = rgbGray( 0, 0.2169 );
            colors.accent.selectedTextBackground = accentColors[ AccentBase ];
        }

        {
            auto& colors = palette.elevation;

            colors.control.border = { rgbGray( 0, 0.0578 ), rgbGray( 0, 0.1622 ) };
            colors.circle.border = { rgbGray( 0, 0.0578 ), rgbGray( 0, 0.1622 ) };
            colors.textControl.border = { rgbGray( 0, 0.0578 ), palette.fillColor.text.secondary };
            colors.textControl.borderFocused = { rgbGray( 0, 0.0578 ), rgbGray( 0, 0.0578 ) };
            colors.accentControl.border = { rgbGray( 255, 0.08 ), rgbGray( 0, 0.40 ) };
        }

        {
            auto& colors = palette.strokeColor;

            colors.control.defaultColor = rgbGray( 0, 0.0578 );
            colors.control.secondary = rgbGray( 0, 0.1622 );
            colors.control.onAccentDefault = rgbGray( 255.08 );
            colors.control.onAccentSecondary = rgbGray( 0, 0.40 );
            colors.control.onAccentTertiary = rgbGray( 0, 0.2169 );
            colors.control.onAccentDisabled = rgbGray( 0, 0.0578 );

            colors.controlStrong.defaultColor = rgbGray( 0, 0.4458 );
            colors.controlStrong.disabled = rgbGray( 0, 0.2169 );

            colors.card.defaultColor = rgbGray( 0, 0.0578 );
            colors.card.defaultSolid = rgbGray( 235 );

            colors.divider.defaultColor = rgbGray( 0, 0.0803 );

            colors.surface.defaultColor = rgbGray( 117, 0.40 );
            colors.surface.flyout = rgbGray( 0, 0.0578 );

            colors.focus.outer = rgbGray( 0, 0.8956 );
            colors.focus.inner = rgbGray( 255 );
        }

        {
            auto& colors = palette.background;

            colors.card.defaultColor = rgbGray( 255, 0.70 );
            colors.card.secondary = rgbGray( 246, 0.50 );
            colors.card.tertiary = rgbGray( 255 );

            colors.overlay.defaultColor = rgbGray( 0, 0.30 );
            colors.layer.alt = rgbGray( 255 );
            colors.flyout.defaultColor = rgbGray( 252, 0.85 );

            colors.solid.base = rgbGray( 243 );
            colors.solid.secondary = rgbGray( 238 );
            colors.solid.tertiary = rgbGray( 249 );
            colors.solid.quaternary = rgbGray( 255 );
        }

        // Shadow:

        shadow.cardRest = { QskShadowMetrics( 0, 4, QPointF( 0, 2 ) ), rgbGray( 0, 0.04 ) };
        shadow.cardHover = { QskShadowMetrics( 0, 4, QPointF( 0, 2 ) ), rgbGray( 0, 0.10 ) };
        shadow.flyout = { QskShadowMetrics( 0, 16, QPointF( 0, 8 ) ), rgbGray( 0, 0.14 ) };
        // ### should actually be drawn twice with different values:
        shadow.dialog = { QskShadowMetrics( 0, 21, QPointF( 0, 2 ) ), rgbGray( 0, 0.1474 ) };
    }
    else
    {
        {
            auto& colors = palette.fillColor;

            colors.text.primary = rgbGray( 255 );
            colors.text.secondary = rgbGray( 255, 0.786 );
            colors.text.tertiary = rgbGray( 255, 0.5442 );
            colors.text.disabled = rgbGray( 255, 0.3628 );

            colors.accentText.primary = accentColors[ AccentLight3 ];
            colors.accentText.secondary = accentColors[ AccentLight3 ];
            colors.accentText.tertiary = accentColors[ AccentLight2 ];
            colors.accentText.disabled = rgbGray( 255, 0.3628 );

            colors.textOnAccent.primary = rgbGray( 0 );
            colors.textOnAccent.secondary = rgbGray( 0, 0.50 );
            colors.textOnAccent.disabled = rgbGray( 255, 0.5302 );
            colors.textOnAccent.selectedText = rgbGray( 255 );

            colors.control.defaultColor = rgbGray( 255, 0.0605 );
            colors.control.secondary = rgbGray( 255, 0.0837 );
            colors.control.tertiary = rgbGray( 255, 0.0326 );
            colors.control.inputActive = rgbGray( 30, 0.70 );
            colors.control.disabled = rgbGray( 255, 0.0419 );

            colors.controlStrong.defaultColor = rgbGray( 255, 0.5442 );
            colors.controlStrong.disabled = rgbGray( 255, 0.2465 );

            colors.subtle.secondary = rgbGray( 255, 0.0605 );
            colors.subtle.tertiary = rgbGray( 255, 0.0419 );
            colors.subtle.disabled = Qt::transparent;

            colors.controlSolid.defaultColor = rgbGray( 69 );

            colors.controlAlt.secondary = rgbGray( 0, 0.10 );
            colors.controlAlt.tertiary = rgbGray( 255, 0.0419 );
            colors.controlAlt.quaternary = rgbGray( 255, 0.0698 );
            colors.controlAlt.disabled = Qt::transparent;

            colors.accent.defaultColor = accentColors[ AccentLight2 ];
            colors.accent.secondary = toTransparentF( accentColors[ AccentLight2 ], 0.90 );
            colors.accent.tertiary = toTransparentF( accentColors[ AccentLight2 ], 0.80 );
            colors.accent.disabled = rgbGray( 255, 0.1581 );
            colors.accent.selectedTextBackground = accentColors[ AccentBase ];
        }

        {
            auto& colors = palette.elevation;

            colors.control.border = { rgbGray( 255, 0.093 ), rgbGray( 255, 0.0698 ) };
            colors.circle.border = { rgbGray( 255, 0.093 ), rgbGray( 255, 0.0698 ) };

            colors.textControl.border = { rgbGray( 255, 0.08 ), palette.fillColor.text.secondary };

            colors.textControl.borderFocused = { rgbGray( 255, 0.08 ), rgbGray( 255, 0.08 ) };
            colors.accentControl.border = { rgbGray( 255, 0.08 ), rgbGray( 0, 0.14 ) };
        }

        {
            auto& colors = palette.strokeColor;

            colors.control.defaultColor = rgbGray( 255, 0.0698 );
            colors.control.secondary = rgbGray( 255, 0.093 );
            colors.control.onAccentDefault = rgbGray( 255, 0.08 );
            colors.control.onAccentSecondary = rgbGray( 0, 0.14 );
            colors.control.onAccentTertiary = rgbGray( 0, 0.2169 );
            colors.control.onAccentDisabled = rgbGray( 0, 0.20 );

            colors.controlStrong.defaultColor = rgbGray( 255, 0.5442 );
            colors.controlStrong.disabled = rgbGray( 255, 0.1581 );

            colors.card.defaultColor = rgbGray( 255, 0.0578 );
            colors.card.defaultSolid = rgbGray( 235 );

            colors.divider.defaultColor = rgbGray( 255, 0.0837 );

            colors.surface.defaultColor = rgbGray( 117, 0.40 );
            colors.surface.flyout = rgbGray( 0, 0.20 );

            colors.focus.outer = rgbGray( 255 );
            colors.focus.inner = rgbGray( 0, 0.70 );
        }

        {
            auto& colors = palette.background;

            colors.card.defaultColor = rgbGray( 255, 0.0512 );
            colors.card.secondary = rgbGray( 255, 0.0326 );
            colors.card.tertiary = rgbGray( 255 ); // not set in Figma

            colors.overlay.defaultColor = rgbGray( 0, 0.30 );
            colors.layer.alt = rgbGray( 255, 0.0538 );
            colors.flyout.defaultColor = rgbGray( 44, 0.96 );

            colors.solid.base = rgbGray( 32 );
            colors.solid.secondary = rgbGray( 28 );
            colors.solid.tertiary = rgbGray( 40 );
            colors.solid.quaternary = rgbGray( 44 );
        }

        // Shadow:

        shadow.cardRest = { QskShadowMetrics( 0, 4, QPointF( 0, 2 ) ), rgbGray( 0, 0.13 ) };
        shadow.cardHover = { QskShadowMetrics( 0, 4, QPointF( 0, 2 ) ), rgbGray( 0, 0.26 ) };
        shadow.flyout = { QskShadowMetrics( 0, 16, QPointF( 0, 8 ) ), rgbGray( 0, 0.26 ) };
        // ### should actually be drawn twice with different values:
        shadow.dialog = { QskShadowMetrics( 0, 21, QPointF( 0, 2 ) ), rgbGray( 0, 0.37 ) };
    }
}
