/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRADIENT_STOP_H
#define QSK_GRADIENT_STOP_H

#include "QskGlobal.h"

#include <qcolor.h>
#include <qmetatype.h>
#include <qvector.h>

typedef QPair< qreal, QColor > QGradientStop;

class QSK_EXPORT QskGradientStop
{
    Q_GADGET

    Q_PROPERTY( qreal position READ position WRITE setPosition )
    Q_PROPERTY( QColor color READ color WRITE setColor )

  public:
    constexpr QskGradientStop() noexcept = default;
    constexpr QskGradientStop( qreal position, const QColor& ) noexcept;
    constexpr QskGradientStop( const QGradientStop& ) noexcept;

    QskGradientStop( qreal position, Qt::GlobalColor ) noexcept;
    QskGradientStop( qreal position, QRgb ) noexcept;

    constexpr bool operator==( const QskGradientStop& ) const noexcept;
    constexpr bool operator!=( const QskGradientStop& ) const noexcept;

    void setStop( qreal position, const QColor& ) noexcept;
    void setStop( qreal position, Qt::GlobalColor ) noexcept;
    void setStop( qreal position, QRgb ) noexcept;

    constexpr qreal position() const noexcept;
    void setPosition( qreal position ) noexcept;

    constexpr const QColor& color() const noexcept;
    void setColor( const QColor& ) noexcept;

    void setRgb( QRgb ) noexcept;
    QRgb rgb() const noexcept;

    static QColor interpolated(
        const QskGradientStop&, const QskGradientStop&, qreal position ) noexcept;

    QskHashValue hash( QskHashValue seed ) const noexcept;

  private:
    qreal m_position = -1.0;
    QColor m_color; // using QRgb instead ?
};

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    Q_DECLARE_TYPEINFO( QskGradientStop, Q_RELOCATABLE_TYPE );
#endif

Q_DECLARE_METATYPE( QskGradientStop )

inline constexpr QskGradientStop::QskGradientStop(
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

inline constexpr QskGradientStop::QskGradientStop( const QGradientStop& qtStop ) noexcept
    : QskGradientStop( qtStop.first, qtStop.second )
{
}

inline constexpr qreal QskGradientStop::position() const noexcept
{
    return m_position;
}

inline constexpr const QColor& QskGradientStop::color() const noexcept
{
    return m_color;
}

inline QRgb QskGradientStop::rgb() const noexcept
{
    return m_color.rgba();
}

inline constexpr bool QskGradientStop::operator==( const QskGradientStop& other ) const noexcept
{
    return ( m_position == other.m_position ) && ( m_color == other.m_color );
}

inline constexpr bool QskGradientStop::operator!=( const QskGradientStop& other ) const noexcept
{
    return ( !( *this == other ) );
}

#ifndef QT_NO_DEBUG_STREAM

class QDebug;

QSK_EXPORT QDebug operator<<( QDebug, const QskGradientStop& );

#endif

typedef QVector< QskGradientStop > QskGradientStops;

QSK_EXPORT bool qskIsMonochrome( const QskGradientStops& ) noexcept;
QSK_EXPORT bool qskIsVisible( const QskGradientStops& ) noexcept;

QSK_EXPORT QskGradientStops qskInterpolatedGradientStops(
    const QskGradientStops&, bool, const QskGradientStops&, bool,
    qreal ratio );

QSK_EXPORT QskGradientStops qskInterpolatedGradientStops(
    const QskGradientStops&, const QColor&, qreal ratio );

QSK_EXPORT QskGradientStops qskInterpolatedGradientStops(
    const QColor&, const QskGradientStops&, qreal ratio );

QSK_EXPORT QskGradientStops qskTransparentGradientStops(
    const QskGradientStops&, qreal ratio );

QSK_EXPORT QskGradientStops qskExtractedGradientStops(
    const QskGradientStops&, qreal from, qreal to );

QSK_EXPORT QskGradientStops qskBuildGradientStops(
    const QVector< QRgb >&, bool discrete = false );

QSK_EXPORT QskGradientStops qskBuildGradientStops(
    const QVector< QColor >&, bool discrete = false );

QSK_EXPORT QskGradientStops qskBuildGradientStops( const QVector< QGradientStop >& );
QSK_EXPORT QVector< QGradientStop > qskToQGradientStops( const QVector< QskGradientStop >& );

#endif
