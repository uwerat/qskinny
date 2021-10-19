/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_ARC_BORDER_COLORS_H
#define QSK_ARC_BORDER_COLORS_H

#include "QskNamespace.h"

#include <qcolor.h>
#include <qmetatype.h>

class QDebug;

class QSK_EXPORT QskArcBorderColors
{
  public:
    QskArcBorderColors();

    QskArcBorderColors( const QColor& outer, const QColor& inner );

    QskArcBorderColors( Qt::GlobalColor );
    QskArcBorderColors( QRgb );
    QskArcBorderColors( const QColor& );

    ~QskArcBorderColors();

    bool operator==( const QskArcBorderColors& ) const;
    bool operator!=( const QskArcBorderColors& ) const;

    void setAlpha( int alpha );

    void setColors( const QColor& );
    void setColors( const QColor& outer, const QColor& inner );

    void setColor( Qsk::ArcPosition, const QColor& );
    QColor color( Qsk::ArcPosition ) const;

    QRgb rgb( Qsk::ArcPosition ) const;

    QskArcBorderColors interpolated( const QskArcBorderColors&, qreal value ) const;

    static QVariant interpolate( const QskArcBorderColors&,
        const QskArcBorderColors&, qreal ratio );

    uint hash( uint seed = 0 ) const;

    bool isMonochrome() const;
    bool isVisible() const;

  private:
    // should be stored as QRgb
    QColor m_colors[ 2 ];
};

inline QskArcBorderColors::QskArcBorderColors( Qt::GlobalColor color )
    : QskArcBorderColors( QColor( color ) )
{
}

inline QskArcBorderColors::QskArcBorderColors( QRgb rgb )
    : QskArcBorderColors( QColor::fromRgba( rgb ) )
{
}

inline bool QskArcBorderColors::operator!=( const QskArcBorderColors& other ) const
{
    return !( *this == other );
}

inline QColor QskArcBorderColors::color( Qsk::ArcPosition position ) const
{
    return m_colors[ position ];
}

inline QRgb QskArcBorderColors::rgb( Qsk::ArcPosition position ) const
{
    return m_colors[ position ].rgba();
}

#ifndef QT_NO_DEBUG_STREAM

QSK_EXPORT QDebug operator<<( QDebug, const QskArcBorderColors& );

#endif

Q_DECLARE_METATYPE( QskArcBorderColors )

#endif
