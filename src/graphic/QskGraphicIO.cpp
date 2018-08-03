/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGraphicIO.h"
#include "QskGraphic.h"
#include "QskPainterCommand.h"

#include <qbuffer.h>
#include <qdatastream.h>
#include <qfile.h>
#include <qvector.h>

#include <cstring>

static const char qskMagicNumber[] = "QSKG";

static inline void qskWritePathData(
    const QPainterPath& path, QDataStream& s )
{
    s << path;
}

static inline void qskReadPathData(
    QDataStream& s, QVector< QskPainterCommand >& commands )
{
    QPainterPath path;
    s >> path;

    commands += QskPainterCommand( path );
}

static inline void qskWritePixmapData(
    const QskPainterCommand::PixmapData& data, QDataStream& s )
{
    s << data.rect << data.pixmap << data.subRect;
}

static inline void qskReadPixmapData(
    QDataStream& s, QVector< QskPainterCommand >& commands )
{
    QskPainterCommand::PixmapData data;

    s >> data.rect;
    s >> data.pixmap;
    s >> data.subRect;

    commands += QskPainterCommand( data.rect, data.pixmap, data.subRect );
}

static inline void qskWriteImageData(
    const QskPainterCommand::ImageData& data, QDataStream& s )
{
    s << data.rect << data.image << data.subRect;
}

static inline void qskReadImageData(
    QDataStream& s, QVector< QskPainterCommand >& commands )
{
    QskPainterCommand::ImageData data;

    s >> data.rect;
    s >> data.image;
    s >> data.subRect;

    quint8 flags;
    s >> flags;
    data.flags = static_cast< Qt::ImageConversionFlags >( flags );

    commands += QskPainterCommand( data.rect, data.image, data.subRect, data.flags );
}

static inline void qskWriteStateData(
    const QskPainterCommand::StateData& data, QDataStream& s )
{
    s << static_cast< quint16 >( data.flags );

    if ( data.flags & QPaintEngine::DirtyPen )
        s << data.pen;

    if ( data.flags & QPaintEngine::DirtyBrush )
        s << data.brush;

    if ( data.flags & QPaintEngine::DirtyBrushOrigin )
        s << data.brushOrigin;

    if ( data.flags & QPaintEngine::DirtyFont )
        s << data.font;

    if ( data.flags & QPaintEngine::DirtyBackground )
    {
        s << static_cast< quint8 >( data.backgroundMode );
        s << data.backgroundBrush;
    }

    if ( data.flags & QPaintEngine::DirtyTransform )
        s << data.transform;

    if ( data.flags & QPaintEngine::DirtyClipEnabled )
        s << data.isClipEnabled;

    if ( data.flags & QPaintEngine::DirtyClipRegion )
    {
        s << data.clipRegion;
        s << static_cast< quint8 >( data.clipOperation );
    }

    if ( data.flags & QPaintEngine::DirtyClipPath )
    {
        s << data.clipPath;
        s << static_cast< quint8 >( data.clipOperation );
    }

    if ( data.flags & QPaintEngine::DirtyHints )
        s << static_cast< qint32 >( data.renderHints );

    if ( data.flags & QPaintEngine::DirtyCompositionMode )
        s << static_cast< qint16 >( data.compositionMode );

    if ( data.flags & QPaintEngine::DirtyOpacity )
        s << data.opacity;
}

static inline void qskReadStateData(
    QDataStream& s, QVector< QskPainterCommand >& commands )
{
    QskPainterCommand::StateData data;

    quint16 flags;
    s >> flags;

    data.flags = ( QPaintEngine::DirtyFlags ) flags;

    if ( data.flags & QPaintEngine::DirtyPen )
        s >> data.pen;

    if ( data.flags & QPaintEngine::DirtyBrush )
        s >> data.brush;

    if ( data.flags & QPaintEngine::DirtyBrushOrigin )
        s >> data.brushOrigin;

    if ( data.flags & QPaintEngine::DirtyFont )
        s >> data.font;

    if ( data.flags & QPaintEngine::DirtyBackground )
    {
        quint8 bgMode = data.backgroundMode;
        s >> bgMode >> data.backgroundBrush;
        data.backgroundMode = static_cast< Qt::BGMode >( bgMode );
    }

    if ( data.flags & QPaintEngine::DirtyTransform )
        s >> data.transform;

    if ( data.flags & QPaintEngine::DirtyClipEnabled )
        s >> data.isClipEnabled;

    if ( data.flags & QPaintEngine::DirtyClipRegion )
    {
        s >> data.clipRegion;

        quint8 clipOperation;
        s >> clipOperation;
        data.clipOperation = static_cast< Qt::ClipOperation >( clipOperation );
    }

    if ( data.flags & QPaintEngine::DirtyClipPath )
    {
        s >> data.clipPath;

        quint8 clipOperation;
        s >> clipOperation;
        data.clipOperation = static_cast< Qt::ClipOperation >( clipOperation );
    }

    if ( data.flags & QPaintEngine::DirtyHints )
    {
        qint32 renderHints;
        s >> renderHints;
        data.renderHints = static_cast< QPainter::RenderHints >( renderHints );
    }

    if ( data.flags & QPaintEngine::DirtyCompositionMode )
    {
        qint16 compositionMode;
        s >> compositionMode;
        data.compositionMode = static_cast< QPainter::CompositionMode >( compositionMode );
    }

    if ( data.flags & QPaintEngine::DirtyOpacity )
        s >> data.opacity;

    commands += QskPainterCommand( data );
}

QskGraphic QskGraphicIO::read( const QString& fileName )
{
    QFile file( fileName );
    if ( file.open( QIODevice::ReadOnly ) == false )
    {
        qWarning( "QskGraphicIO::read can't open %s", qPrintable( fileName ) );
        return QskGraphic();
    }

    return read( &file );
}

QskGraphic QskGraphicIO::read( const QByteArray& data )
{
    QBuffer buffer;
    buffer.setData( data );

    return read( &buffer );
}

QskGraphic QskGraphicIO::read( QIODevice* dev )
{
    if ( dev == nullptr )
        return QskGraphic();

    QDataStream stream( dev );
    stream.setByteOrder( QDataStream::BigEndian );

    char magicNumber[ 4 ];
    stream.readRawData( magicNumber, 4 );
    if ( memcmp( magicNumber, qskMagicNumber, 4 ) != 0 )
    {
        qWarning( "QskGraphicIO::read: bad magic number" );
        return QskGraphic();
    }

    quint32 numCommands;
    stream >> numCommands;

    QVector< QskPainterCommand > commands;
    commands.reserve( numCommands );

    for ( uint i = 0; i < numCommands; i++ )
    {
        quint8 type;
        stream >> type;

        switch ( type )
        {
            case QskPainterCommand::Path:
            {
                qskReadPathData( stream, commands );
                break;
            }
            case QskPainterCommand::Pixmap:
            {
                qskReadPixmapData( stream, commands );
                break;
            }
            case QskPainterCommand::Image:
            {
                qskReadImageData( stream, commands );
                break;
            }
            case QskPainterCommand::State:
            {
                qskReadStateData( stream, commands );
                break;
            }
            default:
                return QskGraphic();
        }
    }

    QskGraphic graphic;
    graphic.setCommands( commands );

    return graphic;
}

bool QskGraphicIO::write( const QskGraphic& graphic, const QString& fileName )
{
    QFile file( fileName );
    if ( file.open( QIODevice::WriteOnly | QIODevice::Truncate ) == false )
    {
        qWarning( "QskGraphicIO::write can't open %s", qPrintable( fileName ) );
        return false;
    }

    return write( graphic, &file );
}

bool QskGraphicIO::write( const QskGraphic& graphic, QByteArray& data )
{
    QBuffer buffer( &data );
    return write( graphic, &buffer );
}

bool QskGraphicIO::write( const QskGraphic& graphic, QIODevice* dev )
{
    if ( dev == nullptr )
        return false;

    QDataStream stream( dev );
    stream.setByteOrder( QDataStream::BigEndian );
    stream.writeRawData( qskMagicNumber, 4 );

    const int numCommands = graphic.commands().size();
    const QskPainterCommand* cmds = graphic.commands().constData();

    stream << static_cast< quint32 >( numCommands );

    for ( int i = 0; i < numCommands; i++ )
    {
        stream << static_cast< quint8 >( cmds[ i ].type() );

        switch ( cmds[ i ].type() )
        {
            case QskPainterCommand::Path:
            {
                qskWritePathData( *cmds[ i ].path(), stream );
                break;
            }
            case QskPainterCommand::Pixmap:
            {
                qskWritePixmapData( *cmds[ i ].pixmapData(), stream );
                break;
            }
            case QskPainterCommand::Image:
            {
                qskWriteImageData( *cmds[ i ].imageData(), stream );
                break;
            }
            case QskPainterCommand::State:
            {
                qskWriteStateData( *cmds[ i ].stateData(), stream );
                break;
            }
            default:
            {
                // cleanup ???
                return false;
            }
        }
    }

    return true;
}
