/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
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

QSK_EXPORT QColor qskInterpolatedColorAt( const QskGradientStops&, qreal pos ) noexcept;

QSK_EXPORT bool qskIsMonochrome( const QskGradientStops& ) noexcept;
QSK_EXPORT bool qskIsVisible( const QskGradientStops& ) noexcept;

QSK_EXPORT QskGradientStops qskInterpolatedGradientStops(
    const QskGradientStops&, bool, const QskGradientStops&, bool,
    qreal ratio );

// interpolating colors in direction of a color.
QSK_EXPORT QskGradientStops qskInterpolatedGradientStops(
    const QskGradientStops&, const QColor&, qreal ratio );

// interpolating colors starting from a color.
QSK_EXPORT QskGradientStops qskInterpolatedGradientStops(
    const QColor&, const QskGradientStops&, qreal ratio );

// interpolating the opacity of the colors
QSK_EXPORT QskGradientStops qskTransparentGradientStops(
    const QskGradientStops&, qreal ratio );

// extracting the colors of [from, to ] and stretching them to [0.0, 1.0] 
QSK_EXPORT QskGradientStops qskExtractedGradientStops(
    const QskGradientStops&, qreal from, qreal to );

// reverting the color stops
QSK_EXPORT QskGradientStops qskRevertedGradientStops( const QskGradientStops& );

/*
   creating equidistant color stops from a list of colors.
   when discrete is true the result will contain 2 stops at each position
   one with the previous and one with the following color so that the
   interval [pos1-pos2] will be monochrome.
 */
QSK_EXPORT QskGradientStops qskBuildGradientStops(
    const QVector< QRgb >&, bool discrete = false );

QSK_EXPORT QskGradientStops qskBuildGradientStops(
    const QVector< QColor >&, bool discrete = false );

/*
   convert color stops from/to a vector of QGradientStop, that can be
   used for QGradients.
 */
QSK_EXPORT QskGradientStops qskFromQGradientStops( const QVector< QGradientStop >& );
QSK_EXPORT QVector< QGradientStop > qskToQGradientStops( const QVector< QskGradientStop >& );

#endif
