/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_ARC_BORDER_METRICS_H
#define QSK_ARC_BORDER_METRICS_H

#include "QskFunctions.h"
#include "QskNamespace.h"

#include <qmetatype.h>
#include <qnamespace.h>
#include <qsize.h>

class QVariant;

class QSK_EXPORT QskArcBorderMetrics
{
    Q_GADGET

    Q_PROPERTY( qreal outerWidth READ outerWidth WRITE setOuterWidth )
    Q_PROPERTY( qreal innerWidth READ innerWidth WRITE setInnerWidth )
    Q_PROPERTY( qreal startWidth READ startWidth WRITE setStartWidth )
    Q_PROPERTY( qreal endWidth READ endWidth WRITE setEndWidth )
    Q_PROPERTY( Qt::SizeMode sizeMode READ sizeMode WRITE setSizeMode )

  public:
    constexpr QskArcBorderMetrics() noexcept;

    constexpr QskArcBorderMetrics( qreal,
        Qt::SizeMode = Qt::AbsoluteSize ) noexcept;

    constexpr QskArcBorderMetrics( qreal, qreal, qreal, qreal,
        Qt::SizeMode = Qt::AbsoluteSize ) noexcept;

    constexpr bool operator==( const QskArcBorderMetrics& ) const noexcept;
    constexpr bool operator!=( const QskArcBorderMetrics& ) const noexcept;

    constexpr bool isNull() const noexcept;

    void setWidth( Qsk::ArcPosition, qreal width ) noexcept;
    constexpr qreal width( Qsk::ArcPosition ) const noexcept;

    void setWidths( qreal ) noexcept;
    void setWidths( qreal, qreal, qreal, qreal ) noexcept;

    void setOuterWidth( qreal ) noexcept;
    void setInnerWidth( qreal ) noexcept;
    void setStartWidth( qreal ) noexcept;
    void setEndWidth( qreal ) noexcept;
    constexpr qreal outerWidth() const noexcept;
    constexpr qreal innerWidth() const noexcept;
    constexpr qreal startWidth() const noexcept;
    constexpr qreal endWidth() const noexcept;

    void setSizeMode( Qt::SizeMode ) noexcept;
    constexpr Qt::SizeMode sizeMode() const noexcept;

    QskArcBorderMetrics interpolated(
        const QskArcBorderMetrics&, qreal value ) const noexcept;

    QskArcBorderMetrics toAbsolute( const QSizeF& ) const noexcept;

    uint hash( uint seed = 0 ) const noexcept;

    static QVariant interpolate( const QskArcBorderMetrics&,
        const QskArcBorderMetrics&, qreal progress );

  private:
    qreal m_widths[ 4 ];
    Qt::SizeMode m_sizeMode;
};

inline constexpr QskArcBorderMetrics::QskArcBorderMetrics() noexcept
    : m_widths{ 0.0, 0.0, 0.0, 0.0 }
    , m_sizeMode( Qt::AbsoluteSize )
{
}

inline constexpr QskArcBorderMetrics::QskArcBorderMetrics(
        qreal width, Qt::SizeMode sizeMode ) noexcept
    : m_widths{ width, width, width, width }
    , m_sizeMode( sizeMode )
{
}

inline constexpr QskArcBorderMetrics::QskArcBorderMetrics( qreal outer,
    qreal inner, qreal start, qreal end, Qt::SizeMode sizeMode ) noexcept
    : m_widths{ outer, inner, start, end }
    , m_sizeMode( sizeMode )
{
}

inline constexpr bool QskArcBorderMetrics::operator==(
    const QskArcBorderMetrics& other ) const noexcept
{
    return ( m_sizeMode == other.m_sizeMode )
            && qskFuzzyCompare( m_widths[0], other.m_widths[0] )
            && qskFuzzyCompare( m_widths[1], other.m_widths[1] )
            && qskFuzzyCompare( m_widths[2], other.m_widths[2] )
            && qskFuzzyCompare( m_widths[3], other.m_widths[3] );
}

inline constexpr bool QskArcBorderMetrics::operator!=(
    const QskArcBorderMetrics& other ) const noexcept
{
    return !( *this == other );
}

inline constexpr bool QskArcBorderMetrics::isNull() const noexcept
{
    return ( qFuzzyIsNull( m_widths[0] )
            && qFuzzyIsNull( m_widths[1] )
            && qFuzzyIsNull( m_widths[2] )
            && qFuzzyIsNull( m_widths[3] )
            && m_sizeMode == Qt::AbsoluteSize );
}

inline constexpr qreal QskArcBorderMetrics::width( Qsk::ArcPosition position ) const noexcept
{
    return m_widths[ position ];
}

inline constexpr qreal QskArcBorderMetrics::outerWidth() const noexcept
{
    return m_widths[ Qsk::Outer ];
}

inline constexpr qreal QskArcBorderMetrics::innerWidth() const noexcept
{
    return m_widths[ Qsk::Inner ];
}

inline constexpr qreal QskArcBorderMetrics::startWidth() const noexcept
{
    return m_widths[ Qsk::Start ];
}

inline constexpr qreal QskArcBorderMetrics::endWidth() const noexcept
{
    return m_widths[ Qsk::End ];
}

inline constexpr Qt::SizeMode QskArcBorderMetrics::sizeMode() const noexcept
{
    return m_sizeMode;
}

#ifndef QT_NO_DEBUG_STREAM

class QDebug;
QSK_EXPORT QDebug operator<<( QDebug, const QskArcBorderMetrics& );

#endif

Q_DECLARE_TYPEINFO( QskArcBorderMetrics, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskArcBorderMetrics )

#endif
