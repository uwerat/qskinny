/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRADIENT_H
#define QSK_GRADIENT_H

#include "QskGradientStop.h"

#include <qbrush.h>
#include <qmetatype.h>

class QskLinearDirection;
class QskRadialDirection;
class QskConicDirection;

class QVariant;
class QGradient;

class QSK_EXPORT QskGradient
{
    Q_GADGET

    Q_PROPERTY( Type type READ type )

    Q_PROPERTY( QskLinearDirection linear READ linearDirection WRITE setLinearDirection )
    Q_PROPERTY( QskConicDirection conic READ conicDirection WRITE setConicDirection )
    Q_PROPERTY( QskRadialDirection radial READ radialDirection WRITE setRadialDirection )

    Q_PROPERTY( QskGradientStops stops READ stops WRITE setStops )

    Q_PROPERTY( SpreadMode spreadMode READ spreadMode WRITE setSpreadMode )
    Q_PROPERTY( StretchMode stretchMode READ stretchMode WRITE setStretchMode )

    Q_PROPERTY( bool valid READ isValid )
    Q_PROPERTY( bool visible READ isVisible )
    Q_PROPERTY( bool monochrome READ isMonochrome )

    Q_CLASSINFO( "DefaultProperty", "stops" )

  public:
    enum Type
    {
        Stops,

        Linear,
        Radial,
        Conic
    };
    Q_ENUM( Type )

    enum SpreadMode
    {
        PadSpread,
        ReflectSpread,
        RepeatSpread
    };
    Q_ENUM( SpreadMode )

    enum StretchMode
    {
        NoStretch,
        StretchToSize
    };
    Q_ENUM( StretchMode )

    QskGradient() noexcept;

    QskGradient( Qt::GlobalColor );
    QskGradient( QRgb );
    QskGradient( const QColor& );
    QskGradient( const QColor&, const QColor& );
    QskGradient( QGradient::Preset );
    QskGradient( const QskGradientStops& );

    QskGradient( const QGradient& );

    QskGradient( const QskGradient& ) noexcept;

    ~QskGradient();

    QskGradient& operator=( const QskGradient& ) noexcept;

    bool operator==( const QskGradient& ) const noexcept;
    bool operator!=( const QskGradient& ) const noexcept;

    QskGradient::Type type() const noexcept;

    void setLinearDirection( const QskLinearDirection& );
    void setLinearDirection( qreal, qreal, qreal, qreal );
    void setLinearDirection( Qt::Orientation );
    QskLinearDirection linearDirection() const;

    void setRadialDirection( const QskRadialDirection& );
    void setRadialDirection( const qreal x, qreal y, qreal radius );
    void setRadialDirection( const qreal x, qreal y, qreal radiusX, qreal radiusY );
    QskRadialDirection radialDirection() const;

    void setConicDirection( qreal, qreal );
    void setConicDirection( qreal, qreal, qreal, qreal = 360.0 );
    void setConicDirection( const QskConicDirection& );
    QskConicDirection conicDirection() const;

    void setDirection( Type );
    void resetDirection();

    bool isValid() const noexcept;
    bool isMonochrome() const noexcept;
    bool isVisible() const noexcept;

    void setStops( const QskGradientStops& );
    const QskGradientStops& stops() const noexcept;

    void setStops( const QRgb );
    void setStops( Qt::GlobalColor );
    void setStops( const QColor& );

    void setStops( const QColor&, const QColor& );
    void setStops( QGradient::Preset );

    void clearStops();

    Q_INVOKABLE bool hasStopAt( qreal value ) const noexcept;

    Q_INVOKABLE QColor startColor() const noexcept;
    Q_INVOKABLE QColor endColor() const noexcept;

    QRgb rgbStart() const;
    QRgb rgbEnd() const;

    void setAlpha( int alpha );

    void setSpreadMode( SpreadMode );
    SpreadMode spreadMode() const noexcept;

    void setStretchMode( StretchMode );
    StretchMode stretchMode() const noexcept;

    void reverse();
    QskGradient reversed() const;

    QskGradient interpolated( const QskGradient&, qreal value ) const;

    void stretchTo( const QRectF& );
    QskGradient stretchedTo( const QSizeF& ) const;
    QskGradient stretchedTo( const QRectF& ) const;

    QskGradient effectiveGradient() const;

    static QVariant interpolate( const QskGradient&,
        const QskGradient&, qreal progress );

    // all stops between [from, to] with positions streched into [0,1]
    QskGradient extracted( qreal from, qreal start ) const;

    QskHashValue hash( QskHashValue seed = 0 ) const;

    Q_INVOKABLE qreal stopAt( int index ) const noexcept;
    Q_INVOKABLE QColor colorAt( int index ) const noexcept;

    int stepCount() const noexcept;

    QGradient toQGradient() const;

  private:
    void updateStatusBits() const;

  private:
    QskGradientStops m_stops;

    /*
        Linear: x1, y1, x2, y2
        Radial: centerX, centerY, radiusX, radiusY
        Conic: centerX, centerY, startAngle, spanAngle
     */
    qreal m_values[4] = {};

    unsigned int m_type : 3;
    unsigned int m_spreadMode : 3;
    unsigned int m_stretchMode : 3;

    mutable bool m_isDirty : 1;
    mutable bool m_isValid : 1;
    mutable bool m_isMonchrome : 1;
    mutable bool m_isVisible : 1;
};

Q_DECLARE_METATYPE( QskGradient )

inline QskGradient::QskGradient() noexcept
    : m_type( Stops )
    , m_spreadMode( PadSpread )
    , m_stretchMode( StretchToSize )
    , m_isDirty( false )
    , m_isValid( false )
    , m_isMonchrome( true )
    , m_isVisible( false )
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
    return static_cast< Type >( m_type );
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

inline void QskGradient::setStops( QRgb rgb )
{
    setStops( QColor::fromRgba( rgb ) );
}

inline void QskGradient::setStops( Qt::GlobalColor color )
{
    setStops( QColor( color ) );
}

inline QColor QskGradient::startColor() const noexcept
{
    return m_stops.isEmpty() ? QColor() : m_stops.first().color();
}

inline QColor QskGradient::endColor() const noexcept
{
    return m_stops.isEmpty() ? QColor() : m_stops.last().color();
}

inline QRgb QskGradient::rgbStart() const
{
    return m_stops.isEmpty() ? qRgba( 0, 0, 0, 255 ) : m_stops.first().rgb();
}

inline QRgb QskGradient::rgbEnd() const
{
    return m_stops.isEmpty() ? qRgba( 0, 0, 0, 255 ) : m_stops.last().rgb();
}

inline QskGradient::SpreadMode QskGradient::spreadMode() const noexcept
{
    return static_cast< SpreadMode >( m_spreadMode );
}

inline QskGradient::StretchMode QskGradient::stretchMode() const noexcept
{
    return static_cast< StretchMode >( m_stretchMode );
}

#ifndef QT_NO_DEBUG_STREAM

class QDebug;

QSK_EXPORT QDebug operator<<( QDebug, const QskGradient& );

#endif

#endif
