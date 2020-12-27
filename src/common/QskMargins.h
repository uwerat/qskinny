/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_MARGINS_H
#define QSK_MARGINS_H

#include "QskGlobal.h"

#include <qmargins.h>
#include <qmetatype.h>
#include <qnamespace.h>

class QVariant;

/*
   Having a wrapper like this feels a bit stupid, but unfortunately QMarginsF
   is not registered as Qt metatype and doing this in an external library
   is an obvious source for conflicts with application code.
 */
class QSK_EXPORT QskMargins : public QMarginsF
{
    Q_GADGET

    Q_PROPERTY( qreal left READ left WRITE setLeft )
    Q_PROPERTY( qreal top READ top WRITE setTop )
    Q_PROPERTY( qreal right READ right WRITE setRight )
    Q_PROPERTY( qreal bottom READ bottom WRITE setBottom )

  public:
    constexpr QskMargins() noexcept = default;
    constexpr QskMargins( const QMarginsF& ) noexcept;
    constexpr QskMargins( const QMargins& ) noexcept;

    constexpr QskMargins( qreal left, qreal top, qreal right, qreal bottom ) noexcept;
    constexpr QskMargins( qreal margin ) noexcept;
    constexpr QskMargins( qreal horizontal, qreal vertical ) noexcept;

    QskMargins mirrored( Qt::Orientations ) const noexcept;
    constexpr QskMargins rotated() const noexcept;

    constexpr QskMargins translated( qreal dx, qreal dy ) const noexcept;

    constexpr QskMargins grownBy( qreal dx, qreal dy ) const noexcept;
    constexpr QskMargins shrunkBy( qreal dx, qreal dy ) const noexcept;

    constexpr QskMargins expandedTo( const QskMargins& ) const noexcept;
    constexpr QskMargins boundedTo( const QskMargins& ) const noexcept;

    void setMargins( qreal margin ) noexcept;
    void setMargins( qreal horizontal, qreal vertical ) noexcept;

    void setMarginsAt( Qt::Edges, qreal ) noexcept;
    qreal marginAt( Qt::Edge ) const noexcept;

    constexpr qreal width() const noexcept;
    constexpr qreal height() const noexcept;
    constexpr qreal extent( Qt::Orientation ) const noexcept;

    QskMargins interpolated( const QskMargins&, qreal progress ) const noexcept;

    constexpr bool isExpanding() const noexcept;

    static QVariant interpolate( const QskMargins&,
        const QskMargins&, qreal progress ) noexcept;
};

constexpr inline QskMargins::QskMargins( qreal margin ) noexcept
    : QskMargins( margin, margin, margin, margin )
{
}

constexpr inline QskMargins::QskMargins(
        qreal horizontal, qreal vertical ) noexcept
    : QskMargins( horizontal, vertical, horizontal, vertical )
{
}

constexpr QskMargins::QskMargins(
        qreal left, qreal top, qreal right, qreal bottom ) noexcept
    : QMarginsF( left, top, right, bottom )
{
}

constexpr inline QskMargins::QskMargins( const QMarginsF& margins ) noexcept
    : QMarginsF( margins )
{
}

constexpr inline QskMargins::QskMargins( const QMargins& margins ) noexcept
    : QMarginsF( margins )
{
}

inline void QskMargins::setMargins( qreal margin ) noexcept
{
    *this = QskMargins( margin );
}

inline constexpr bool QskMargins::isExpanding() const noexcept
{
    return ( left() > 0 ) || ( right() > 0 ) || ( top() > 0 ) || ( right() > 0 );
}

inline void QskMargins::setMargins( qreal horizontal, qreal vertical ) noexcept
{
    *this = QskMargins( horizontal, vertical );
}

inline QskMargins QskMargins::mirrored(
    Qt::Orientations orientations ) const noexcept
{
    switch ( int( orientations ) )
    {
        case Qt::Vertical:
            return QskMargins( left(), bottom(), right(), top() );

        case Qt::Horizontal:
            return QskMargins( right(), top(), left(), bottom() );

        case Qt::Vertical | Qt::Horizontal:
            return QskMargins( right(), bottom(), left(), top() );

        default:
            return *this;
    }
}

constexpr inline QskMargins QskMargins::rotated() const noexcept
{
    return QskMargins( top(), left(), bottom(), right() );
}

constexpr inline QskMargins QskMargins::translated( qreal dx, qreal dy ) const noexcept
{
    return QskMargins( left() + dx, top() + dy, right() - dx, bottom() - dy );
}

constexpr inline QskMargins QskMargins::grownBy( qreal dx, qreal dy ) const noexcept
{
    return QskMargins( left() + dx, top() + dy, right() + dx, bottom() + dy );
}

constexpr inline QskMargins QskMargins::shrunkBy( qreal dx, qreal dy ) const noexcept
{
    return QskMargins( left() - dx, top() - dy, right() - dx, bottom() - dy );
}

constexpr inline QskMargins QskMargins::expandedTo( const QskMargins& other ) const noexcept
{
    return QskMargins(
        qMax( left(), other.left() ),
        qMax( top(), other.top() ),
        qMax( right(), other.right() ),
        qMax( bottom(), other.bottom() )
    );
}

constexpr inline QskMargins QskMargins::boundedTo( const QskMargins& other ) const noexcept
{
    return QskMargins(
        qMin( left(), other.left() ),
        qMin( top(), other.top() ),
        qMin( right(), other.right() ),
        qMin( bottom(), other.bottom() )
    );
}
constexpr inline qreal QskMargins::extent( Qt::Orientation orientation ) const noexcept
{
    return ( orientation == Qt::Horizontal ) ? width() : height();
}

constexpr inline qreal QskMargins::width() const noexcept
{
    return left() + right();
}

constexpr inline qreal QskMargins::height() const noexcept
{
    return top() + bottom();
}

Q_DECLARE_TYPEINFO( QskMargins, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskMargins )

#endif
