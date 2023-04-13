/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskWindowsSkinFactory.h"
#include "QskWindowsSkin.h"

static const QString windowsLightSkinName = QStringLiteral( "Windows Light" );
static const QString windowsDarkSkinName = QStringLiteral( "Windows Dark" );

QskWindowsSkinFactory::QskWindowsSkinFactory( QObject* parent )
    : QskSkinFactory( parent )
{
}

QskWindowsSkinFactory::~QskWindowsSkinFactory()
{
}

QStringList QskWindowsSkinFactory::skinNames() const
{
    return { windowsLightSkinName, windowsDarkSkinName };
}

QskSkin* QskWindowsSkinFactory::createSkin( const QString& skinName )
{
    if ( QString::compare( skinName, windowsLightSkinName, Qt::CaseInsensitive ) == 0 )
    {
        QskWindowsTheme theme( QskWindowsTheme::Light );
        return new QskWindowsSkin( theme );
    }
    else if ( QString::compare( skinName, windowsDarkSkinName, Qt::CaseInsensitive ) == 0 )
    {
        QskWindowsTheme theme( QskWindowsTheme::Dark );
        return new QskWindowsSkin( theme );
    }

    return nullptr;
}

#include "moc_QskWindowsSkinFactory.cpp"
