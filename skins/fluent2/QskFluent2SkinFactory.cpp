/******************************************************************************
 * QSkinny - Copyright (C) 2023 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFluent2SkinFactory.h"
#include "QskFluent2Skin.h"
#include "QskFluent2Theme.h"

static const QString nameLight = QStringLiteral( "Fluent2 Light" );
static const QString nameDark = QStringLiteral( "Fluent2 Dark" );

namespace
{
    inline constexpr QRgb rgbGray( int value )
    {
        return qRgba( value, value, value, 255 );
    } 
}

QskFluent2SkinFactory::QskFluent2SkinFactory( QObject* parent )
    : QskSkinFactory( parent )
{
}

QskFluent2SkinFactory::~QskFluent2SkinFactory()
{
}

QStringList QskFluent2SkinFactory::skinNames() const
{
    return { nameLight, nameDark };
}

QskSkin* QskFluent2SkinFactory::createSkin( const QString& skinName )
{
    QskSkin::ColorScheme colorScheme;

    if ( QString::compare( skinName, nameLight, Qt::CaseInsensitive ) == 0 )
    {
        colorScheme = QskSkin::LightScheme;
    }
    else if ( QString::compare( skinName, nameDark, Qt::CaseInsensitive ) == 0 )
    {
        colorScheme = QskSkin::DarkScheme;
    }
    else
    {
        return nullptr;
    }

    struct
    {
        QskSkin::ColorScheme scheme;
        QskFluent2Theme::BaseColors baseColors;
        QskFluent2Theme::AccentColors accentColors;

        QskFluent2Theme theme() const { return { scheme, baseColors, accentColors }; }
    } colors[2];

    switch( colorScheme )
    {
        case QskSkin::LightScheme:
        {
            colors[0].scheme = colorScheme;
            colors[0].baseColors = { rgbGray( 243 ), rgbGray( 249 ), rgbGray( 238 ) };
            colors[0].accentColors = { 0xff0078d4, 0xff005eb7, 0xff003d92, 0xff001968 };

            colors[1].scheme = colorScheme;
            colors[1].baseColors = { rgbGray( 249 ), rgbGray( 249 ), rgbGray( 238 ) };
            colors[1].accentColors = colors[0].accentColors;

            break;
        }
        case QskSkin::DarkScheme:
        {
            colors[0].scheme = colorScheme;
            colors[0].baseColors = { rgbGray( 32 ), rgbGray( 40 ), rgbGray( 28 ) };
            colors[0].accentColors = { 0xff0078d4, 0xff0093f9, 0xff60ccfe, 0xff98ecfe };

            colors[1].scheme = colorScheme;
            colors[1].baseColors = { rgbGray( 40 ), rgbGray( 44 ), rgbGray( 28 ) };
            colors[1].accentColors = colors[0].accentColors;

            break;
        }
        default:;
    }

    auto skin = new QskFluent2Skin();

    skin->addTheme( QskAspect::Body, colors[0].theme() );
    skin->addTheme( QskAspect::Header, colors[1].theme() );
    skin->addTheme( QskAspect::Footer, colors[1].theme() );

    return skin;
}

#include "moc_QskFluent2SkinFactory.cpp"
