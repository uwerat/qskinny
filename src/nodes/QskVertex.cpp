/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskVertex.h"

using namespace QskVertex;

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, Color color )
{
    QDebugStateSaver saver( debug );
    debug.nospace();
    debug << "C" << '(';
    debug << color.r << "," << color.g << "," << color.b << "," << color.a;
    debug << ')';

    return debug;
}

QDebug operator<<( QDebug debug, ColoredLine line )
{
    qDebug() << qRound( line.p1.x ) << qRound( line.p1.y )
        << "->" << qRound( line.p2.x ) << qRound( line.p2.y );

    return debug;
}

QDebug operator<<( QDebug debug, Line line )
{
    qDebug() << qRound( line.p1.x ) << qRound( line.p1.y )
        << "->" << qRound( line.p2.x ) << qRound( line.p2.y );

    return debug;
}

#endif

template< class Line >
static inline void qskDebugGeometry( Line* lines, int count )
{
#ifndef QT_NO_DEBUG_STREAM
    for ( int i = 0; i < count; i++ )
    {
        const auto l = lines[i];

        qDebug() << i << ":"
            << qRound( l.p1.x ) << qRound( l.p1.y )
            << "->" << qRound( l.p2.x ) << qRound( l.p2.y );
    }
#endif
}

void QskVertex::debugGeometry( const QSGGeometry& geometry )
{
    const auto lineCount = geometry.vertexCount() / 2;

    if ( geometry.attributeCount() == 1 )
    {
        const auto lines = reinterpret_cast< const Line* >( geometry.vertexData() );
        qskDebugGeometry( lines, lineCount );
    }
    else if ( geometry.attributeCount() == 2 )
    {
        const auto lines = reinterpret_cast< const ColoredLine* >( geometry.vertexData() );
        qskDebugGeometry( lines, lineCount );
    }
}
