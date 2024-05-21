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
    Q_PROPERTY( bool proportional READ isProportional WRITE setProportional )

  public:
    constexpr QskArcMetrics() noexcept = default;

    constexpr QskArcMetrics( qreal thickness,
        Qt::SizeMode = Qt::AbsoluteSize, bool proportional = false ) noexcept;

    constexpr QskArcMetrics( qreal startAngle, qreal spanAngle, qreal thickness,
        Qt::SizeMode = Qt::AbsoluteSize, bool proportional = false ) noexcept;

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

    /*
        A proportional arc scales the thickness of the arc according to the
        aspect ratio of the target rectangle. F.e when having a 20x10 rectangle
        the thickness in west/east direction is doubled, while for a
        10x20 rectangle the thickness in north/south direction is doubled.
        This matches the lines that result from a filling with a conic gradient.

        A non proportional arc will have a fixed thickness regardless of
        the aspect ratio.
     */
    void setProportional( bool ) noexcept;
    constexpr bool isProportional() const noexcept;

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

    bool m_relativeSize = false;
    bool m_proportional = false;
};

inline constexpr QskArcMetrics::QskArcMetrics(
        qreal thickness, Qt::SizeMode sizeMode, bool proportional ) noexcept
    : QskArcMetrics( 0.0, 360.0, thickness, sizeMode, proportional )
{
}

inline constexpr QskArcMetrics::QskArcMetrics( qreal startAngle, qreal spanAngle,
        qreal thickness, Qt::SizeMode sizeMode, bool proportional ) noexcept
    : m_startAngle( startAngle )
    , m_spanAngle( spanAngle )
    , m_thickness( thickness )
    , m_relativeSize( sizeMode == Qt::RelativeSize )
    , m_proportional( proportional )
{
}

inline bool QskArcMetrics::operator==(
    const QskArcMetrics& other ) const noexcept
{
    return qskFuzzyCompare( m_thickness, other.m_thickness )
        && qskFuzzyCompare( m_startAngle, other.m_startAngle )
        && qskFuzzyCompare( m_spanAngle, other.m_spanAngle )
        && ( m_relativeSize == other.m_relativeSize )
        && ( m_proportional == other.m_proportional );
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
    return m_relativeSize ? Qt::RelativeSize : Qt::AbsoluteSize;
}

inline constexpr bool QskArcMetrics::isProportional() const noexcept
{
    return m_proportional;
}

#ifndef QT_NO_DEBUG_STREAM

class QDebug;
QSK_EXPORT QDebug operator<<( QDebug, const QskArcMetrics& );

#endif

Q_DECLARE_TYPEINFO( QskArcMetrics, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskArcMetrics )

#endif
