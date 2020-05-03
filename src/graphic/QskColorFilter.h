/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_COLOR_FILTER_H
#define QSK_COLOR_FILTER_H

#include "QskGlobal.h"

#include <qcolor.h>
#include <qmetatype.h>
#include <qpair.h>
#include <qvector.h>

class QPen;
class QBrush;
class QVariant;

class QSK_EXPORT QskColorFilter
{
  public:
    QskColorFilter() noexcept = default;

    void addColorSubstitution( QRgb from, QRgb to );
    void addColorSubstitution( Qt::GlobalColor, QRgb );
    void addColorSubstitution( QRgb, Qt::GlobalColor );
    void addColorSubstitution( Qt::GlobalColor, Qt::GlobalColor );

    void reset();

    QPen substituted( const QPen& ) const;
    QBrush substituted( const QBrush& ) const;

    QColor substituted( const QColor& ) const;
    QRgb substituted( const QRgb& ) const;

    bool isIdentity() const noexcept;

    bool operator==( const QskColorFilter& other ) const noexcept;
    bool operator!=( const QskColorFilter& other ) const noexcept;

    const QVector< QPair< QRgb, QRgb > >& substitutions() const noexcept;

    QskColorFilter interpolated(
        const QskColorFilter&, qreal value ) const;

    // can be registered by qRegisterAnimationInterpolator
    static QVariant interpolate(
        const QskColorFilter&, const QskColorFilter&, qreal progress );

  private:
    QVector< QPair< QRgb, QRgb > > m_substitutions;
};

inline bool QskColorFilter::isIdentity() const noexcept
{
    return m_substitutions.isEmpty();
}

inline bool QskColorFilter::operator==(
    const QskColorFilter& other ) const noexcept
{
    // what about having the same substitutions, but in different order ???
    return ( m_substitutions == other.m_substitutions );
}

inline bool QskColorFilter::operator!=(
    const QskColorFilter& other ) const noexcept
{
    return ( !( *this == other ) );
}

inline const QVector< QPair< QRgb, QRgb > >&
QskColorFilter::substitutions() const noexcept
{
    return m_substitutions;
}

inline void QskColorFilter::addColorSubstitution( Qt::GlobalColor from, QRgb to )
{
    addColorSubstitution( QColor( from ).rgb(), to );
}

inline void QskColorFilter::addColorSubstitution( QRgb from, Qt::GlobalColor to )
{
    addColorSubstitution( from, QColor( to ).rgb() );
}

inline void QskColorFilter::addColorSubstitution(
    Qt::GlobalColor from, Qt::GlobalColor to )
{
    addColorSubstitution( QColor( from ).rgb(), QColor( to ).rgb() );
}

Q_DECLARE_METATYPE( QskColorFilter )

#ifndef QT_NO_DEBUG_STREAM
class QDebug;
QSK_EXPORT QDebug operator<<( QDebug, const QskColorFilter& );
#endif

#endif
