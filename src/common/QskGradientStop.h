/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRADIENT_STOP_H
#define QSK_GRADIENT_STOP_H

#include "QskGlobal.h"

#include <qcolor.h>
#include <qmetatype.h>

class QSK_EXPORT QskGradientStop
{
    Q_GADGET

    Q_PROPERTY( qreal position READ position WRITE setPosition RESET resetPosition )
    Q_PROPERTY( QColor color READ color WRITE setColor RESET resetColor )

  public:
    QskGradientStop() noexcept;
    QskGradientStop( qreal position, const QColor& ) noexcept;
    QskGradientStop( qreal position, Qt::GlobalColor ) noexcept;
    QskGradientStop( qreal position, QRgb ) noexcept;

    bool operator==( const QskGradientStop& ) const noexcept;
    bool operator!=( const QskGradientStop& ) const noexcept;

    void setStop( qreal position, const QColor& ) noexcept;

    qreal position() const noexcept;
    void setPosition( qreal position ) noexcept;
    void resetPosition() noexcept;

    const QColor& color() const noexcept;
    void setColor( const QColor& ) noexcept;
    void resetColor() noexcept;

    static QColor interpolated(
        const QskGradientStop&, const QskGradientStop&, qreal position ) noexcept;

    QskHashValue hash( QskHashValue seed ) const noexcept;

  private:
    qreal m_position;
    QColor m_color; // using RGBA instead ?
};

Q_DECLARE_TYPEINFO( QskGradientStop, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskGradientStop )

inline QskGradientStop::QskGradientStop() noexcept
    : m_position( -1.0 )
{
}

inline QskGradientStop::QskGradientStop(
        qreal position, const QColor& color ) noexcept
    : m_position( position )
    , m_color( color )
{
}

inline QskGradientStop::QskGradientStop(
        qreal position, const Qt::GlobalColor color ) noexcept
    : QskGradientStop( position, QColor( color ) )
{
}

inline QskGradientStop::QskGradientStop(
        qreal position, QRgb rgb ) noexcept
    : QskGradientStop( position, QColor::fromRgba( rgb ) )
{
}

inline qreal QskGradientStop::position() const noexcept
{
    return m_position;
}

inline const QColor& QskGradientStop::color() const noexcept
{
    return m_color;
}

inline bool QskGradientStop::operator==( const QskGradientStop& other ) const noexcept
{
    return ( m_position == other.m_position ) && ( m_color == other.m_color );
}

inline bool QskGradientStop::operator!=( const QskGradientStop& other ) const noexcept
{
    return ( !( *this == other ) );
}

#ifndef QT_NO_DEBUG_STREAM

class QDebug;

QSK_EXPORT QDebug operator<<( QDebug, const QskGradientStop& );

#endif

#endif
