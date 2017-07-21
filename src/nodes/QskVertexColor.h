/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_VERTEX_COLOR_H
#define QSK_VERTEX_COLOR_H

#include "QskGlobal.h"
#include <QColor>

class QSK_EXPORT QskVertexColor
{
public:
    QskVertexColor();
    QskVertexColor( const QColor& color );
    QskVertexColor( unsigned char red, unsigned char green,
        unsigned char blue, unsigned char alpha );

    QskVertexColor interpolatedTo( const QskVertexColor&, double ratio ) const;

    bool operator==( const QskVertexColor& ) const;
    bool operator!=( const QskVertexColor& ) const;

public:
    unsigned char r, g, b, a;
};

class QSK_EXPORT QskVertexColorMap
{
public:
    QskVertexColorMap();

    void setColors( const QskVertexColor&, const QskVertexColor& );
    void setRange( double v1, double v2 );

    QskVertexColor colorAt( double v ) const;

    inline double value1() const
    {
        return m_v1;
    };
    inline double value2() const
    {
        return m_v2;
    }

    inline QskVertexColor color1() const
    {
        return m_color1;
    }
    inline QskVertexColor color2() const
    {
        return m_color2;
    }

private:
    double m_v1;
    double m_v2;
    double m_range;

    QskVertexColor m_color1;
    QskVertexColor m_color2;

    bool m_isIdentity;
};

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<( QDebug debug, const QskVertexColor& );
#endif

inline QskVertexColor::QskVertexColor():
    r( 0 ),
    g( 0 ),
    b( 0 ),
    a( 255 )
{
}

inline QskVertexColor::QskVertexColor( unsigned char red, unsigned char green,
    unsigned char blue, unsigned char alpha ):
    r( red ),
    g( green ),
    b( blue ),
    a( alpha )
{
}

inline QskVertexColor::QskVertexColor( const QColor& color )
{
    r = color.red();
    g = color.green();
    b = color.blue();
    a = color.alpha();

    if ( a < 255 )
    {
        const double af = color.alphaF();

        r *= af;
        g *= af;
        b *= af;
    }
}

inline QskVertexColor QskVertexColor::interpolatedTo(
    const QskVertexColor& colorTo, double ratio ) const
{
    if ( ratio <= 0.0 )
        return *this;

    if ( ratio >= 1.0 )
        return colorTo;

    const double t = ratio;
    const double rt = 1.0 - ratio;

    return QskVertexColor( rt * r + t * colorTo.r, rt * g + t * colorTo.g,
        rt * b + t * colorTo.b, rt * a + t * colorTo.a );
}

inline bool QskVertexColor::operator==( const QskVertexColor& other ) const
{
    return ( r == other.r ) && ( g == other.g )
           && ( b == other.b ) && ( a == other.a );
}

inline bool QskVertexColor::operator!=( const QskVertexColor& other ) const
{
    return !( *this == other );
}

inline QskVertexColor QskVertexColorMap::colorAt( const double v ) const
{
    if ( m_isIdentity )
        return m_color1;

    const double t = ( v - m_v1 ) / m_range;

    QskVertexColor c;
    c.a = ( 1.0 - t ) * m_color1.a + t * m_color2.a;
    c.r = ( 1.0 - t ) * m_color1.r + t * m_color2.r;
    c.g = ( 1.0 - t ) * m_color1.g + t * m_color2.g;
    c.b = ( 1.0 - t ) * m_color1.b + t * m_color2.b;

    return c;
}

#endif
