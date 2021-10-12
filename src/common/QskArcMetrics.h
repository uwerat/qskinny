/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_ARC_METRICS_H
#define QSK_ARC_METRICS_H

#include "QskFunctions.h"

#include <qmetatype.h>

class QVariant;

class QSK_EXPORT QskArcMetrics
{
    Q_GADGET

    Q_PROPERTY( QskMargins widths READ widths WRITE setWidths )
    Q_PROPERTY( Qt::SizeMode sizeMode READ sizeMode WRITE setSizeMode )

  public:
    constexpr QskArcMetrics() noexcept;

    constexpr QskArcMetrics( qreal startAngle, qreal endAngle,
        qreal width, Qt::SizeMode = Qt::AbsoluteSize ) noexcept;

    constexpr bool operator==( const QskArcMetrics& ) const noexcept;
    constexpr bool operator!=( const QskArcMetrics& ) const noexcept;

    constexpr bool isNull() const noexcept;

    void setStartAngle( qreal ) noexcept;
    constexpr qreal startAngle() const noexcept;
    void setEndAngle( qreal ) noexcept;
    constexpr qreal endAngle() const noexcept;

    void setWidth( qreal width ) noexcept;
    constexpr qreal width() const noexcept;

    void setSizeMode( Qt::SizeMode ) noexcept;
    constexpr Qt::SizeMode sizeMode() const noexcept;

    uint hash( uint seed = 0 ) const noexcept;

    static QVariant interpolate( const QskArcMetrics&,
        const QskArcMetrics&, qreal progress );

  private:
    qreal m_startAngle;
    qreal m_endAngle;
    qreal m_width;
    Qt::SizeMode m_sizeMode;
};

inline constexpr QskArcMetrics::QskArcMetrics() noexcept
    : m_startAngle( 0 )
    , m_endAngle( 0 )
    , m_width( 0 )
    , m_sizeMode( Qt::AbsoluteSize )
{
}

inline constexpr QskArcMetrics::QskArcMetrics( qreal startAngle, qreal endAngle,
    qreal width, Qt::SizeMode sizeMode ) noexcept
    : m_startAngle( startAngle )
    , m_endAngle( endAngle )
    , m_width( width )
    , m_sizeMode( sizeMode )
{
}

inline constexpr bool QskArcMetrics::operator==(
    const QskArcMetrics& other ) const noexcept
{
    return ( qskFuzzyCompare( m_startAngle, other.m_startAngle )
             && qskFuzzyCompare( m_endAngle, other.m_endAngle )
             && qskFuzzyCompare( m_width, other.m_width )
             && m_sizeMode == other.m_sizeMode );
}

inline constexpr bool QskArcMetrics::operator!=(
    const QskArcMetrics& other ) const noexcept
{
    return !( *this == other );
}

inline constexpr bool QskArcMetrics::isNull() const noexcept
{
    return qFuzzyIsNull( m_startAngle )
            && qFuzzyIsNull( m_endAngle )
            && qFuzzyIsNull( m_width )
            && m_sizeMode == Qt::AbsoluteSize;
}

inline constexpr qreal QskArcMetrics::width() const noexcept
{
    return m_width;
}

inline constexpr Qt::SizeMode QskArcMetrics::sizeMode() const noexcept
{
    return m_sizeMode;
}

#ifndef QT_NO_DEBUG_STREAM

class QDebug;
QSK_EXPORT QDebug operator<<( QDebug, const QskArcMetrics& );

#endif

Q_DECLARE_TYPEINFO( QskArcMetrics, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskArcMetrics )

#endif
