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
    const auto sz = qMin( rect.width(), rect.height() );
    if ( sz <= 0.0 )
        return QPainterPath();

    const auto m = metrics.toAbsolute( 0.5 * sz );

    const auto tx = m.thickness() * rect.width() / sz;
    const auto ty = m.thickness() * rect.height() / sz;

    const auto innerRect = rect.adjusted( tx, ty, -tx, -ty );

    const auto angle = m.startAngle();
    const auto span = m.spanAngle();

    QPainterPath path;

    /*
        We need the end point of the inner arc to add the line that connects
        the inner/outer arcs. As QPainterPath does not offer such a method
        we insert a dummy arcMoveTo and grab the calculated position.
     */
    path.arcMoveTo( innerRect, angle + span );
    const auto pos = path.currentPosition();

    path.arcMoveTo( rect, angle ); // replaces the dummy arcMoveTo above
    path.arcTo( rect, angle, span );

    path.lineTo( pos );
    path.arcTo( innerRect, angle + span, -span );

    path.closeSubpath();

    return path;
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
