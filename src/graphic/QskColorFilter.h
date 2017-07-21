/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_COLOR_FILTER_H
#define QSK_COLOR_FILTER_H

#include "QskGlobal.h"

#include <QColor>
#include <QVector>
#include <QPair>
#include <QMetaType>

class QPen;
class QBrush;
class QVariant;
class QDebug;

class QSK_EXPORT QskColorFilter
{
public:
    QskColorFilter();
    ~QskColorFilter();

    void addColorSubstitution( QRgb from, QRgb to );
    void reset();

    QPen substituted( const QPen& ) const;
    QBrush substituted( const QBrush& ) const;

    QColor substituted( const QColor& ) const;
    QRgb substituted( const QRgb& ) const;

    bool isIdentity() const;

    bool operator==( const QskColorFilter& other ) const;
    bool operator!=( const QskColorFilter& other ) const;

    const QVector< QPair< QRgb, QRgb > >& substitutions() const;

    QskColorFilter interpolated(
        const QskColorFilter&, qreal value ) const;

    // can be registered by qRegisterAnimationInterpolator
    static QVariant interpolate( const QskColorFilter &,
        const QskColorFilter &, qreal progress );

private:
    QVector< QPair< QRgb, QRgb > > m_substitutions;
};

inline bool QskColorFilter::isIdentity() const
{
    return m_substitutions.isEmpty();
}

inline bool QskColorFilter::operator!=(
    const QskColorFilter& other ) const
{
    return ( !( *this == other ) );
}

inline const QVector< QPair< QRgb, QRgb > >&
QskColorFilter::substitutions() const
{
    return m_substitutions;
}

Q_DECLARE_METATYPE( QskColorFilter )

#ifndef QT_NO_DEBUG_STREAM
QSK_EXPORT QDebug operator<<( QDebug, const QskColorFilter& );
#endif

#endif
