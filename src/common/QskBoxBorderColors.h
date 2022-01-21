/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_BORDER_COLORS_H
#define QSK_BOX_BORDER_COLORS_H

#include "QskGradient.h"
#include "QskNamespace.h"

#include <qcolor.h>
#include <qmetatype.h>

class QDebug;

class QSK_EXPORT QskBoxBorderColors
{
  public:
    QskBoxBorderColors();

    QskBoxBorderColors( const QskGradient& left, const QskGradient& top,
        const QskGradient& right, const QskGradient& bottom );

    QskBoxBorderColors( Qt::GlobalColor );
    QskBoxBorderColors( QRgb );
    QskBoxBorderColors( const QColor& );
    QskBoxBorderColors( const QskGradient& );

    ~QskBoxBorderColors();

    bool operator==( const QskBoxBorderColors& ) const;
    bool operator!=( const QskBoxBorderColors& ) const;

    void setAlpha( int alpha );

    void setGradients( const QskGradient& );
    void setGradients( const QskGradient& left, const QskGradient& top,
        const QskGradient& right, const QskGradient& bottom );

    void setGradient( Qsk::Position, const QskGradient& );
    QskGradient gradient( Qsk::Position ) const;

    void setGradientAt( Qt::Edges, const QskGradient& );
    const QskGradient& gradientAt( Qt::Edge ) const;

    QskBoxBorderColors interpolated( const QskBoxBorderColors&, qreal value ) const;

    static QVariant interpolate( const QskBoxBorderColors&,
        const QskBoxBorderColors&, qreal ratio );

    uint hash( uint seed = 0 ) const;

    bool isMonochrome() const;
    bool isVisible() const;

  private:
    QskGradient m_gradients[ 4 ];
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

inline QskGradient QskBoxBorderColors::gradient( Qsk::Position position ) const
{
    return m_gradients[ position ];
}

#ifndef QT_NO_DEBUG_STREAM

QSK_EXPORT QDebug operator<<( QDebug, const QskBoxBorderColors& );

#endif

Q_DECLARE_METATYPE( QskBoxBorderColors )

#endif
