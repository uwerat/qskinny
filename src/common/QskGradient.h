/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRADIENT_H
#define QSK_GRADIENT_H

#include "QskGradientStop.h"

#include <qbrush.h>
#include <qmetatype.h>

class QVariant;

/*
    Don't use QskGradientStops for definitions seen by moc
    Otherwise exporting these interfaces to QML does not work.
 */

class QSK_EXPORT QskGradient
{
    Q_GADGET

    Q_PROPERTY( Orientation orientation READ orientation WRITE setOrientation )
    Q_PROPERTY( QVector< QskGradientStop > stops READ stops WRITE setStops )

    Q_PROPERTY( bool valid READ isValid )
    Q_PROPERTY( bool visible READ isVisible )
    Q_PROPERTY( bool monochrome READ isMonochrome )

  public:
    // TODO: radial/canonical gradients + other diagonal linear gradients
    enum Orientation
    {
        Horizontal,
        Vertical,
        Diagonal
    };
    Q_ENUM( Orientation )

    QskGradient() noexcept = default;

    QskGradient( Orientation ) noexcept;
    QskGradient( Qt::GlobalColor );
    QskGradient( QRgb );
    QskGradient( const QColor& );
    QskGradient( QGradient::Preset );

    QskGradient( Qt::Orientation, const QVector< QskGradientStop >& );
    QskGradient( Qt::Orientation, const QColor&, const QColor& );
    QskGradient( Qt::Orientation, QGradient::Preset );

    QskGradient( Orientation, const QVector< QskGradientStop >& );
    QskGradient( Orientation, const QColor&, const QColor& );
    QskGradient( Orientation, QGradient::Preset );

    ~QskGradient();

    bool operator==( const QskGradient& ) const noexcept;
    bool operator!=( const QskGradient& ) const noexcept;

    void setOrientation( Qt::Orientation ) noexcept;
    void setOrientation( Orientation ) noexcept;
    Orientation orientation() const noexcept;

    bool isHorizontal() const noexcept;
    bool isVertical() const noexcept;
    bool isTilted() const noexcept;

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

  private:
    void updateStatusBits() const;

  private:
    QVector< QskGradientStop > m_stops;

    Orientation m_orientation = Vertical;

    mutable bool m_isDirty = false;
    mutable bool m_isValid = false;
    mutable bool m_isMonchrome = true;
    mutable bool m_isVisible = false;
};

Q_DECLARE_METATYPE( QskGradient )

inline QskGradient::QskGradient( Qt::GlobalColor color )
    : QskGradient( QColor( color ) )
{
}

inline QskGradient::QskGradient( QRgb rgb )
    : QskGradient( QColor::fromRgba( rgb ) )
{
}

inline QskGradient::QskGradient( QGradient::Preset preset )
    : QskGradient( Vertical, preset )
{
}

inline bool QskGradient::operator!=( const QskGradient& other ) const noexcept
{
    return !( *this == other );
}

inline QskGradient::Orientation QskGradient::orientation() const noexcept
{
    return m_orientation;
}

inline bool QskGradient::isHorizontal() const noexcept
{
    return orientation() == Horizontal;
}

inline bool QskGradient::isVertical() const noexcept
{
    return orientation() == Vertical;
}

inline bool QskGradient::isTilted() const noexcept
{
    return orientation() == Diagonal;
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

#ifndef QT_NO_DEBUG_STREAM

class QDebug;

QSK_EXPORT QDebug operator<<( QDebug, const QskGradient& );

#endif

#endif
