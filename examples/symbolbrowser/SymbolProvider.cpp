
/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "SymbolProvider.h"

#include <QskSkinManager.h>
#include <QskGraphic.h>

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
    if ( fileName == m_fontFileName )
        return;

    qDebug() << fileName;

    QFile file( fileName );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        qWarning() << "Can't load symbol fonts from:" << file.fileName();
        return;
    }

    m_fontFileName = fileName;
    setFontData( file.readAll() );

#if 0
    for ( uint i = 0xe000; i < 0xffff; i++ )
    {
        const auto graphic = requestSymbol( i );
        delete graphic;
    }
#endif
}
