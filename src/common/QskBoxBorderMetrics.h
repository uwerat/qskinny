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
    Q_GADGET

    Q_PROPERTY( QskMargins widths READ widths WRITE setWidths )
    Q_PROPERTY( Qt::SizeMode sizeMode READ sizeMode WRITE setSizeMode )

    Q_PROPERTY( qreal left READ left WRITE setLeft )
    Q_PROPERTY( qreal top READ top WRITE setTop )
    Q_PROPERTY( qreal right READ right WRITE setRight )
    Q_PROPERTY( qreal bottom READ bottom WRITE setBottom )

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

    constexpr qreal left() const noexcept;
    constexpr qreal top() const noexcept;
    constexpr qreal right() const noexcept;
    constexpr qreal bottom() const noexcept;

    void setLeft( qreal ) noexcept;
    void setTop( qreal ) noexcept;
    void setRight( qreal ) noexcept;
    void setBottom( qreal ) noexcept;

    void setSizeMode( Qt::SizeMode ) noexcept;
    constexpr Qt::SizeMode sizeMode() const noexcept;

    QskBoxBorderMetrics interpolated(
        const QskBoxBorderMetrics&, qreal value ) const noexcept;

    QskBoxBorderMetrics toAbsolute( const QSizeF& ) const noexcept;

    QskHashValue hash( QskHashValue seed = 0 ) const noexcept;

    static QVariant interpolate( const QskBoxBorderMetrics&,
        const QskBoxBorderMetrics&, qreal progress );

    constexpr bool isEquidistant() const noexcept;

    QRectF adjustedRect( const QRectF& )const ;

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

inline constexpr bool QskBoxBorderMetrics::isEquidistant() const noexcept
{
    return m_widths.isEquidistant();
}

inline constexpr const QskMargins& QskBoxBorderMetrics::widths() const noexcept
{
    return m_widths;
}

inline qreal QskBoxBorderMetrics::widthAt( Qt::Edge edge ) const noexcept
{
    return m_widths.marginAt( edge );
}

inline constexpr qreal QskBoxBorderMetrics::left() const noexcept
{
    return m_widths.left();
}

inline constexpr qreal QskBoxBorderMetrics::top() const noexcept
{
    return m_widths.top();
}

inline constexpr qreal QskBoxBorderMetrics::right() const noexcept
{
    return m_widths.right();
}

inline constexpr qreal QskBoxBorderMetrics::bottom() const noexcept
{
    return m_widths.bottom();
}

inline void QskBoxBorderMetrics::setLeft( qreal left ) noexcept
{
    return m_widths.setLeft( left );
}

inline void QskBoxBorderMetrics::setTop( qreal top ) noexcept
{
    return m_widths.setTop( top );
}

inline void QskBoxBorderMetrics::setRight( qreal right ) noexcept
{
    return m_widths.setRight( right );
}

inline void QskBoxBorderMetrics::setBottom( qreal bottom ) noexcept
{
    return m_widths.setBottom( bottom );
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
