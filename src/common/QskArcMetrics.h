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

    constexpr QskArcMetrics( qreal startAngle, qreal spanAngle, qreal thickness,
        Qt::SizeMode = Qt::AbsoluteSize ) noexcept;

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

    QskArcMetrics toAbsolute( const QSizeF& ) const noexcept;
    QskArcMetrics toAbsolute( qreal radiusX, qreal radiusY ) const noexcept;
    QskArcMetrics toAbsolute( qreal radius ) const noexcept;

    /*
        The arc is interpolated by pairs of points, where one point is on
        the outer and the other on the inner side of the arc. The length between
        these points depends on the thickness.

        When radial is set the inner point lies on the line between the outer point
        and the center of the arc. This corresponds to the lines of a conic gradient.
          
        Otherwise the line between the inner and outer point is orthogonal to the
        tangent at the point in the middle of the arc. This is how the width
        of the pen is expanded by QPainter::drawArc.

        Note, that the radial flag is irrelevant for circular arcs as the tangent
        is always orthogonal to any point on the circle.
     */
    QPainterPath painterPath( const QRectF& ellipseRect, bool radial = false ) const;

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
};

inline constexpr QskArcMetrics::QskArcMetrics(
        qreal thickness, Qt::SizeMode sizeMode ) noexcept
    : QskArcMetrics( 0.0, 360.0, thickness, sizeMode )
{
}

inline constexpr QskArcMetrics::QskArcMetrics( qreal startAngle, qreal spanAngle,
        qreal thickness, Qt::SizeMode sizeMode ) noexcept
    : m_startAngle( startAngle )
    , m_spanAngle( spanAngle )
    , m_thickness( thickness )
    , m_relativeSize( sizeMode == Qt::RelativeSize )
{
}

inline bool QskArcMetrics::operator==(
    const QskArcMetrics& other ) const noexcept
{
    return qskFuzzyCompare( m_thickness, other.m_thickness )
        && qskFuzzyCompare( m_startAngle, other.m_startAngle )
        && qskFuzzyCompare( m_spanAngle, other.m_spanAngle )
        && ( m_relativeSize == other.m_relativeSize );
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

#ifndef QT_NO_DEBUG_STREAM

class QDebug;
QSK_EXPORT QDebug operator<<( QDebug, const QskArcMetrics& );

#endif

Q_DECLARE_TYPEINFO( QskArcMetrics, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskArcMetrics )

#endif
