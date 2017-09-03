/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_COLORS_H
#define QSK_BOX_COLORS_H

#include "QskGlobal.h"

#include <QMarginsF>
#include <QColor>
#include <Qt>
#include <QMetaType>

class QDebug;

class QSK_EXPORT QskBoxColors
{
public:
    QskBoxColors();
    QskBoxColors( const QColor& border, const QColor& fill );

    ~QskBoxColors();

    bool operator==( const QskBoxColors& ) const;
    bool operator!=( const QskBoxColors& ) const;

    void setBorderColor( const QColor& );
    void setBorderColor( Qt::Edges, const QColor& );

    void setBorderColor( const QColor& left, const QColor& top,
        const QColor& right, const QColor& bottom );

    QColor borderColor( Qt::Edge ) const;

    // will be a QskGradient later
    void setFillColor( const QColor& );
    void setFillColor( Qt::Corner, const QColor& );
    void setFillColor( Qt::Edge, const QColor& );

    void setFillColor( const QColor& topLeft, const QColor& topRight,
        const QColor& bottomLeft, const QColor& bottomRight );

    QColor fillColor( Qt::Corner ) const;

    QskBoxColors interpolated( const QskBoxColors&, qreal value ) const;
    QskBoxColors shaded( uint alpha ) const;

    QskBoxColors mirrored( Qt::Orientations ) const;
    QskBoxColors rotated() const;

    static QVariant interpolate( const QskBoxColors&,
        const QskBoxColors&, qreal ratio );

private:
    QColor m_border[ 4 ];
    QColor m_fill[ 4 ]; // should be QskGradient later
};

inline bool QskBoxColors::operator!=( const QskBoxColors& other ) const
{
    return !( *this == other );
}

#ifndef QT_NO_DEBUG_STREAM

QSK_EXPORT QDebug operator<<( QDebug, const QskBoxColors& );

#endif

Q_DECLARE_METATYPE( QskBoxColors )

#endif
