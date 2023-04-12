/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcRenderer.h"
#include "QskArcMetrics.h"

#include <qpainterpath.h>
#include <qrect.h>

QPainterPath QskArcRenderer::arcPath(
    const QRectF& rect, const QskArcMetrics& metrics )
{
    const auto m = metrics.toAbsolute( rect.size() );

    const qreal t2 = 0.5 * m.thickness();
    const auto r = rect.adjusted( t2, t2, -t2, -t2 );

    QPainterPath path;
    path.arcMoveTo( r, m.startAngle() );
    path.arcTo( r, m.startAngle(), m.spanAngle() );

    QPainterPathStroker stroker;
    stroker.setWidth( m.thickness() );
    stroker.setCapStyle( Qt::FlatCap );

    return stroker.createStroke( path );
}
