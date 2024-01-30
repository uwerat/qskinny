/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFusionSkinFactory.h"
#include "QskFusionSkin.h"

static const QString name = QStringLiteral( "Fusion" );

QskFusionSkinFactory::QskFusionSkinFactory( QObject* parent )
    : QskSkinFactory( parent )
{
}

QskFusionSkinFactory::~QskFusionSkinFactory()
{
}

QStringList QskFusionSkinFactory::skinNames() const
{
    return { name };
}

QskSkin* QskFusionSkinFactory::createSkin( const QString& skinName )
{
    if ( QString::compare( skinName, name, Qt::CaseInsensitive ) == 0 )
        return new QskFusionSkin();

    return nullptr;
}

#include "moc_QskFusionSkinFactory.cpp"
