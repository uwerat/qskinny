/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef _BORDER_H
#define _BORDER_H

#include "QskGlobal.h"
#include <QMetaType>
#include <QColor>

class QDebug;

class Border
{
    Q_GADGET

    Q_PROPERTY( qreal width READ width WRITE setWidth )
    Q_PROPERTY( QColor color READ color WRITE setColor )

public:
    Border( const QColor& = Qt::black );
    Border( qreal width, const QColor& = Qt::black );

    qreal width() const;
    void setWidth( qreal width );

    QColor color() const;
    void setColor( const QColor& );

    bool operator==( const Border& other ) const;
    bool operator!=( const Border& other ) const;

private:
    qreal m_width;
    QColor m_color;
};

inline bool Border::operator!=( const Border& other ) const
{
    return !( *this == other );
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<( QDebug, const Border& );
#endif

Q_DECLARE_TYPEINFO( Border, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( Border )

#endif
