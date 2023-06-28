/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskMaterial3SkinFactory.h"
#include "QskMaterial3Skin.h"

static const QString materialSkinName = QStringLiteral( "Material3" );

QskMaterial3SkinFactory::QskMaterial3SkinFactory( QObject* parent )
    : QskSkinFactory( parent )
{
}

QskMaterial3SkinFactory::~QskMaterial3SkinFactory()
{
}

QVector< QskSkin::SkinInfo > QskMaterial3SkinFactory::skins() const
{
    return { qMakePair( materialSkinName, QskSkin::LightScheme ),
            qMakePair( materialSkinName, QskSkin::DarkScheme ) };
}

QskSkin* QskMaterial3SkinFactory::createSkin( QskSkin::SkinInfo info )
{
    // default to light:
    const auto scheme = info.second == QskSkin::UnknownScheme ? QskSkin::LightScheme : info.second;

    if( QString::compare( info.first, materialSkinName, Qt::CaseInsensitive ) == 0 )
    {
        QskMaterial3Theme theme( scheme );
        return new QskMaterial3Skin( theme );
    }

    return nullptr;
}

#include "moc_QskMaterial3SkinFactory.cpp"
