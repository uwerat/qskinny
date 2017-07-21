/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "Border.h"
#include <QDebug>

static void registerBorder()
{
    qRegisterMetaType<Border>();
}

Q_CONSTRUCTOR_FUNCTION( registerBorder )

Border::Border( const QColor& color ):
    m_width( 1.0 ),
    m_color( color )
{
}

Border::Border( qreal width, const QColor& color ):
    m_width( qMax( width, 0.0 ) ),
    m_color( color )
{
}

void Border::setWidth( qreal width )
{
    m_width = qMax( width, 0.0 );
}

qreal Border::width() const
{
    return m_width;
}

void Border::setColor( const QColor& color )
{
    m_color = color;
}

QColor Border::color() const
{
    return m_color;
}

bool Border::operator==( const Border& other ) const
{
    return ( m_width == other.m_width ) && ( m_color == other.m_color );
}

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<( QDebug debug, const Border& border )
{
    QDebugStateSaver saver( debug );
    debug.nospace();
    debug << "Border" << '(';
    debug << border.width() << "," << border.color();
    debug << ')';
    return debug;
}

#endif

#include "moc_Border.cpp"
