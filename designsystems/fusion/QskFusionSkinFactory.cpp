/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFusionSkinFactory.h"
#include "QskFusionSkin.h"

static const QStringList fusionSkinNames =
    { QStringLiteral( "Fusion Light" ), QStringLiteral( "Fusion Dark" ) };

QskFusionSkinFactory::QskFusionSkinFactory( QObject* parent )
    : QskSkinFactory( parent )
{
}

QskFusionSkinFactory::~QskFusionSkinFactory()
{
}

QStringList QskFusionSkinFactory::skinNames() const
{
    return fusionSkinNames;
}

QskSkin* QskFusionSkinFactory::createSkin( const QString& skinName )
{
    if ( QString::compare( skinName, fusionSkinNames[0], Qt::CaseInsensitive ) == 0 )
        return new QskFusionSkin( QskSkin::LightScheme );

    if ( QString::compare( skinName, fusionSkinNames[1], Qt::CaseInsensitive ) == 0 )
        return new QskFusionSkin( QskSkin::DarkScheme );

    return nullptr;
}

#include "moc_QskFusionSkinFactory.cpp"
