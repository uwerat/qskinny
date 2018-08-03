/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TEXT_COLORS_H
#define QSK_TEXT_COLORS_H

#include "QskGlobal.h"

#include <qcolor.h>
#include <qmetatype.h>

class QDebug;
class QVariant;

class QSK_EXPORT QskTextColors
{
  public:
    QskTextColors( const QColor& text = QColor(),
        const QColor& style = QColor(), const QColor& link = QColor() );

    QskTextColors interpolated( const QskTextColors&, qreal value ) const;

    static QVariant interpolate( const QskTextColors&,
        const QskTextColors&, qreal ratio );

    uint hash( uint seed = 0 ) const;

    QColor textColor;
    QColor styleColor;
    QColor linkColor;
};

inline QskTextColors::QskTextColors(
        const QColor& text, const QColor& style, const QColor& link )
    : textColor( text )
    , styleColor( style )
    , linkColor( link )
{
}

#ifndef QT_NO_DEBUG_STREAM

QSK_EXPORT QDebug operator<<( QDebug, const QskTextColors& );

#endif

Q_DECLARE_METATYPE( QskTextColors )

#endif
