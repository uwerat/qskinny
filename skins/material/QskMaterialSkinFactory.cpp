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
    if ( QString::compare( skinName, materialLightSkinName, Qt::CaseInsensitive ) == 0 )
    {
        QskMaterialTheme theme( QskMaterialTheme::Light );
        return new QskMaterialSkin( theme );
    }
    else if ( QString::compare( skinName, materialDarkSkinName, Qt::CaseInsensitive ) == 0 )
    {
        QskMaterialTheme theme( QskMaterialTheme::Dark );
        return new QskMaterialSkin( theme );
    }

    return nullptr;
}

#include "moc_QskMaterialSkinFactory.cpp"
