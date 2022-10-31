/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRADIENT_H
#define QSK_GRADIENT_H

#include "QskGradientStop.h"

#include <qbrush.h>
#include <qmetatype.h>

class QskLinearGradient;
class QskRadialGradient;
class QskConicGradient;

class QVariant;

/*
    Don't use QskGradientStops for definitions seen by moc
    Otherwise exporting these interfaces to QML does not work.
 */

class QSK_EXPORT QskGradient
{
    Q_GADGET

    Q_PROPERTY( QVector< QskGradientStop > stops READ stops WRITE setStops )

    Q_PROPERTY( bool valid READ isValid )
    Q_PROPERTY( bool visible READ isVisible )
    Q_PROPERTY( bool monochrome READ isMonochrome )

  public:
    enum Type
    {
        Stops,

        Linear,
        Radial,
        Conic
    };
    Q_ENUM( Type )

    QskGradient() noexcept = default;

    QskGradient( Qt::GlobalColor );
    QskGradient( QRgb );
    QskGradient( const QColor& );
    QskGradient( const QColor&, const QColor& );
    QskGradient( QGradient::Preset );
    QskGradient( const QVector< QskGradientStop >& );

    QskGradient( const QskGradient& ) noexcept;

    ~QskGradient();

    QskGradient& operator=( const QskGradient& ) noexcept;

    bool operator==( const QskGradient& ) const noexcept;
    bool operator!=( const QskGradient& ) const noexcept;

    QskGradient::Type type() const noexcept;

    bool isValid() const noexcept;
    bool isMonochrome() const noexcept;
    bool isVisible() const noexcept;

    Q_INVOKABLE void setStops( const QVector< QskGradientStop >& );
    Q_INVOKABLE const QVector< QskGradientStop >& stops() const noexcept;

    void setStops( const QColor& );
    void setStops( const QColor&, const QColor& );
    void setStops( QGradient::Preset );

    void clearStops();

    Q_INVOKABLE bool hasStopAt( qreal value ) const noexcept;

    Q_INVOKABLE QColor startColor() const noexcept;
    Q_INVOKABLE QColor endColor() const noexcept;

    void setAlpha( int alpha );

    void setSpread( QGradient::Spread );
    QGradient::Spread spread() const noexcept;

    void reverse();
    QskGradient reversed() const;

    QskGradient interpolated( const QskGradient&, qreal value ) const;

    static QVariant interpolate( const QskGradient&,
        const QskGradient&, qreal progress );

    // all stops between [from, to] with positions streched into [0,1]
    QskGradient extracted( qreal from, qreal start ) const;

    QskHashValue hash( QskHashValue seed ) const;

    Q_INVOKABLE qreal stopAt( int index ) const noexcept;
    Q_INVOKABLE QColor colorAt( int index ) const noexcept;

    int stepCount() const noexcept;

    QskLinearGradient& asLinearGradient();
    const QskLinearGradient& asLinearGradient() const;

    QskRadialGradient& asRadialGradient();
    const QskRadialGradient& asRadialGradient() const;

    QskConicGradient& asConicGradient();
    const QskConicGradient& asConicGradient() const;

  private:
    friend class QskLinearGradient;
    friend class QskRadialGradient;
    friend class QskConicGradient;

    QskGradient( Type ) noexcept;
    QskGradient( Type, qreal, qreal, qreal, qreal ) noexcept;

    void updateStatusBits() const;

  private:
    QVector< QskGradientStop > m_stops;

    /*
        Linear: x1, y1, x2, y2
        Radial: centerX, centerY, radius, n/a
        Conic: centerX, centerY, startAngle, spanAngle
     */
    qreal m_values[4] = {};

    Type m_type = Stops;
    QGradient::Spread m_spread = QGradient::PadSpread;

    mutable bool m_isDirty = false;
    mutable bool m_isValid = false;
    mutable bool m_isMonchrome = true;
    mutable bool m_isVisible = false;
};

Q_DECLARE_METATYPE( QskGradient )

inline QskGradient::QskGradient( QskGradient::Type type ) noexcept
    : m_type( type )
{
}

inline QskGradient::QskGradient( QskGradient::Type type,
        qreal v1, qreal v2, qreal v3, qreal v4  ) noexcept
    : m_values{ v1, v2, v3, v4 }
    , m_type( type )
{
}

inline QskGradient::QskGradient( Qt::GlobalColor color )
    : QskGradient( QColor( color ) )
{
}

inline QskGradient::QskGradient( QRgb rgb )
    : QskGradient( QColor::fromRgba( rgb ) )
{
}

inline bool QskGradient::operator!=( const QskGradient& other ) const noexcept
{
    return !( *this == other );
}

inline QskGradient::Type QskGradient::type() const noexcept
{
    return m_type;
}

inline const QskGradientStops& QskGradient::stops() const noexcept
{
#if 1
    /*
        Returning a const& so that it is possible to write:
            for ( const auto& stop : qAsConst( gradient.stops() ) )

        Once we have changed QskGradientStop from QColor to QRgb
        we should check if there is a better solution possible
     */
#endif
    return m_stops;
}

inline QColor QskGradient::startColor() const noexcept
{
    return m_stops.isEmpty() ? QColor() : m_stops.first().color();
}

inline QColor QskGradient::endColor() const noexcept
{
    return m_stops.isEmpty() ? QColor() : m_stops.last().color();
}

inline QGradient::Spread QskGradient::spread() const noexcept
{
    return m_spread;
}

#ifndef QT_NO_DEBUG_STREAM

class QDebug;

QSK_EXPORT QDebug operator<<( QDebug, const QskGradient& );

#endif

#endif
