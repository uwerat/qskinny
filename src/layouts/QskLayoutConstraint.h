/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LAYOUT_CONSTRAINT_H
#define QSK_LAYOUT_CONSTRAINT_H

#include "QskGlobal.h"

#include <qnamespace.h>
#include <qsize.h>
#include <limits>

class QskSizePolicy;
class QskControl;
class QskLayoutHint;
class QQuickItem;
class QSizeF;
class QRectF;

namespace std { template< typename T > class function; }

namespace QskLayoutConstraint
{
    enum Type
    {
        Unconstrained  = 0,

        WidthForHeight = 1 << 0,
        HeightForWidth = 1 << 1
    };

    QSK_EXPORT qreal heightForWidth( const QQuickItem*, qreal width );
    QSK_EXPORT qreal widthForHeight( const QQuickItem*, qreal height );

    QSK_EXPORT Type constraintType( const QQuickItem* );
    QSK_EXPORT bool isConstrained( const QQuickItem*, Qt::Orientation );

    QSK_EXPORT qreal constrainedMetric(
        Type, const QskControl*, qreal value,
        std::function< qreal( Type, const QskControl*, qreal ) > );

    QSK_EXPORT qreal constrainedChildrenMetric(
        Type, const QskControl*, qreal constraint );

    QSK_EXPORT QskSizePolicy sizePolicy( const QQuickItem* );

    QSK_EXPORT QSizeF boundedSize( const QQuickItem*, const QSizeF& );
    QSK_EXPORT QSizeF adjustedSize( const QQuickItem*, const QSizeF& );

    QSK_EXPORT QSizeF sizeHint( const QQuickItem*,
        Qt::SizeHint, const QSizeF& constraint = QSizeF() );

    QSK_EXPORT qreal sizeHint(
        const QQuickItem*, Qt::SizeHint, Qt::Orientation, qreal constraint );

    QSK_EXPORT QRectF itemRect(
        const QQuickItem*, const QRectF&, Qt::Alignment );

    QSK_EXPORT QskLayoutHint layoutHint(
        const QQuickItem*, Qt::Orientation, qreal constraint );

    const qreal unlimited = std::numeric_limits< float >::max();
}

#endif
