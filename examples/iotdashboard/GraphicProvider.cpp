/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "GraphicProvider.h"

#include <QskGraphic.h>
#include <QskGraphicIO.h>

#include <QFile>

const inline QString pathName( const QString& baseName, const QString& suffix )
{
    QString fileName = baseName;
    if ( !suffix.isEmpty() )
        fileName += suffix;

    return QFile( fileName ).exists() ? fileName : QString();
}

const QskGraphic* GraphicProvider::loadGraphic( const QString& id ) const
{
    static QString scope = QStringLiteral( ":/images/qvg/" );

    QString baseName = scope;
    baseName += id.toLower().replace( ' ', '-' );

    auto path = pathName( baseName, QString() );

    if ( path.isEmpty() )
        path = pathName( baseName, ".qvg" );

    QskGraphic graphic;

    if ( !path.isEmpty() )
        graphic = QskGraphicIO::read( path );

    return graphic.isNull() ? nullptr : new QskGraphic( graphic );
}
