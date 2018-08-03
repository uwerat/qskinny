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
    constexpr QskMargins expanded( qreal dx, qreal dy ) const noexcept;

    void setMargins( qreal margin ) noexcept;
    void setMargins( qreal horizontal, qreal vertical ) noexcept;

    void setMarginsAt( Qt::Edges, qreal ) noexcept;
    qreal marginAt( Qt::Edge ) const noexcept;

    constexpr qreal length( Qt::Orientation ) const noexcept;

    QskMargins interpolated( const QskMargins&, qreal progress ) const noexcept;

    bool isExpanding() const;

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

inline bool QskMargins::isExpanding() const
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

constexpr QskMargins QskMargins::expanded( qreal dx, qreal dy ) const noexcept
{
    return QskMargins( left() + dx, top() + dy, right() + dx, bottom() + dy );
}

constexpr qreal QskMargins::length( Qt::Orientation orientation ) const noexcept
{
    return ( orientation == Qt::Horizontal )
        ? ( left() + right() ) : ( top() + bottom() );
}

Q_DECLARE_TYPEINFO( QskMargins, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskMargins )

#endif
