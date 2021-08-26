/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "GraphicProvider.h"

#include <QskGraphic.h>

#include <QSvgRenderer>
#include <QPainter>
#include <QImage>
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
    static QString scope = QStringLiteral( ":/images/" );

    QString baseName = scope;
    baseName += id.toLower().replace( ' ', '-' );

    auto path = pathName( baseName, QString() );

    if ( path.isEmpty() )
        path = pathName( baseName, ".png" );

    if ( path.isEmpty() )
        path = pathName( baseName, ".svg" );

    QskGraphic graphic;

    if ( !path.isEmpty() )
    {
        if ( path.endsWith( ".png" ) )
        {
            graphic = QskGraphic::fromImage( QImage( path ) );
        }
        else
        {
            QSvgRenderer renderer;
            if ( renderer.load( path ) )
            {
                QPainter painter( &graphic );
                renderer.render( &painter );
                painter.end();
            }
        }
    }

    return graphic.isNull() ? nullptr : new QskGraphic( graphic );
}
