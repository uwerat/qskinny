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

    // size/rect bounded by the layout hints
    QSK_EXPORT QSizeF boundedSize( const QQuickItem*, const QSizeF& );
    QSK_EXPORT QRectF boundedRect(
        const QQuickItem*, const QRectF&, Qt::Alignment );

    QSK_EXPORT QSizeF sizeHint( const QQuickItem*,
        Qt::SizeHint, const QSizeF& constraint = QSizeF() );

    QSK_EXPORT qreal sizeHint(
        const QQuickItem*, Qt::SizeHint, Qt::Orientation, qreal constraint );

    QSK_EXPORT QskLayoutHint layoutHint(
        const QQuickItem*, Qt::Orientation, qreal constraint );

    QSK_EXPORT Qt::Alignment layoutAlignmentHint( const QQuickItem* );
    QSK_EXPORT void setLayoutAlignmentHint( QQuickItem*, Qt::Alignment );

    QSK_EXPORT bool retainSizeWhenHidden( const QQuickItem* );
    QSK_EXPORT void setRetainSizeWhenHidden( QQuickItem*, bool );

    QSK_EXPORT bool isVisibleToLayout( const QQuickItem* );

    const qreal unlimited = std::numeric_limits< float >::max();
    const QSizeF defaultSizeHints[] = { { 0, 0 }, { -1, -1 }, { unlimited, unlimited } };
}

#endif
