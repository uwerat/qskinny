/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "QskMaterialSkinFactory.h"
#include "QskMaterialSkin.h"

static const QString materialLightSkinName = QStringLiteral( "materialLight" );
static const QString materialDarkSkinName = QStringLiteral( "materialDark" );

namespace
{
    inline int lightnessRgb( QRgb rgb )
    {
        const int red = qRed( rgb );
        const int green = qGreen( rgb );
        const int blue = qBlue( rgb );

        int min, max;

        if ( red > green )
        {
            max = qMax( red, blue );
            min = qMin( green, blue );
        }
        else
        {
            max = qMax( green, blue );
            min = qMin( red, blue );
        }

        return ( max + min ) / 2;
    }

    inline QRgb toUnsaturated( QRgb rgb )
    {
        /*
            a saturation of 0 results in having the lightness as r,g,b
            Is this intended ?
         */

        const auto l = lightnessRgb( rgb );
        return qRgba( l, l, l, qAlpha( rgb ) );
    }
}

QskMaterialSkinFactory::QskMaterialSkinFactory( QObject* parent )
    : QskSkinFactory( parent )
{
}

QskMaterialSkinFactory::~QskMaterialSkinFactory()
{
}

QStringList QskMaterialSkinFactory::skinNames() const
{
    return { materialLightSkinName, materialDarkSkinName };
}

QskSkin* QskMaterialSkinFactory::createSkin( const QString& skinName )
{
    if ( QString::compare( skinName, materialLightSkinName, Qt::CaseInsensitive ) == 0 )
    {
        QskMaterialPalette pal( QskMaterialPalette::Light );;

        pal.primary = 0xff6200ee;
        pal.primaryVariant = 0xff3700b3;
        pal.onPrimary = QskRgb::White;
        pal.secondary = 0xff03dac6;
        pal.secondaryVariant = 0xff018786;
        pal.onSecondary = QskRgb::White;
        pal.background = QskRgb::DefaultMaterialNeutral90;
        pal.onBackground = QskRgb::Black;
        pal.error = 0xffb00020;
        pal.onError = QskRgb::White;

        pal.primaryNoSaturation = toUnsaturated( pal.primary );
        pal.secondaryNoSaturation = toUnsaturated( pal.secondary );
        pal.secondaryVariantNoSaturation = toUnsaturated( pal.secondaryVariant );

        return new QskMaterialSkin( pal );
    }

    if ( QString::compare( skinName, materialDarkSkinName, Qt::CaseInsensitive ) == 0 )
    {
        QskMaterialPalette pal( QskMaterialPalette::Dark );

        pal.primary = 0xffbb86fc;
        pal.primaryVariant = 0xff3700b3;
        pal.onPrimary = QskRgb::Black;
        pal.secondary = 0xff03dac6;
        pal.secondaryVariant = 0xff018786;
        pal.onSecondary = QskRgb::Black;
        pal.background = 0xff121212;
        pal.onBackground = QskRgb::White;
        pal.error = 0xffcf6679;
        pal.onError = QskRgb::Black;

        pal.primaryNoSaturation = toUnsaturated( pal.primary );
        pal.secondaryNoSaturation = toUnsaturated( pal.secondary );
        pal.secondaryVariantNoSaturation = toUnsaturated( pal.secondaryVariant );

        return new QskMaterialSkin( pal );
    }

    return nullptr;
}

#include "moc_QskMaterialSkinFactory.cpp"
