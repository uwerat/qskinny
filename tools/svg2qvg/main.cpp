/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include <QskGraphicIO.h>
#include <QskGraphic.h>
#include <QSvgRenderer>
#include <QPainter>
#include <QDebug>

static void usage( const char* appName )
{
    qWarning() << "usage: " << appName << "svgfile qvgfile";
}

int main( int argc, char* argv[] )
{
    if ( argc != 3 )
    {
        usage( argv[0] );
        return -1;
    }

    QSvgRenderer renderer;
    if ( !renderer.load( QString( argv[1] ) ) )
        return -2;

    QskGraphic graphic;

    QPainter painter( &graphic );
    renderer.render( &painter );
    painter.end();

    QskGraphicIO::write( graphic, argv[2] );

    return 0;
}

