/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SHADOW_METRICS_H
#define QSK_SHADOW_METRICS_H

#include "QskGlobal.h"

#include <qmetatype.h>
#include <qnamespace.h>
#include <qpoint.h>

class QVariant;

class QSK_EXPORT QskShadowMetrics
{
    Q_GADGET

    Q_PROPERTY( QPointF offset READ offset WRITE setOffset )
    Q_PROPERTY( qreal spreadRadius READ spreadRadius WRITE setSpreadRadius )
    Q_PROPERTY( qreal blurRadius READ blurRadius WRITE setBlurRadius )

    Q_PROPERTY( Qt::SizeMode sizeMode READ sizeMode WRITE setSizeMode )

  public:
    constexpr QskShadowMetrics( const QPointF& offset = QPointF() ) noexcept;

    constexpr QskShadowMetrics( qreal spreadRadius, qreal blurRadius ) noexcept;

    constexpr QskShadowMetrics( qreal spreadRadius, qreal blurRadius,
        const QPointF& offset, Qt::SizeMode = Qt::AbsoluteSize ) noexcept;

    constexpr bool operator==( const QskShadowMetrics& ) const noexcept;
    constexpr bool operator!=( const QskShadowMetrics& ) const noexcept;

    bool isNull() const noexcept;

    void setSpreadRadius( qreal ) noexcept;
    constexpr qreal spreadRadius() const noexcept;

    void setBlurRadius( qreal ) noexcept;
    constexpr qreal blurRadius() const noexcept;

    constexpr qreal totalRadius() const noexcept;

    void setOffsetX( qreal dx ) noexcept;
    void setOffsetY( qreal dy ) noexcept;

    void setOffset( qreal dx, qreal dy ) noexcept;
    void setOffset( const QPointF& ) noexcept;

    constexpr QPointF offset() const noexcept;

    void setSizeMode( Qt::SizeMode ) noexcept;
    constexpr Qt::SizeMode sizeMode() const noexcept;

    QskShadowMetrics interpolated(
        const QskShadowMetrics&, qreal value ) const noexcept;

    QskShadowMetrics toAbsolute( const QSizeF& ) const noexcept;

    QRectF shadowRect( const QRectF& sourceRect ) const;

    QskHashValue hash( QskHashValue seed = 0 ) const noexcept;

    static QVariant interpolate( const QskShadowMetrics&,
        const QskShadowMetrics&, qreal progress );

  private:
    QPointF m_offset;
    qreal m_spreadRadius = 0.0;
    qreal m_blurRadius = 0.0;
    Qt::SizeMode m_sizeMode = Qt::AbsoluteSize;
};

inline constexpr QskShadowMetrics::QskShadowMetrics( const QPointF& offset ) noexcept
    : m_offset( offset )
{
}

inline constexpr QskShadowMetrics::QskShadowMetrics(
        qreal spreadRadius, qreal blurRadius ) noexcept
    : m_spreadRadius( spreadRadius )
    , m_blurRadius( blurRadius )
{
}

inline constexpr QskShadowMetrics::QskShadowMetrics(
        qreal spreadRadius, qreal blurRadius,
        const QPointF& offset, Qt::SizeMode sizeMode ) noexcept
    : m_offset( offset )
    , m_spreadRadius( spreadRadius )
    , m_blurRadius( blurRadius )
    , m_sizeMode( sizeMode )
{
}

inline constexpr bool QskShadowMetrics::operator==(
    const QskShadowMetrics& other ) const noexcept
{
    return ( m_sizeMode == other.m_sizeMode )
        && ( m_offset == other.m_offset )
        && ( m_spreadRadius == other.m_spreadRadius )
        && ( m_blurRadius == other.m_blurRadius )
        && ( m_sizeMode == other.m_sizeMode );
}

inline constexpr bool QskShadowMetrics::operator!=(
    const QskShadowMetrics& other ) const noexcept
{
    return !( *this == other );
}

inline void QskShadowMetrics::setSpreadRadius( qreal radius ) noexcept
{
    m_spreadRadius = radius;
}

inline constexpr qreal QskShadowMetrics::spreadRadius() const noexcept
{
    return m_spreadRadius;
}

inline void QskShadowMetrics::setBlurRadius( qreal radius ) noexcept
{
    m_blurRadius = radius;
}

inline constexpr qreal QskShadowMetrics::blurRadius() const noexcept
{
    return m_blurRadius;
}

inline constexpr qreal QskShadowMetrics::totalRadius() const noexcept
{
    return m_spreadRadius + m_blurRadius;
}

inline void QskShadowMetrics::setSizeMode( Qt::SizeMode sizeMode ) noexcept
{
    m_sizeMode = sizeMode;
}

inline constexpr Qt::SizeMode QskShadowMetrics::sizeMode() const noexcept
{
    return m_sizeMode;
}

inline void QskShadowMetrics::setOffsetX( qreal dx ) noexcept
{
    m_offset.rx() = dx;
}

inline void QskShadowMetrics::setOffsetY( qreal dy ) noexcept
{
    m_offset.ry() = dy;
}

inline void QskShadowMetrics::setOffset( qreal dx, qreal dy ) noexcept
{
    m_offset.rx() = dx;
    m_offset.ry() = dy;
}

inline void QskShadowMetrics::setOffset( const QPointF& offset ) noexcept
{
    m_offset = offset;
}

inline constexpr QPointF QskShadowMetrics::offset() const noexcept
{
    return m_offset;
}

inline bool QskShadowMetrics::isNull() const noexcept
{
    return m_offset.isNull() &&
        ( m_spreadRadius == 0.0 ) && ( m_blurRadius == 0.0 );
}

#ifndef QT_NO_DEBUG_STREAM

QSK_EXPORT QDebug operator<<( QDebug, const QskShadowMetrics& );

#endif

Q_DECLARE_TYPEINFO( QskShadowMetrics, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskShadowMetrics )

#endif
