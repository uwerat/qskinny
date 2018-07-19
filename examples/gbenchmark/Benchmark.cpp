/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Benchmark.h"

#include <QskGraphic.h>
#include <QskGraphicIO.h>
#include <QskGraphicTextureFactory.h>

#include <QDir>
#include <QStringList>
#include <QSvgRenderer>
#include <QSvgRenderer>
#include <QDebug>
#include <QElapsedTimer>
#include <QQuickWindow>
#include <QPainter>

bool Benchmark::run( const QString& dirName )
{
    QDir svgDir( dirName );

    QStringList svgFiles = svgDir.entryList( QStringList() << "*.svg", QDir::Files );
    if ( svgFiles.isEmpty() )
        return false;

    const char qvgPath[] = "/tmp/benchmark/qvg";
    if ( !svgDir.mkpath( qvgPath ) )
        return false;

    QStringList qvgFiles = svgFiles;
    for ( int i = 0; i < qvgFiles.size(); i++ )
    {
        svgFiles[i].prepend( "/" );
        svgFiles[i].prepend( dirName );

        qvgFiles[i].replace( ".svg", ".qvg" );
        qvgFiles[i].prepend( "/" );
        qvgFiles[i].prepend( qvgPath );
    }

    QVector< QskGraphic > graphics( qvgFiles.size() );
    QVector< QSvgRenderer* > renderers( svgFiles.size() );

    qint64 msElapsed[6];

    QElapsedTimer timer;

    {
        // compile step

        timer.start();

        for ( int i = 0; i < svgFiles.size(); i++ )
        {
            renderers[i] = new QSvgRenderer();
            if ( !renderers[i]->load( svgFiles[i] ) )
            {
                qCritical() << "Can't load" << svgFiles[i];
                return false;
            }
        }

        msElapsed[0] = timer.elapsed();
    }

    {
        // converting into graphics and storing to disk

        timer.start();

        for ( int i = 0; i < renderers.size(); i++ )
        {
            QPainter painter( &graphics[i] );
            renderers[i]->render( &painter );
            painter.end();
        }


        msElapsed[1] = timer.elapsed();
    }

    {
        // writing them to disk

        timer.start();

        for ( int i = 0; i < graphics.size(); i++ )
        {
            QskGraphicIO::write( graphics[i], qvgFiles[i] );
        }

        msElapsed[2] = timer.elapsed();
    }

    {
        // loading qvg files to memory

        timer.start();

        for ( int i = 0; i < qvgFiles.size(); i++ )
        {
            graphics[i] = QskGraphicIO::read( qvgFiles[i] );
            if ( graphics[i].isNull() )
            {
                qCritical() << "Can't load" << qvgFiles[i];
                return false;
            }
        }

        msElapsed[3] = timer.elapsed();
    }

    {
        // creating textures using OpenGL

        timer.start();

        const QRect targetRect( 0, 0, 200, 200 );
        const QskColorFilter colorFilter;

        for ( int i = 0; i < qvgFiles.size(); i++ )
        {
            const auto textureId = QskGraphicTextureFactory::createTexture(
                QskGraphicTextureFactory::OpenGL, targetRect, Qt::KeepAspectRatio,
                graphics[i], colorFilter );

            if ( textureId == 0 )
            {
                qCritical() << "Can't render texture for" << qvgFiles[i];
                return false;
            }
        }

        msElapsed[4] = timer.elapsed();
    }

    {
        // creating textures using Raster

        timer.start();

        const QRect targetRect( 0, 0, 100, 100 );
        const QskColorFilter colorFilter;

        for ( int i = 0; i < qvgFiles.size(); i++ )
        {
            const auto textureId = QskGraphicTextureFactory::createTexture(
                QskGraphicTextureFactory::Raster, targetRect, Qt::KeepAspectRatio,
                graphics[i], colorFilter );

            if ( textureId == 0 )
            {
                qCritical() << "Can't render texture for" << qvgFiles[i];
                return false;
            }
        }

        msElapsed[5] = timer.elapsed();
    }

    qDebug() << "#Icons:" << svgFiles.count() <<
        "Compiled:" << msElapsed[0] <<
        "Converted:" << msElapsed[1] <<
        "Stored:" << msElapsed[2] <<
        "Loaded:" << msElapsed[3] <<
        "Rendered OpenGL:" << msElapsed[4] <<
        "Rendered Raster:" << msElapsed[5];

    svgDir.rmdir( qvgPath );

    return true;
}
