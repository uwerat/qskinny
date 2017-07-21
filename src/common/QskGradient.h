/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRADIENT_H
#define QSK_GRADIENT_H

#include "QskGlobal.h"
#include <QMetaType>
#include <QColor>
#include <QVector>

class QDebug;

class QSK_EXPORT QskGradientStop
{
    Q_GADGET

    Q_PROPERTY( qreal position READ position WRITE setPosition RESET resetPosition )
    Q_PROPERTY( QColor color READ color WRITE setColor RESET resetColor )

public:
    QskGradientStop();
    QskGradientStop( qreal position, const QColor& color );

    bool operator==( const QskGradientStop& ) const;
    bool operator!=( const QskGradientStop& ) const;

    qreal position() const;
    void setPosition( qreal position );
    void resetPosition();

    QColor color() const;
    void setColor( const QColor& color );
    void resetColor();

private:
    qreal m_position;
    QColor m_color;
};

class QSK_EXPORT QskGradient
{
    Q_GADGET

    Q_PROPERTY( Orientation orientation READ orientation WRITE setOrientation )
    Q_PROPERTY( QVector< QskGradientStop > stops READ stops WRITE setStops )

public:
    // TODO: radial/canonical gradients + other diagonal linear gradients
    enum Orientation
    {
        Horizontal,
        Vertical,
        Diagonal
    };

    Q_ENUM( Orientation )

    QskGradient();
    QskGradient( const QColor& );
    QskGradient( Orientation, const QColor&, const QColor& );

    ~QskGradient();

    void setOrientation( Orientation );
    Orientation orientation() const;

    bool isValid() const;
    void invalidate();

    bool operator==( const QskGradient& ) const;
    bool operator!=( const QskGradient& ) const;

    void setColor( const QColor& );
    void setColors( const QColor&, const QColor& );

    void setStops( const QVector< QskGradientStop >& stops );
    QVector< QskGradientStop > stops() const;

    bool isMonochrome() const;
    bool isVisible() const;

private:
    void setStopAt( int index, qreal stop );
    qreal stopAt( int index ) const;

    void setColorAt( int index, const QColor& color );
    QColor colorAt( int index ) const;

    Orientation m_orientation;
    QVector< QskGradientStop > m_stops;
};

inline bool QskGradientStop::operator==( const QskGradientStop& other ) const
{
    return ( m_position == other.m_position ) && ( m_color == other.m_color );
}

inline bool QskGradientStop::operator!=( const QskGradientStop& other ) const
{
    return ( !( *this == other ) );
}

inline bool QskGradient::operator==( const QskGradient& other ) const
{
    return ( m_orientation == other.m_orientation ) && ( m_stops == other.m_stops );
}

inline bool QskGradient::operator!=( const QskGradient& other ) const
{
    return ( !( *this == other ) );
}

Q_DECLARE_TYPEINFO( QskGradientStop, Q_MOVABLE_TYPE );

Q_DECLARE_METATYPE( QskGradientStop )
Q_DECLARE_METATYPE( QskGradient )

#ifndef QT_NO_DEBUG_STREAM

QSK_EXPORT QDebug operator<<( QDebug, const QskGradientStop& );
QSK_EXPORT QDebug operator<<( QDebug, const QskGradient& );

#endif

#endif
