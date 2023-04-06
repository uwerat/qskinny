/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_ARC_RENDERER_H
#define QSK_ARC_RENDERER_H

#include "QskGlobal.h"

class QskArcMetrics;
class QskGradient;

class QPainter;
class QRectF;

class QSK_EXPORT QskArcRenderer
{
  public:
    void renderArc( const QRectF&, const QskArcMetrics&,
        const QskGradient&, QPainter* );
};

#endif
