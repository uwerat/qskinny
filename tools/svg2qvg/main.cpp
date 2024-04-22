/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
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

static QRectF viewBox( QSvgRenderer& renderer )
{
    /*
        QSvgRenderer::viewBoxF() returns a bounding box when no viewBox
        has been defined. So we clear the viewBox and compare the result with
        the initial value - assuming, that there was a viewBox when they differ.
     */
    const auto viewBox = renderer.viewBoxF();

    renderer.setViewBox( QRectF() );
    const bool hasViewBox = ( viewBox != renderer.viewBoxF() );
    renderer.setViewBox( viewBox );

    return hasViewBox ? viewBox : QRectF( 0.0, 0.0, -1.0, -1.0 );
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
    graphic.setViewBox( ::viewBox( renderer ) );

    QPainter painter( &graphic );
    renderer.render( &painter );
    painter.end();

    if ( graphic.commandTypes() & QskGraphic::RasterData )
        qWarning() << argv[1] << "contains non scalable parts.";

    QskGraphicIO::write( graphic, argv[2] );

    return 0;
}

