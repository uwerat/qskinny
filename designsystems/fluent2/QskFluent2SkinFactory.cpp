/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFluent2SkinFactory.h"
#include "QskFluent2Skin.h"

static const QString name = QStringLiteral( "Fluent2" );

QskFluent2SkinFactory::QskFluent2SkinFactory( QObject* parent )
    : QskSkinFactory( parent )
{
}

QskFluent2SkinFactory::~QskFluent2SkinFactory()
{
}

QStringList QskFluent2SkinFactory::skinNames() const
{
    return { name };
}

QskSkin* QskFluent2SkinFactory::createSkin( const QString& skinName )
{
    if ( QString::compare( skinName, name, Qt::CaseInsensitive ) == 0 )
        return new QskFluent2Skin();

    return nullptr;
}

#include "moc_QskFluent2SkinFactory.cpp"
