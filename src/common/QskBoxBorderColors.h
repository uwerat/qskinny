/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_BORDER_COLORS_H
#define QSK_BOX_BORDER_COLORS_H

#include "QskNamespace.h"

#include <qcolor.h>
#include <qmetatype.h>

class QDebug;

class QSK_EXPORT QskBoxBorderColors
{
  public:
    QskBoxBorderColors();

    QskBoxBorderColors( const QColor& left, const QColor& top,
        const QColor& right, const QColor& bottom );

    QskBoxBorderColors( Qt::GlobalColor );
    QskBoxBorderColors( QRgb );
    QskBoxBorderColors( const QColor& );

    ~QskBoxBorderColors();

    bool operator==( const QskBoxBorderColors& ) const;
    bool operator!=( const QskBoxBorderColors& ) const;

    void setAlpha( int alpha );

    void setColors( const QColor& );
    void setColors( const QColor& left, const QColor& top,
        const QColor& right, const QColor& bottom );

    void setColor( Qsk::Position, const QColor& );
    QColor color( Qsk::Position ) const;

    void setColorsAt( Qt::Edges, const QColor& );
    QColor colorAt( Qt::Edge ) const;

    QRgb rgb( Qsk::Position ) const;

    QskBoxBorderColors interpolated( const QskBoxBorderColors&, qreal value ) const;

    static QVariant interpolate( const QskBoxBorderColors&,
        const QskBoxBorderColors&, qreal ratio );

    uint hash( uint seed = 0 ) const;

    bool isMonochrome() const;
    bool isVisible() const;

  private:
    // should be stored as QRgb
    QColor m_colors[ 4 ];
};

inline QskBoxBorderColors::QskBoxBorderColors( Qt::GlobalColor color )
    : QskBoxBorderColors( QColor( color ) )
{
}

inline QskBoxBorderColors::QskBoxBorderColors( QRgb rgb )
    : QskBoxBorderColors( QColor::fromRgba( rgb ) )
{
}

inline bool QskBoxBorderColors::operator!=( const QskBoxBorderColors& other ) const
{
    return !( *this == other );
}

inline QColor QskBoxBorderColors::color( Qsk::Position position ) const
{
    return m_colors[ position ];
}

inline QRgb QskBoxBorderColors::rgb( Qsk::Position position ) const
{
    return m_colors[ position ].rgba();
}

#ifndef QT_NO_DEBUG_STREAM

QSK_EXPORT QDebug operator<<( QDebug, const QskBoxBorderColors& );

#endif

Q_DECLARE_METATYPE( QskBoxBorderColors )

#endif
