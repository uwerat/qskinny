/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_CORNER_H
#define QSK_CORNER_H

#include "QskGlobal.h"

#include <QMetaType>
#include <Qt>

class QDebug;
class QSizeF;

class QSK_EXPORT QskCorner
{
    Q_GADGET

    Q_PROPERTY( qreal radius READ radius WRITE setRadius )
    Q_PROPERTY( Qt::SizeMode mode READ mode WRITE setMode )

public:
    // currently only radial, ellipsoid will follow ...

    QskCorner();
    QskCorner( Qt::SizeMode, qreal radius );

    qreal radius() const;
    void setRadius( qreal width );

    Qt::SizeMode mode() const;
    void setMode( Qt::SizeMode );

    qreal effectiveRadius( qreal width, qreal height ) const;
    qreal effectiveRadius( const QSizeF& ) const;

    bool operator==( const QskCorner& ) const;
    bool operator!=( const QskCorner& ) const;

private:
    qreal m_radius;
    Qt::SizeMode m_mode;
};

inline bool QskCorner::operator!=( const QskCorner& other ) const
{
    return ( !( *this == other ) );
}

#ifndef QT_NO_DEBUG_STREAM
QSK_EXPORT QDebug operator<<( QDebug, const QskCorner& );
#endif

Q_DECLARE_TYPEINFO( QskCorner, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskCorner )

#endif
