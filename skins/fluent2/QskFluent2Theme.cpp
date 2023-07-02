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
    if( colorScheme == QskSkin::Light )
    {
        // Fill color:

        palette.fillColor.text.primary = rgbGray( 0, 0.8956 );
        palette.fillColor.text.secondary = rgbGray( 0, 0.6063 );
        palette.fillColor.text.tertiary = rgbGray( 0, 0.4458 );
        palette.fillColor.text.disabled = rgbGray( 0, 0.3614 );

        palette.fillColor.accentText.primary = accentColors[ AccentDark2 ];
        palette.fillColor.accentText.secondary = accentColors[ AccentDark3 ];
        palette.fillColor.accentText.tertiary = accentColors[ AccentDark1 ];
        palette.fillColor.accentText.disabled = rgbGray( 0, 0.3614 );

        palette.fillColor.textOnAccent.primary = rgbGray( 255 );
        palette.fillColor.textOnAccent.secondary = rgbGray( 255, 0.70 );
        palette.fillColor.textOnAccent.disabled = rgbGray( 255 );
        palette.fillColor.textOnAccent.selectedText = rgbGray( 255 );

        palette.fillColor.control.defaultColor = rgbGray( 255, 0.70 );
        palette.fillColor.control.secondary = rgbGray( 249, 0.50 );
        palette.fillColor.control.tertiary = rgbGray( 249, 0.30 );
        palette.fillColor.control.inputActive = rgbGray( 255 );
        palette.fillColor.control.disabled = rgbGray( 249, 0.30 );

        palette.fillColor.controlStrong.defaultColor = rgbGray( 0, 0.4458 );
        palette.fillColor.controlStrong.disabled = rgbGray( 0, 0.3173 );

        palette.fillColor.subtle.secondary = rgbGray( 0, 0.0373 );
        palette.fillColor.subtle.tertiary = rgbGray( 0, 0.0241 );
        palette.fillColor.subtle.disabled = Qt::transparent;

        palette.fillColor.controlSolid.defaultColor = rgbGray( 255 );

        palette.fillColor.controlAlt.secondary = rgbGray( 0, 0.0241 );
        palette.fillColor.controlAlt.tertiary = rgbGray( 0, 0.0578 );
        palette.fillColor.controlAlt.quaternary = rgbGray( 0, 0.0924 );
        palette.fillColor.controlAlt.disabled = Qt::transparent;

        palette.fillColor.accent.defaultColor = accentColors[ AccentDark1 ];
        palette.fillColor.accent.secondary = QskRgb::toTransparentF( accentColors[ AccentDark1 ], 0.90 );
        palette.fillColor.accent.tertiary = QskRgb::toTransparentF( accentColors[ AccentDark1 ], 0.80 );
        palette.fillColor.accent.disabled = rgbGray( 0, 0.2169 );
        palette.fillColor.accent.selectedTextBackground = accentColors[ AccentBase ];

        // Elevation:

        palette.elevation.control.border = { rgbGray( 0, 0.0578 ), rgbGray( 0, 0.1622 ) };
        palette.elevation.circle.border = { rgbGray( 0, 0.0578 ), rgbGray( 0, 0.1622 ) };
        palette.elevation.textControl.border = { rgbGray( 0, 0.0578 ), palette.fillColor.text.secondary };
        palette.elevation.textControl.borderFocused = { rgbGray( 0, 0.0578 ), rgbGray( 0, 0.0578 ) };
        palette.elevation.accentControl.border = { rgbGray( 255, 0.08 ), rgbGray( 0, 0.40 ) };

        // Stroke color:

        palette.strokeColor.control.defaultColor = rgbGray( 0, 0.0578 );
        palette.strokeColor.control.secondary = rgbGray( 0, 0.1622 );
        palette.strokeColor.control.onAccentDefault = rgbGray( 255.08 );
        palette.strokeColor.control.onAccentSecondary = rgbGray( 0, 0.40 );
        palette.strokeColor.control.onAccentTertiary = rgbGray( 0, 0.2169 );
        palette.strokeColor.control.onAccentDisabled = rgbGray( 0, 0.0578 );

        palette.strokeColor.controlStrong.defaultColor = rgbGray( 0, 0.4458 );
        palette.strokeColor.controlStrong.disabled = rgbGray( 0, 0.2169 );

        palette.strokeColor.card.defaultColor = rgbGray( 0, 0.0578 );
        palette.strokeColor.card.defaultSolid = rgbGray( 235 );

        palette.strokeColor.divider.defaultColor = rgbGray( 0, 0.0803 );

        palette.strokeColor.surface.defaultColor = rgbGray( 117, 0.40 );
        palette.strokeColor.surface.flyout = rgbGray( 0, 0.0578 );

        palette.strokeColor.focus.outer = rgbGray( 0, 0.8956 );
        palette.strokeColor.focus.inner = rgbGray( 255 );

        // Background:

        palette.background.card.defaultColor = rgbGray( 255, 0.70 );
        palette.background.card.secondary = rgbGray( 246, 0.50 );
        palette.background.card.tertiary = rgbGray( 255 );

        palette.background.overlay.defaultColor = rgbGray( 0, 0.30 );

        palette.background.layer.alt = rgbGray( 255 );

        palette.background.flyout.defaultColor = rgbGray( 252, 0.85 );

        palette.background.solid.base = rgbGray( 243 );
        palette.background.solid.secondary = rgbGray( 238 );
        palette.background.solid.tertiary = rgbGray( 249 );
        palette.background.solid.quaternary = rgbGray( 255 );


        // Shadow:

        shadow.cardRest = { QskShadowMetrics( 0, 4, QPointF( 0, 2 ) ), rgbGray( 0, 0.04 ) };
        shadow.cardHover = { QskShadowMetrics( 0, 4, QPointF( 0, 2 ) ), rgbGray( 0, 0.10 ) };
        shadow.flyout = { QskShadowMetrics( 0, 16, QPointF( 0, 8 ) ), rgbGray( 0, 0.14 ) };
        // ### should actually be drawn twice with different values:
        shadow.dialog = { QskShadowMetrics( 0, 21, QPointF( 0, 2 ) ), rgbGray( 0, 0.1474 ) };
    }
    else if( colorScheme == QskSkin::Dark )
    {
        // Fill color:

        palette.fillColor.text.primary = rgbGray( 255 );
        palette.fillColor.text.secondary = rgbGray( 255, 0.786 );
        palette.fillColor.text.tertiary = rgbGray( 255, 0.5442 );
        palette.fillColor.text.disabled = rgbGray( 255, 0.3628 );

        palette.fillColor.accentText.primary = accentColors[ AccentLight3 ];
        palette.fillColor.accentText.secondary = accentColors[ AccentLight3 ];
        palette.fillColor.accentText.tertiary = accentColors[ AccentLight2 ];
        palette.fillColor.accentText.disabled = rgbGray( 255, 0.3628 );

        palette.fillColor.textOnAccent.primary = rgbGray( 0 );
        palette.fillColor.textOnAccent.secondary = rgbGray( 0, 0.50 );
        palette.fillColor.textOnAccent.disabled = rgbGray( 255, 0.5302 );
        palette.fillColor.textOnAccent.selectedText = rgbGray( 255 );

        palette.fillColor.control.defaultColor = rgbGray( 255, 0.0605 );
        palette.fillColor.control.secondary = rgbGray( 255, 0.0837 );
        palette.fillColor.control.tertiary = rgbGray( 255, 0.0326 );
        palette.fillColor.control.inputActive = rgbGray( 30, 0.70 );
        palette.fillColor.control.disabled = rgbGray( 255, 0.0419 );

        palette.fillColor.controlStrong.defaultColor = rgbGray( 255, 0.5442 );
        palette.fillColor.controlStrong.disabled = rgbGray( 255, 0.2465 );

        palette.fillColor.subtle.secondary = rgbGray( 255, 0.0605 );
        palette.fillColor.subtle.tertiary = rgbGray( 255, 0.0419 );
        palette.fillColor.subtle.disabled = Qt::transparent;

        palette.fillColor.controlSolid.defaultColor = rgbGray( 69 );

        palette.fillColor.controlAlt.secondary = rgbGray( 0, 0.10 );
        palette.fillColor.controlAlt.tertiary = rgbGray( 255, 0.0419 );
        palette.fillColor.controlAlt.quaternary = rgbGray( 255, 0.0698 );
        palette.fillColor.controlAlt.disabled = Qt::transparent;

        palette.fillColor.accent.defaultColor = accentColors[ AccentLight2 ];
        palette.fillColor.accent.secondary = QskRgb::toTransparentF( accentColors[ AccentLight2 ], 0.90 );
        palette.fillColor.accent.tertiary = QskRgb::toTransparentF( accentColors[ AccentLight2 ], 0.80 );
        palette.fillColor.accent.disabled = rgbGray( 255, 0.1581 );
        palette.fillColor.accent.selectedTextBackground = accentColors[ AccentBase ];

        // Elevation:

        palette.elevation.control.border = { rgbGray( 255, 0.093 ), rgbGray( 255, 0.0698 ) };
        palette.elevation.circle.border = { rgbGray( 255, 0.093 ), rgbGray( 255, 0.0698 ) };

        palette.elevation.textControl.border = { rgbGray( 255, 0.08 ), palette.fillColor.text.secondary };

        palette.elevation.textControl.borderFocused = { rgbGray( 255, 0.08 ), rgbGray( 255, 0.08 ) };
        palette.elevation.accentControl.border = { rgbGray( 255, 0.08 ), rgbGray( 0, 0.14 ) };

        // Stroke color:

        palette.strokeColor.control.defaultColor = rgbGray( 255, 0.0698 );
        palette.strokeColor.control.secondary = rgbGray( 255, 0.093 );
        palette.strokeColor.control.onAccentDefault = rgbGray( 255, 0.08 );
        palette.strokeColor.control.onAccentSecondary = rgbGray( 0, 0.14 );
        palette.strokeColor.control.onAccentTertiary = rgbGray( 0, 0.2169 );
        palette.strokeColor.control.onAccentDisabled = rgbGray( 0, 0.20 );

        palette.strokeColor.controlStrong.defaultColor = rgbGray( 255, 0.5442 );
        palette.strokeColor.controlStrong.disabled = rgbGray( 255, 0.1581 );

        palette.strokeColor.card.defaultColor = rgbGray( 255, 0.0578 );
        palette.strokeColor.card.defaultSolid = rgbGray( 235 );

        palette.strokeColor.divider.defaultColor = rgbGray( 255, 0.0837 );

        palette.strokeColor.surface.defaultColor = rgbGray( 117, 0.40 );
        palette.strokeColor.surface.flyout = rgbGray( 0, 0.20 );

        palette.strokeColor.focus.outer = rgbGray( 255 );
        palette.strokeColor.focus.inner = rgbGray( 0, 0.70 );

        // Background:

        palette.background.card.defaultColor = rgbGray( 255, 0.0512 );
        palette.background.card.secondary = rgbGray( 255, 0.0326 );
        palette.background.card.tertiary = rgbGray( 255 ); // not set in Figma

        palette.background.overlay.defaultColor = rgbGray( 0, 0.30 );

        palette.background.layer.alt = rgbGray( 255, 0.0538 );

        palette.background.flyout.defaultColor = rgbGray( 44, 0.96 );

        palette.background.solid.base = rgbGray( 32 );
        palette.background.solid.secondary = rgbGray( 28 );
        palette.background.solid.tertiary = rgbGray( 40 );
        palette.background.solid.quaternary = rgbGray( 44 );


        // Shadow:

        shadow.cardRest = { QskShadowMetrics( 0, 4, QPointF( 0, 2 ) ), rgbGray( 0, 0.13 ) };
        shadow.cardHover = { QskShadowMetrics( 0, 4, QPointF( 0, 2 ) ), rgbGray( 0, 0.26 ) };
        shadow.flyout = { QskShadowMetrics( 0, 16, QPointF( 0, 8 ) ), rgbGray( 0, 0.26 ) };
        // ### should actually be drawn twice with different values:
        shadow.dialog = { QskShadowMetrics( 0, 21, QPointF( 0, 2 ) ), rgbGray( 0, 0.37 ) };
    }
}
