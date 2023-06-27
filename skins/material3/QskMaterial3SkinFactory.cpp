/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
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
        QskMaterial3Theme theme( QskSkin::Light );
        return new QskMaterial3Skin( theme );
    }
    else if ( QString::compare( skinName, materialDarkSkinName, Qt::CaseInsensitive ) == 0 )
    {
        QskMaterial3Theme theme( QskSkin::Dark );
        return new QskMaterial3Skin( theme );
    }

    return nullptr;
}

#include "moc_QskMaterial3SkinFactory.cpp"
