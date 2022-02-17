/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "QskMaterialSkinFactory.h"
#include "QskMaterialSkin.h"

static const QString materialLightSkinName = QStringLiteral( "materialLight" );
static const QString materialDarkSkinName = QStringLiteral( "materialDark" );

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
    if ( QString::compare( skinName, materialLightSkinName, Qt::CaseInsensitive ) )
        return new QskMaterialSkin( ColorPalette() );

    if ( QString::compare( skinName, materialDarkSkinName, Qt::CaseInsensitive ) )
    {
        return new QskMaterialSkin( ColorPalette(
            ColorPalette::dark,            // lightness
            QColor::fromRgb( 0xBB86FC ),   // primary
            QColor::fromRgb( 0x3700B3 ),   // primaryVariant
            Qt::black,                     // onPrimary
            QColor::fromRgb( 0x03DAC6 ),   // secondary
            QColor::fromRgb( 0x018786 ),   // secondaryVariant
            Qt::black,                     // onSecondary
            QColor::fromRgb( 0x121212 ),   // background
            Qt::white,                     // onBackground
            QColor::fromRgb( 0xCF6679 ),   // error
            Qt::black                      // onError
            ) );
    }

    return nullptr;
}

#include "moc_QskMaterialSkinFactory.cpp"
