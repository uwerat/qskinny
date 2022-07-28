/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_FUNCTIONS_H
#define QSK_FUNCTIONS_H

#include "QskGlobal.h"

#include <qmargins.h>
#include <qrect.h>

class QFont;
class QFontMetricsF;
class QString;

QSK_EXPORT QRect qskAlignedRect( const QRect& outerRect,
    int width, int height, Qt::Alignment alignment );

QSK_EXPORT QRectF qskAlignedRectF( const QRectF& outerRect,
    qreal width, qreal height, Qt::Alignment alignment );

QSK_EXPORT QRect qskInnerRect( const QRectF& rect );
QSK_EXPORT QRectF qskInnerRectF( const QRectF& rect );

QSK_EXPORT QRectF qskValidOrEmptyInnerRect(
    const QRectF& rect, const QMarginsF& margins );

QSK_EXPORT qreal qskHorizontalAdvance( const QFont&, const QString& );
QSK_EXPORT qreal qskHorizontalAdvance( const QFontMetricsF&, const QString& );

QSK_EXPORT QRectF qskInterpolatedRect(
    const QRectF& from, const QRectF& to, qreal progress );

QSK_EXPORT QSizeF qskInterpolatedSize(
    const QSizeF& from, const QSizeF& to, qreal progress );

inline QRect qskAlignedRect( const QRect& outerRect,
    const QSize& size, Qt::Alignment alignment )
{
    return qskAlignedRect( outerRect, size.width(), size.height(), alignment );
}

inline QRectF qskAlignedRectF( const QRectF& outerRect,
    const QSizeF& size, Qt::Alignment alignment )
{
    return qskAlignedRectF( outerRect, size.width(), size.height(), alignment );
}

inline QMarginsF qskMargins( const QRectF& rect, const QRectF& innerRect )
{
    return QMarginsF(
        innerRect.left() - rect.left(),
        innerRect.top() - rect.top(),
        rect.right() - innerRect.right(),
        rect.bottom() - innerRect.bottom() );
}

inline bool qskFuzzyCompare( qreal value1, qreal value2 )
{
    if ( qFuzzyIsNull( value1 ) )
        return qFuzzyIsNull( value2 );

    if ( qFuzzyIsNull( value2 ) )
        return false;

    return qFuzzyCompare( value1, value2 );
}

QSK_EXPORT qreal qskFuzzyFloor( qreal value, qreal stepSize );
QSK_EXPORT qreal qskFuzzyCeil( qreal value, qreal stepSize );

#endif
