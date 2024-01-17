/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_STIPPLE_METRICS_H
#define QSK_STIPPLE_METRICS_H

#include "QskGlobal.h"

#include <qmetatype.h>
#include <qvector.h>
#include <qnamespace.h>

class QPen;

class QSK_EXPORT QskStippleMetrics
{
    Q_GADGET

    Q_PROPERTY( qreal offset READ offset WRITE setOffset )
    Q_PROPERTY( QVector< qreal > pattern READ pattern WRITE setPattern )

  public:
    QskStippleMetrics( Qt::PenStyle = Qt::SolidLine );
    QskStippleMetrics( const QPen& );
    QskStippleMetrics( const QVector< qreal >&, qreal offset = 0.0 );

    bool operator==( const QskStippleMetrics& ) const noexcept;
    bool operator!=( const QskStippleMetrics& ) const noexcept;

    bool isValid() const noexcept;
    bool isSolid() const noexcept;

    void setOffset( qreal offset ) noexcept;
    qreal offset() const noexcept;

    void setPattern( const QVector< qreal >& );
    QVector< qreal > pattern() const;

    QskStippleMetrics interpolated(
        const QskStippleMetrics&, qreal value ) const;

    static QVariant interpolate( const QskStippleMetrics&,
        const QskStippleMetrics&, qreal progress );

    QskHashValue hash( QskHashValue seed = 0 ) const noexcept;

  private:
    qreal m_offset = 0.0;
    QVector< qreal > m_pattern;
};

inline QskStippleMetrics::QskStippleMetrics(
        const QVector< qreal >& pattern, qreal offset )
    : m_offset( offset )
    , m_pattern( pattern )
{
}

inline qreal QskStippleMetrics::offset() const noexcept
{
    return m_offset;
}

inline QVector< qreal > QskStippleMetrics::pattern() const
{
    return m_pattern;
}

inline bool QskStippleMetrics::operator==(
    const QskStippleMetrics& other ) const noexcept
{
    return ( m_offset == other.m_offset )
        && ( m_pattern == other.m_pattern );
}

inline bool QskStippleMetrics::operator!=(
    const QskStippleMetrics& other ) const noexcept
{
    return !( *this == other );
}

inline bool QskStippleMetrics::isValid() const noexcept
{
    return !m_pattern.isEmpty();
}

inline bool QskStippleMetrics::isSolid() const noexcept
{
    return m_pattern.count() == 1;
}

QSK_EXPORT QVector< qreal > qskDashPattern( Qt::PenStyle );

#ifndef QT_NO_DEBUG_STREAM

class QDebug;
QSK_EXPORT QDebug operator<<( QDebug, const QskStippleMetrics& );

#endif

Q_DECLARE_METATYPE( QskStippleMetrics )

#endif
