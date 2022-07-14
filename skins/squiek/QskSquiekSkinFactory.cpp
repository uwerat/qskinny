/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
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

QStringList QskSquiekSkinFactory::skinNames() const
{
    return { squiekSkinName };
}

QskSkin* QskSquiekSkinFactory::createSkin( const QString& skinName )
{
    if ( QString::compare( skinName, squiekSkinName, Qt::CaseInsensitive ) == 0 )
        return new QskSquiekSkin();

    return nullptr;
}

#include "moc_QskSquiekSkinFactory.cpp"
