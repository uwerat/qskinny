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

namespace QskArcRenderer
{
    QSK_EXPORT QPainterPath arcPath( const QRectF&, const QskArcMetrics& );
};

#endif
