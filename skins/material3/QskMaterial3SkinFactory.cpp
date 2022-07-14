/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "QskMaterial3SkinFactory.h"
#include "QskMaterial3Skin.h"

static const QString materialLightSkinName = QStringLiteral( "Material3 Light" );
static const QString materialDarkSkinName = QStringLiteral( "Material3 Dark" );

QskMaterial3SkinFactory::QskMaterial3SkinFactory( QObject* parent )
    : QskSkinFactory( parent )
{
}

QskMaterial3SkinFactory::~QskMaterial3SkinFactory()
{
}

QStringList QskMaterial3SkinFactory::skinNames() const
{
    return { materialLightSkinName, materialDarkSkinName };
}

QskSkin* QskMaterial3SkinFactory::createSkin( const QString& skinName )
{
    if ( QString::compare( skinName, materialLightSkinName, Qt::CaseInsensitive ) == 0 )
    {
        QskMaterial3Theme theme( QskMaterial3Theme::Light );
        return new QskMaterial3Skin( theme );
    }
    else if ( QString::compare( skinName, materialDarkSkinName, Qt::CaseInsensitive ) == 0 )
    {
        QskMaterial3Theme theme( QskMaterial3Theme::Dark );
        return new QskMaterial3Skin( theme );
    }

    return nullptr;
}

#include "moc_QskMaterial3SkinFactory.cpp"
