/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "QskMaterialSkinFactory.h"
#include "QskMaterialSkin.h"

static const QString materialSkinName = QStringLiteral( "material" );

QskMaterialSkinFactory::QskMaterialSkinFactory( QObject* parent )
    : QskSkinFactory( parent )
{
}

QskMaterialSkinFactory::~QskMaterialSkinFactory()
{
}

QStringList QskMaterialSkinFactory::skinNames() const
{
    return { materialSkinName };
}

QskSkin* QskMaterialSkinFactory::createSkin( const QString& skinName )
{
    if ( skinName.toLower() == materialSkinName )
        return new QskMaterialSkin();

    return nullptr;
}

#include "moc_QskMaterialSkinFactory.cpp"
