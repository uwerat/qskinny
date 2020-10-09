/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_BORDER_METRICS_H
#define QSK_BOX_BORDER_METRICS_H

#include "QskMargins.h"

#include <qmetatype.h>
#include <qnamespace.h>
#include <qsize.h>

class QVariant;

class QSK_EXPORT QskBoxBorderMetrics
{
  public:
    constexpr QskBoxBorderMetrics() noexcept;

    constexpr QskBoxBorderMetrics(
        qreal width, Qt::SizeMode = Qt::AbsoluteSize ) noexcept;

    constexpr QskBoxBorderMetrics( qreal left, qreal top,
        qreal right, qreal bottom, Qt::SizeMode = Qt::AbsoluteSize ) noexcept;

    constexpr QskBoxBorderMetrics( const QskMargins& widths,
        Qt::SizeMode sizeMode = Qt::AbsoluteSize ) noexcept;

    constexpr QskBoxBorderMetrics( const QMarginsF& widths,
        Qt::SizeMode sizeMode = Qt::AbsoluteSize ) noexcept;

    constexpr bool operator==( const QskBoxBorderMetrics& ) const noexcept;
    constexpr bool operator!=( const QskBoxBorderMetrics& ) const noexcept;

    constexpr bool isNull() const noexcept;

    void setWidthAt( Qt::Edges, qreal width ) noexcept;
    qreal widthAt( Qt::Edge ) const noexcept;

    void setWidths( const QskMargins& ) noexcept;
    constexpr const QskMargins& widths() const noexcept;

    void setSizeMode( Qt::SizeMode ) noexcept;
    constexpr Qt::SizeMode sizeMode() const noexcept;

    QskBoxBorderMetrics interpolated(
        const QskBoxBorderMetrics&, qreal value ) const noexcept;

    QskBoxBorderMetrics toAbsolute( const QSizeF& ) const noexcept;

    uint hash( uint seed = 0 ) const noexcept;

    static QVariant interpolate( const QskBoxBorderMetrics&,
        const QskBoxBorderMetrics&, qreal progress );

  private:
    QskMargins m_widths;
    Qt::SizeMode m_sizeMode;
};

inline constexpr QskBoxBorderMetrics::QskBoxBorderMetrics() noexcept
    : m_sizeMode( Qt::AbsoluteSize )
{
}

inline constexpr QskBoxBorderMetrics::QskBoxBorderMetrics(
        qreal width, Qt::SizeMode sizeMode ) noexcept
    : m_widths( width )
    , m_sizeMode( sizeMode )
{
}

inline constexpr QskBoxBorderMetrics::QskBoxBorderMetrics(
        const QMarginsF& widths, Qt::SizeMode sizeMode ) noexcept
    : m_widths( widths )
    , m_sizeMode( sizeMode )
{
}

inline constexpr QskBoxBorderMetrics::QskBoxBorderMetrics(
        const QskMargins& widths, Qt::SizeMode sizeMode ) noexcept
    : m_widths( widths )
    , m_sizeMode( sizeMode )
{
}

inline constexpr QskBoxBorderMetrics::QskBoxBorderMetrics(
        qreal left, qreal top, qreal right, qreal bottom, Qt::SizeMode sizeMode ) noexcept
    : m_widths( left, top, right, bottom )
    , m_sizeMode( sizeMode )
{
}

inline constexpr bool QskBoxBorderMetrics::operator==(
    const QskBoxBorderMetrics& other ) const noexcept
{
    return ( m_sizeMode == other.m_sizeMode ) && ( m_widths == other.m_widths );
}

inline constexpr bool QskBoxBorderMetrics::operator!=(
    const QskBoxBorderMetrics& other ) const noexcept
{
    return !( *this == other );
}

inline constexpr bool QskBoxBorderMetrics::isNull() const noexcept
{
    return m_widths.isNull();
}

inline constexpr const QskMargins& QskBoxBorderMetrics::widths() const noexcept
{
    return m_widths;
}

inline qreal QskBoxBorderMetrics::widthAt( Qt::Edge edge ) const noexcept
{
    return m_widths.marginAt( edge );
}

inline constexpr Qt::SizeMode QskBoxBorderMetrics::sizeMode() const noexcept
{
    return m_sizeMode;
}

#ifndef QT_NO_DEBUG_STREAM

class QDebug;
QSK_EXPORT QDebug operator<<( QDebug, const QskBoxBorderMetrics& );

#endif

Q_DECLARE_TYPEINFO( QskBoxBorderMetrics, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskBoxBorderMetrics )

#endif
