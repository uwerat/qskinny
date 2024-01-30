/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFusionPalette.h"
#include <QskRgbValue.h>

#if 0
extern QPalette qt_fusionPalette();
#endif

namespace
{
    inline constexpr QRgb rgbGray( int value )
    {
        return qRgb( value, value, value );
    }
}

QskFusionPalette::QskFusionPalette( QskSkin::ColorScheme colorScheme )
{
    using namespace QskRgb;
    using P = QPalette;

    struct
    {
        QRgb base, background, text, disabledText, light, midLight, mid, dark,
             darkDisabled, highlight, disabledHighlight, shadow, disabledShadow,
             hightlightedText, link;
    } colors;

    if ( colorScheme == QskSkin::DarkScheme )
    {
        colors.base              = rgbGray( 36 );
        colors.background        = rgbGray( 50 );
        colors.text              = rgbGray( 240 );
        colors.disabledText      = rgbGray( 130 );
        colors.light             = rgbGray( 75 );
        colors.midLight          = rgbGray( 42 );
        colors.mid               = rgbGray( 38 );
        colors.dark              = rgbGray( 33 );
        colors.darkDisabled      = rgbGray( 190 );
        colors.highlight         = qRgb( 48, 140, 198 );
        colors.disabledHighlight = rgbGray( 145 );
        colors.shadow            = rgbGray( 25 );
        colors.disabledShadow    = rgbGray( 36 );
        colors.hightlightedText  = colors.text;
        colors.link              = colors.highlight;
    }
    else
    {
        colors.base              = White;
        colors.background        = rgbGray( 239 );
        colors.text              = Black;
        colors.disabledText      = rgbGray( 190 );
        colors.light             = White;
        colors.midLight          = rgbGray( 202 );
        colors.mid               = rgbGray( 184 );
        colors.dark              = rgbGray( 159 );
        colors.darkDisabled      = rgbGray( 190 );
        colors.highlight         = qRgb( 48, 140, 198 );
        colors.disabledHighlight = rgbGray( 145 );
        colors.shadow            = rgbGray( 118 );
        colors.disabledShadow    = rgbGray( 177 );
        colors.hightlightedText  = White;
        colors.link              = Blue;
    }

    setColor( P::WindowText, colors.text );
    setColor( P::Button, colors.background );

    setColor( P::Light, colors.light );
    setColor( P::Midlight, colors.midLight );
    setColor( P::Dark, colors.dark );
    setColor( P::Mid, colors.mid );

    setColor( P::Text, colors.text );
    setColor( P::BrightText, colors.light );
    setColor( P::ButtonText, colors.text );

    setColor( P::Base, colors.base );
    setColor( P::Window, colors.background );
    setColor( P::Shadow, colors.shadow );
    setColor( P::AlternateBase, interpolated( colors.base, colors.background, 0.5 ) );

    setColor( P::Highlight, colors.highlight );
    setColor( P::HighlightedText, colors.hightlightedText );

    setColor( P::Link, colors.link);
    setColor( P::LinkVisited, Magenta );

    setColor( P::ToolTipBase, qRgb( 255, 255, 220 ) );
    setColor( P::ToolTipText, Black );

    setColor( P::PlaceholderText, toTransparent( colors.text, 128 ) );

    // disabled colors

    setColor( P::Disabled, P::Text, colors.disabledText );
    setColor( P::Disabled, P::WindowText, colors.disabledText );
    setColor( P::Disabled, P::ButtonText, colors.disabledText );

    setColor( P::Disabled, P::Base, colors.background );
    setColor( P::Disabled, P::Dark, colors.darkDisabled );
    setColor( P::Disabled, P::Shadow, colors.disabledShadow );

    setColor( P::Disabled, P::Highlight, colors.disabledHighlight );

#if QT_VERSION >= QT_VERSION_CHECK( 6, 6, 0 )
    setColor( P::Active, P::Accent, active( P::Highlight ) );
    setColor( P::Disabled, P::Accent, disabled( P::Highlight ) );
#endif

    initExtraColors();
}

QskFusionPalette::QskFusionPalette( const QPalette& palette )
{
    using P = QPalette;

    for ( int i = 0; i < 2; i++ )
    {
        const auto group = static_cast< P::ColorGroup >( i );

        for ( int j = 0; j < P::NColorRoles; j++ )
        {
            const auto role = static_cast< P::ColorRole >( j );
            m_colors[ group ][ role ] = palette.color( group, role ).rgba();
        }
    }

    initExtraColors();
}

QskFusionPalette::~QskFusionPalette()
{
}

void QskFusionPalette::initExtraColors()
{
    using P = QPalette;

    outline = QskRgb::darker( active( P::Window ), 140 );

    button = active( P::Button );

    {
        auto c = QColor::fromRgba( button );
        const int val = qGray( button );
        c = c.lighter( 100 + qMax( 1, ( 180 - val ) / 6 ) );
        c.setHsv(c.hue(), c.saturation() * 0.75, c.value() );

        button = c.rgba();
    }

    {
        auto c = QColor::fromRgba( button ).toHsv();
        c.setHsv( c.hue(), qMin( 255, (int)( c.saturation() ) ),
            qMin( 255, (int)( c.value() * 0.9 ) ) );

        groove = c.rgba();
    }

    tabFrame = QskRgb::lighter( button, 104 );

    highlightedOutline = active( QPalette::Highlight );

    {
        highlightedOutline = QskRgb::darker( highlightedOutline, 125 );

        auto c = QColor::fromRgba( highlightedOutline );
        if ( c.value() > 160 )
        {
            c.setHsl(c.hue(), c.saturation(), 160 );
            highlightedOutline = c.rgba();
        }
    }

    error = QskRgb::DarkRed; // ????
}
