/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcRenderer.h"
#include "QskArcMetrics.h"

#include <qpainterpath.h>
#include <qrect.h>

static inline QPainterPath qskArcPath(
    const QRectF& rect, const QskArcMetrics& metrics )
{
    const auto rx = 0.5 * rect.width();
    const auto ry = 0.5 * rect.height();

    const auto m = metrics.toAbsolute( rx, ry );

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

static inline QRectF qskArcRect(
    const QRectF& rect, const QskArcMetrics& metrics )
{
    return qskArcPath( rect, metrics ).controlPointRect();
}

QPainterPath QskArcRenderer::arcPath(
    qreal radius, const QskArcMetrics& metrics )
{
    const QRectF r( 0.0, 0.0, 2 * radius, 2 * radius );
    return qskArcPath( r, metrics );
}

QPainterPath QskArcRenderer::arcPath(
    const QSizeF& diameters, const QskArcMetrics& metrics )
{
    const QRectF r( 0.0, 0.0, diameters.width(), diameters.height() );
    return qskArcPath( r, metrics );
}

QPainterPath QskArcRenderer::arcPath(
    const QRectF& rect, const QskArcMetrics& metrics )
{
    return qskArcPath( rect, metrics );
}

QRectF QskArcRenderer::arcRect( qreal radius, const QskArcMetrics& metrics )
{
    const qreal d = 2.0 * radius;
    return qskArcRect( QRectF( 0.0, 0.0, d, d ), metrics );
}

QRectF QskArcRenderer::arcRect( const QSizeF& diameters, const QskArcMetrics& metrics )
{
    const QRectF r( 0.0, 0.0, diameters.width(), diameters.height() );
    return qskArcRect( r, metrics );
}

QRectF QskArcRenderer::arcRect( const QRectF& rect, const QskArcMetrics& metrics )
{
    return qskArcRect( rect, metrics );
}

QSizeF QskArcRenderer::arcSize(
    const QSizeF& diameters, const QskArcMetrics& metrics )
{
    if ( qFuzzyIsNull( metrics.spanAngle() ) )
        return QSizeF();

    if ( qAbs( metrics.spanAngle() ) >= 360.0 )
        return diameters;

    const QRectF r( 0.0, 0.0, diameters.width(), diameters.height() );
    return qskArcRect( r, metrics ).size();
}
