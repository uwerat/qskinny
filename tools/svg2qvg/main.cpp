/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#if defined( QSK_STANDALONE )
#include <QskGraphic.cpp>
#include <QskRgbValue.cpp>
#include <QskColorFilter.cpp>
#include <QskPainterCommand.cpp>
#include <QskGraphicPaintEngine.cpp>
#include <QskGraphicIO.cpp>
#else
#include <QskGraphicIO.h>
#include <QskGraphic.h>
#endif

#include <QGuiApplication>
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

#if 0
    /*
        When there are no "text" parts in the SVGs we can avoid
        the costs of initializing not needed fonts
        by using the minimal plugin.
     */
    qputenv( "QT_QPA_PLATFORM", "minimal" );
#endif

#if 1
    /*
        When having a SVG with specific font assignments Qt runs on
        qGuiApp to load a default font. Makes no sense in this context,
        but to avoid having segfaults ...
     */
    QGuiApplication app( argc, argv );
#endif

    QSvgRenderer renderer;
    if ( !renderer.load( QString( argv[1] ) ) )
        return -2;

    QskGraphic graphic;

    QPainter painter( &graphic );
    renderer.render( &painter );
    painter.end();

    if ( graphic.commandTypes() & QskGraphic::RasterData )
        qWarning() << argv[1] << "contains non scalable parts.";

    QskGraphicIO::write( graphic, argv[2] );

    return 0;
}

