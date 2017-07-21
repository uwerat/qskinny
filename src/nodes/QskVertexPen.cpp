/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskVertexPen.h"

QskVertexPen::QskVertexPen():
    m_width( 0.0 ),
    m_shadeFactor( 0.0 )
{
}

bool QskVertexPen::isValid() const
{
    return ( m_width > 0.0 ) && m_colorLight.isValid() && m_colorDark.isValid();
}

bool QskVertexPen::isVisible() const
{
    if ( !isValid() )
        return false;

    return ( m_colorLight.alpha() > 0 ) && ( m_colorDark.alpha() > 0 );
}

bool QskVertexPen::isMonochrome() const
{
    return ( m_colorLight == m_colorDark );
}

void QskVertexPen::setWidth( double width )
{
    m_width = qMax( width, 0.0 );
}

double QskVertexPen::width() const
{
    return m_width;
}

void QskVertexPen::setColor( const QColor& color )
{
    m_colorLight = m_colorDark = color;
}

void QskVertexPen::setColors( const QColor& darkColor, const QColor& lightColor )
{
    m_colorDark = darkColor;
    m_colorLight = lightColor;
}

QColor QskVertexPen::lightColor() const
{
    return m_colorLight;
}

QColor QskVertexPen::darkColor() const
{
    return m_colorDark;
}

void QskVertexPen::setShadeFactor( double factor )
{
    m_shadeFactor = qBound( -1.0, factor, 1.0 );
}

double QskVertexPen::shadeFactor() const
{
    return m_shadeFactor;
}
