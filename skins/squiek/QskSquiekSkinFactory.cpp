/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSquiekSkinFactory.h"
#include "QskSquiekSkin.h"

static const QString squiekSkinName = QStringLiteral( "Squiek" );

QskSquiekSkinFactory::QskSquiekSkinFactory( QObject* parent )
    : QskSkinFactory( parent )
{
}

QskSquiekSkinFactory::~QskSquiekSkinFactory()
{
}

QVector< QskSkin::SkinInfo > QskSquiekSkinFactory::skins() const
{
    return { qMakePair( squiekSkinName, QskSkin::UnknownScheme ) };
}

QskSkin* QskSquiekSkinFactory::createSkin( QskSkin::SkinInfo info )
{
    if ( QString::compare( info.first, squiekSkinName, Qt::CaseInsensitive ) == 0 )
    {
        return new QskSquiekSkin();
    }

    return nullptr;
}

#include "moc_QskSquiekSkinFactory.cpp"
