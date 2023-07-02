/******************************************************************************
 * QSkinny - Copyright (C) 2023 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFluent2SkinFactory.h"
#include "QskFluent2Skin.h"
#include "QskFluent2Theme.h"

static const QString fluent2LightSkinName = QStringLiteral( "Fluent2 Light" );
static const QString fluent2DarkSkinName = QStringLiteral( "Fluent2 Dark" );

QskFluent2SkinFactory::QskFluent2SkinFactory( QObject* parent )
    : QskSkinFactory( parent )
{
}

QskFluent2SkinFactory::~QskFluent2SkinFactory()
{
}

QStringList QskFluent2SkinFactory::skinNames() const
{
    return { fluent2LightSkinName, fluent2DarkSkinName };
}

QskSkin* QskFluent2SkinFactory::createSkin( const QString& skinName )
{
    if ( QString::compare( skinName, fluent2LightSkinName, Qt::CaseInsensitive ) == 0 )
    {
        QskFluent2Theme theme( QskSkin::Light );
        return new QskFluent2Skin( theme );
    }
    else if ( QString::compare( skinName, fluent2DarkSkinName, Qt::CaseInsensitive ) == 0 )
    {
        QskFluent2Theme theme( QskSkin::Dark );
        return new QskFluent2Skin( theme );
    }

    return nullptr;
}

#include "moc_QskFluent2SkinFactory.cpp"
