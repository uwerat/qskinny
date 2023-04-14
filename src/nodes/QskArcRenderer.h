/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_ARC_RENDERER_H
#define QSK_ARC_RENDERER_H

#include "QskGlobal.h"

class QskArcMetrics;

class QPainterPath;
class QRectF;
class QSizeF;

namespace QskArcRenderer
{
    // radius
    QSK_EXPORT QPainterPath arcPath( qreal radius, const QskArcMetrics& );
    QSK_EXPORT QRectF arcRect( qreal radius, const QskArcMetrics& );

    // diameter
    QSK_EXPORT QPainterPath arcPath( const QSizeF&, const QskArcMetrics& );
    QSK_EXPORT QSizeF arcSize( const QSizeF&, const QskArcMetrics& );
    QSK_EXPORT QRectF arcRect( const QSizeF&, const QskArcMetrics& );

    // bounding rectangle
    QSK_EXPORT QPainterPath arcPath( const QRectF&, const QskArcMetrics& );
    QSK_EXPORT QRectF arcRect( const QRectF&, const QskArcMetrics& );
};

#endif
