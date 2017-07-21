/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskVertexColor.h"

#ifndef QT_NO_DEBUG_STREAM

#include <QDebug>

QDebug operator<<( QDebug debug, const QskVertexColor& color )
{
    QDebugStateSaver saver( debug );
    debug.nospace();
    debug << "C" << '(';
    debug << color.r << "," << color.g << "," << color.b << "," << color.a;
    debug << ')';
    return debug;
}

QskVertexColorMap::QskVertexColorMap():
    m_v1( 0.0 ),
    m_v2( 0.0 ),
    m_range( 0.0 ),
    m_isIdentity( true )
{
}

void QskVertexColorMap::setColors( const QskVertexColor& c1, const QskVertexColor& c2 )
{
    m_color1 = QskVertexColor( c1 );
    m_isIdentity = ( c1 == c2 );

    if ( !m_isIdentity )
        m_color2 = QskVertexColor( c2 );
}

void QskVertexColorMap::setRange( const double v1, const double v2 )
{
    m_v1 = v1;
    m_v2 = v2;
    m_range = v2 - v1;
}

#endif
