/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskMaterial3SkinFactory.h"
#include "QskMaterial3Skin.h"

static const QString name = QStringLiteral( "Material3" );

QskMaterial3SkinFactory::QskMaterial3SkinFactory( QObject* parent )
    : QskSkinFactory( parent )
{
}

QskMaterial3SkinFactory::~QskMaterial3SkinFactory()
{
}

QStringList QskMaterial3SkinFactory::skinNames() const
{
    return { name };
}

QskSkin* QskMaterial3SkinFactory::createSkin( const QString& skinName )
{
    if ( QString::compare( skinName, name, Qt::CaseInsensitive ) == 0 )
        return new QskMaterial3Skin();

    return nullptr;
}

#include "moc_QskMaterial3SkinFactory.cpp"
