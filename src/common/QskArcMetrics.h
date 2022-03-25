/******************************************************************************
 * QSkinny - Copyright (C) 2021 Uwe Rathmann
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

    Q_PROPERTY( qreal width READ width WRITE setWidth )
    Q_PROPERTY( qreal startAngle READ startAngle WRITE setStartAngle )
    Q_PROPERTY( qreal spanAngle READ spanAngle WRITE setSpanAngle )
    Q_PROPERTY( Qt::SizeMode sizeMode READ sizeMode WRITE setSizeMode )

  public:
    constexpr QskArcMetrics() noexcept;
    constexpr QskArcMetrics( qreal width, qreal startAngle, qreal spanAngle,
        Qt::SizeMode = Qt::AbsoluteSize ) noexcept;

    bool operator==( const QskArcMetrics& ) const noexcept;
    bool operator!=( const QskArcMetrics& ) const noexcept;

    constexpr bool isNull() const noexcept;

    void setWidth( qreal width ) noexcept;
    constexpr qreal width() const noexcept;

    void setStartAngle( qreal startAngle ) noexcept;
    constexpr qreal startAngle() const noexcept;

    void setSpanAngle( qreal spanAngle ) noexcept;
    constexpr qreal spanAngle() const noexcept;

    constexpr qreal endAngle() const noexcept;

    void setSizeMode( Qt::SizeMode ) noexcept;
    constexpr Qt::SizeMode sizeMode() const noexcept;

    QskArcMetrics interpolated( const QskArcMetrics&,
        qreal value ) const noexcept;

    QskArcMetrics toAbsolute( const QSizeF& ) const noexcept;

    QskHashValue hash( QskHashValue seed = 0 ) const noexcept;

    static QVariant interpolate( const QskArcMetrics&,
        const QskArcMetrics&, qreal progress );

  private:
    qreal m_width;
    qreal m_startAngle;
    qreal m_spanAngle;
    Qt::SizeMode m_sizeMode;
};

inline constexpr QskArcMetrics::QskArcMetrics() noexcept
    : m_width( 0 )
    , m_startAngle( 0 )
    , m_spanAngle( 0 )
    , m_sizeMode( Qt::AbsoluteSize )
{
}

inline constexpr QskArcMetrics::QskArcMetrics(
        qreal width, qreal startAngle, qreal spanAngle,
        Qt::SizeMode sizeMode ) noexcept
    : m_width( width )
    , m_startAngle( startAngle )
    , m_spanAngle( spanAngle )
    , m_sizeMode( sizeMode )
{
}

inline bool QskArcMetrics::operator==(
    const QskArcMetrics& other ) const noexcept
{
    return ( qskFuzzyCompare( m_width, other.m_width )
        && qskFuzzyCompare( m_startAngle, other.m_startAngle )
        && qskFuzzyCompare( m_spanAngle, other.m_spanAngle )
        && m_sizeMode == other.m_sizeMode );
}

inline bool QskArcMetrics::operator!=(
    const QskArcMetrics& other ) const noexcept
{
    return !( *this == other );
}

inline constexpr bool QskArcMetrics::isNull() const noexcept
{
    return qFuzzyIsNull( m_width ) || qFuzzyIsNull( m_spanAngle );
}

inline constexpr qreal QskArcMetrics::width() const noexcept
{
    return m_width;
}

inline constexpr qreal QskArcMetrics::startAngle() const noexcept
{
    return m_startAngle;
}

inline constexpr qreal QskArcMetrics::spanAngle() const noexcept
{
    return m_spanAngle;
}

inline constexpr qreal QskArcMetrics::endAngle() const noexcept
{
    return m_startAngle + m_spanAngle;
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
