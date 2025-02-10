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
#include <QRawFont>
#include <QPainter>
#include <QPainterPath>
#include <QDebug>

static void usage( const char* appName )
{
    qWarning() << "usage: " << appName << "<fontfile> <glyphindex> <qvgfile>";
}

int main( int argc, char* argv[] )
{
    if ( argc != 4 )
    {
        usage( argv[0] );
        return -1;
    }

    const int pixelSize = 24; // something

    QGuiApplication app( argc, argv );

    QRawFont font( QString( argv[1] ), pixelSize );
    if ( !font.isValid() )
    {
        qWarning() << "invalid font name:" << argv[1];
        return -2;
    }

    bool ok;

    const auto glyphIndex = QString( argv[2] ).toUInt( &ok );
    if ( !ok )
    {
        qWarning() << "invalid glyph index:" << argv[2];
        return -3;
    }

    auto path = font.pathForGlyph( glyphIndex );
    if ( path.isEmpty() )
    {
        qWarning() << "no glyph for index:" << argv[2];
        return -3;
    }
    path.translate( 0.0, pixelSize );

    QskGraphic graphic;
    graphic.setViewBox( QRectF( 0.0, 0.0, pixelSize, pixelSize ) );

    QPainter painter( &graphic );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.fillPath( path, Qt::black );

    QskGraphicIO::write( graphic, argv[3] );

    return 0;
}
