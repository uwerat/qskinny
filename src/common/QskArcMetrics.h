/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_ARC_METRICS_H
#define QSK_ARC_METRICS_H

#include "QskFunctions.h"

#include <qmetatype.h>

class QVariant;
class QPainterPath;

class QSK_EXPORT QskArcMetrics
{
    Q_GADGET

    Q_PROPERTY( qreal startAngle READ startAngle WRITE setStartAngle )
    Q_PROPERTY( qreal spanAngle READ spanAngle WRITE setSpanAngle )

    Q_PROPERTY( qreal thickness READ thickness WRITE setThickness )
    Q_PROPERTY( Qt::SizeMode sizeMode READ sizeMode WRITE setSizeMode )

  public:
    constexpr QskArcMetrics() noexcept = default;

    constexpr QskArcMetrics( qreal thickness,
        Qt::SizeMode = Qt::AbsoluteSize ) noexcept;

    constexpr QskArcMetrics( qreal startAngle, qreal spanAngle,
        qreal thickness, Qt::SizeMode = Qt::AbsoluteSize ) noexcept;

    bool operator==( const QskArcMetrics& ) const noexcept;
    bool operator!=( const QskArcMetrics& ) const noexcept;

    constexpr bool isNull() const noexcept;

    void setStartAngle( qreal startAngle ) noexcept;
    constexpr qreal startAngle() const noexcept;

    void setSpanAngle( qreal spanAngle ) noexcept;
    constexpr qreal spanAngle() const noexcept;

    constexpr qreal endAngle() const noexcept;
    constexpr qreal angleAtRatio( qreal ratio ) const noexcept;

    bool containsAngle( qreal ) const;
    bool isClosed() const;

    void setThickness( qreal ) noexcept;
    constexpr qreal thickness() const noexcept;

    void setSizeMode( Qt::SizeMode ) noexcept;
    constexpr Qt::SizeMode sizeMode() const noexcept;

    QskArcMetrics interpolated( const QskArcMetrics&,
        qreal value ) const noexcept;

    QskArcMetrics toAbsolute( qreal radiusX, qreal radiusY ) const noexcept;
    QskArcMetrics toAbsolute( qreal radius ) const noexcept;

    QPainterPath painterPath( const QRectF& ellipseRect ) const;

    QRectF boundingRect( const QRectF& ellipseRect ) const;
    QSizeF boundingSize( const QSizeF& ellipseSize ) const;

    QskHashValue hash( QskHashValue seed = 0 ) const noexcept;

    static QVariant interpolate( const QskArcMetrics&,
        const QskArcMetrics&, qreal progress );

  private:
    qreal m_startAngle = 0.0;
    qreal m_spanAngle = 0.0;

    qreal m_thickness = 0.0;
    Qt::SizeMode m_sizeMode = Qt::AbsoluteSize;
};

inline constexpr QskArcMetrics::QskArcMetrics(
        qreal thickness, Qt::SizeMode sizeMode ) noexcept
    : QskArcMetrics( 0.0, 360.0, thickness, sizeMode )
{
}

inline constexpr QskArcMetrics::QskArcMetrics(
        qreal startAngle, qreal spanAngle,
        qreal thickness, Qt::SizeMode sizeMode ) noexcept
    : m_startAngle( startAngle )
    , m_spanAngle( spanAngle )
    , m_thickness( thickness )
    , m_sizeMode( sizeMode )
{
}

inline bool QskArcMetrics::operator==(
    const QskArcMetrics& other ) const noexcept
{
    return ( qskFuzzyCompare( m_thickness, other.m_thickness )
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
    return qFuzzyIsNull( m_thickness ) || qFuzzyIsNull( m_spanAngle );
}

inline constexpr qreal QskArcMetrics::thickness() const noexcept
{
    return m_thickness;
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

inline constexpr qreal QskArcMetrics::angleAtRatio( qreal ratio ) const noexcept
{
    return m_startAngle + ratio * m_spanAngle;
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
