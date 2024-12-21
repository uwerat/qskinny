
/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "SymbolProvider.h"

#include <QskSkinManager.h>

#include <QFile>
#include <QDebug>

static QString fontFileName()
{
    const auto skinName = qskSkinManager->skinName();
    if ( skinName == QStringLiteral( "Fluent2" ) )
        return QStringLiteral( ":/fonts/FluentSystemIcons.ttf" );

    if ( skinName == QStringLiteral( "Material3" ) )
        return QStringLiteral( ":/fonts/MaterialSymbole.ttf" );

    return QStringLiteral( ":/fonts/FontAwesome.ttf" );
}

SymbolProvider::SymbolProvider( QObject* parent )
    : QskSymbolProvider( parent )
{
    updateFont();
}

void SymbolProvider::updateFont()
{
    const auto fileName = fontFileName();
    qDebug() << fileName;

    QFile file( fileName );
    if ( file.open( QIODevice::ReadOnly ) )
        setFontData( file.readAll() );
    else
        qWarning() << "Can't load symbol fonts from:" << file.fileName();
}
