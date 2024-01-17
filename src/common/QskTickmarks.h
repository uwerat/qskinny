/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TICKMARKS_H
#define QSK_TICKMARKS_H

#include <QskIntervalF.h>
#include <QVector>

class QSK_EXPORT QskTickmarks
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

    QskTickmarks();
    QskTickmarks( const QVector< qreal >& minorTicks,
        const QVector< qreal >& mediumTicks, const QVector< qreal >& majorTicks );

    ~QskTickmarks();

    bool operator==( const QskTickmarks& ) const noexcept;
    bool operator!=( const QskTickmarks& ) const noexcept;

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

    QskHashValue hash( QskHashValue seed = 0 ) const noexcept;

  private:
    QVector< qreal > m_ticks[ 3 ];
};

inline void QskTickmarks::setMinorTicks( const QVector< qreal >& ticks )
{
    setTicks( MinorTick, ticks );
}

inline QVector< qreal > QskTickmarks::minorTicks() const noexcept
{
    return ticks( MinorTick );
}

inline void QskTickmarks::setMediumTicks( const QVector< qreal >& ticks )
{
    setTicks( MediumTick, ticks );
}

inline QVector< qreal > QskTickmarks::mediumTicks() const noexcept
{
    return ticks( MediumTick );
}

inline void QskTickmarks::setMajorTicks( const QVector< qreal >& ticks )
{
    setTicks( MajorTick, ticks );
}

inline QVector< qreal > QskTickmarks::majorTicks() const noexcept
{
    return ticks( MajorTick );
}

inline bool QskTickmarks::operator!=(
    const QskTickmarks& other ) const noexcept
{
    return !( *this == other );
}

Q_DECLARE_METATYPE( QskTickmarks )

#ifndef QT_NO_DEBUG_STREAM

QSK_EXPORT QDebug operator<<( QDebug, const QskTickmarks& );

#endif

#endif
