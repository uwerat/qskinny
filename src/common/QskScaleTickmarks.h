/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SCALE_TICKMARKS_H
#define QSK_SCALE_TICKMARKS_H

#include <QskIntervalF.h>
#include <QVector>

class QSK_EXPORT QskScaleTickmarks
{
    Q_GADGET

    Q_PROPERTY( QVector< qreal > majorTicks READ majorTicks WRITE setMajorTicks )
    Q_PROPERTY( QVector< qreal > mediumTicks READ mediumTicks WRITE setMediumTicks )
    Q_PROPERTY( QVector< qreal > minorTicks READ minorTicks WRITE setMinorTicks )

  public:
    enum TickType
    {
        MinorTick,
        MediumTick,
        MajorTick
    };

    Q_ENUM( TickType )

    QskScaleTickmarks();
    ~QskScaleTickmarks();

    bool operator==( const QskScaleTickmarks& ) const noexcept;
    bool operator!=( const QskScaleTickmarks& ) const noexcept;

    int tickCount() const noexcept;
    Q_INVOKABLE int tickCount( TickType ) const noexcept;

    QVector< qreal > ticks( TickType ) const noexcept;
    void setTicks( TickType, const QVector< qreal >& );

    void setMinorTicks( const QVector< qreal >& );
    QVector< qreal > minorTicks() const noexcept;

    void setMediumTicks( const QVector< qreal >& );
    QVector< qreal > mediumTicks() const noexcept;

    void setMajorTicks( const QVector< qreal >& );
    QVector< qreal > majorTicks() const noexcept;

    Q_INVOKABLE qreal tickAt( TickType, int index ) const;

    void invert();
    void reset();

    uint hash( uint seed = 0 ) const;

  private:
    QVector< qreal > m_ticks[ 3 ];
};

inline void QskScaleTickmarks::setMinorTicks( const QVector< qreal >& ticks )
{
    setTicks( MinorTick, ticks );
}

inline QVector< qreal > QskScaleTickmarks::minorTicks() const noexcept
{
    return ticks( MinorTick );
}

inline void QskScaleTickmarks::setMediumTicks( const QVector< qreal >& ticks )
{
    setTicks( MediumTick, ticks );
}

inline QVector< qreal > QskScaleTickmarks::mediumTicks() const noexcept
{
    return ticks( MediumTick );
}

inline void QskScaleTickmarks::setMajorTicks( const QVector< qreal >& ticks )
{
    setTicks( MajorTick, ticks );
}

inline QVector< qreal > QskScaleTickmarks::majorTicks() const noexcept
{
    return ticks( MajorTick );
}

inline bool QskScaleTickmarks::operator!=(
    const QskScaleTickmarks& other ) const noexcept
{
    return !( *this == other );
}

Q_DECLARE_METATYPE( QskScaleTickmarks )

#ifndef QT_NO_DEBUG_STREAM

QSK_EXPORT QDebug operator<<( QDebug, const QskScaleTickmarks& );

#endif

#endif
