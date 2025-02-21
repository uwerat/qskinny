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

enum Options
{
    ViewBox = 1 << 0,
    Antialiasing = 1 << 1
};

static void usage( const char* appName )
{
    qWarning() << "usage: " << appName << "<fontfile> <pixelsize> <glyphindex> <qvgfile>";
}

static QPainterPath glyphPath( const QRawFont& font,
    const qreal pixelSize, const uint glyphIndex )
{
    auto path = font.pathForGlyph( glyphIndex );
    path = path.simplified();
    path = path.translated( 0.0, pixelSize );

    return path;
}

static QskGraphic icon( const QPainterPath& path,
    const qreal pixelSize, const int options )
{
    QskGraphic graphic;

    if ( options & ViewBox )
        graphic.setViewBox( QRectF( 0.0, 0.0, pixelSize, pixelSize ) );

    QPainter painter( &graphic );

    if ( options & Antialiasing )
        painter.setRenderHint( QPainter::Antialiasing, true );

    painter.fillPath( path, Qt::black );

    return graphic;
}

int main( int argc, char* argv[] )
{
    QGuiApplication app( argc, argv );

    if ( argc != 5 )
    {
        usage( argv[0] );
        return -1;
    }

    bool ok;

    const auto pixelSize = QString( argv[2] ).toDouble( &ok );
    if ( !ok || ( pixelSize <= 0 ) )
    {
        qWarning() << "invalid pixel size:" << argv[2];
        return -3;
    }

    const auto glyphIndex = QString( argv[3] ).toUInt( &ok );
    if ( !ok )
    {
        qWarning() << "invalid glyph index:" << argv[3];
        return -3;
    }

    QRawFont font( QString( argv[1] ), pixelSize );
    if ( !font.isValid() )
    {
        qWarning() << "invalid font name:" << argv[1];
        return -2;
    }

    const auto path = glyphPath( font, pixelSize, glyphIndex );
    if ( path.isEmpty() )
    {
        qWarning() << "no glyph for index:" << argv[3];
        return -3;
    }

    const auto graphic = icon( path, pixelSize, ViewBox | Antialiasing );
    QskGraphicIO::write( graphic, argv[4] );

    return 0;
}
