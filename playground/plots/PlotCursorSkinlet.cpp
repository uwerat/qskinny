/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "PlotCursorSkinlet.h"
#include "PlotCursor.h"

#include <QskSGNode.h>
#include <QskTextOptions.h>
#include <QskTextColors.h>

#include <QskPlotCurve.h>
#include <QskPlotCorridor.h>
#include <QskPlotCorridorData.h>
#include <QskPlotView.h>
#include <QskFunctions.h>

#include <qtransform.h>
#include <qfontmetrics.h>
#include <qcolor.h>

enum { Lower, Upper, Value };

static inline QskAspect::Variation variation( int index )
{
    if ( index == Lower )
        return QskAspect::Lower;

    if ( index == Upper )
        return QskAspect::Upper;

    return QskAspect::NoVariation;
}

class PlotCursorSkinlet::PrivateData
{
  public:

    struct
    {
        qreal value;
    } labelInfo[ 3 ];
};

PlotCursorSkinlet::PlotCursorSkinlet( QskSkin* skin )
    : Inherited( skin )
    , m_data( new PrivateData )
{
    setNodeRoles( { CursorLine, TextBox, Text } );
}

PlotCursorSkinlet::~PlotCursorSkinlet()
{
}

void PlotCursorSkinlet::updateNode(
    QskSkinnable* skinnable, QSGNode* parent ) const
{
    const auto cursor = static_cast< const PlotCursor* >( skinnable );
    const auto x = cursor->position();

    auto info = m_data->labelInfo;

    for ( auto child : cursor->view()->children() )
    {
        if ( auto curve = qobject_cast< const QskPlotCurve* >( child ) )
        {
            const auto pos = curve->interpolatedPoint( Qt::Horizontal, x );
            info[Value].value = pos.y();
        }
        else if ( auto corridor = qobject_cast< const QskPlotCorridor* >( child ) )
        {
            const auto boundary = corridor->interpolatedSample( x ).boundary;

            info[ Lower ].value = boundary.lowerBound();
            info[ Upper ].value = boundary.upperBound();
        }
    }

    Inherited::updateNode( skinnable, parent );
}

QSGNode* PlotCursorSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    using Q = PlotCursor;

    switch( nodeRole )
    {
        case CursorLine:
            return updateCursorLineNode( skinnable, node );

        case TextBox:
            return updateSeriesNode( skinnable, Q::LabelPanel, node );

        case Text:
            return updateSeriesNode( skinnable, Q::LabelText, node );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

int PlotCursorSkinlet::sampleCount(
    const QskSkinnable*, QskAspect::Subcontrol ) const
{
    return 3;
}

QRectF PlotCursorSkinlet::sampleRect( const QskSkinnable* skinnable,
    const QRectF&, QskAspect::Subcontrol, int index ) const
{
    using Q = PlotCursor;

    const auto cursor = static_cast< const PlotCursor* >( skinnable );

    auto pos = QPointF( cursor->position(),
        m_data->labelInfo[ index ].value );

    pos = cursor->transformation().map( pos );

    const QFontMetricsF fm( cursor->effectiveFont( Q::LabelText ) );

    const qreal w = qskHorizontalAdvance( fm, "100.0" );
    const qreal h = fm.height();

    QRectF r( 0, 0, w, h );
    r = r.marginsAdded( cursor->paddingHint( Q::LabelPanel ) );

    r.moveRight( pos.x() - 5 );
    r.moveBottom( pos.y() );

    return r;
}

QSGNode* PlotCursorSkinlet::updateSampleNode( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, int index, QSGNode* node ) const
{
    using Q = PlotCursor;

    const auto rect = sampleRect( skinnable, QRectF(), subControl, index );
    const auto aspect = subControl | variation( index );

    if ( subControl == Q::LabelPanel )
    {
        const auto gradient = skinnable->gradientHint( aspect );
        return updateBoxNode( skinnable, node, rect, gradient, subControl );
    }

    if ( subControl == Q::LabelText )
    {
        const auto info = m_data->labelInfo[ index ];

        const auto text = QString::number( info.value, 'f', 1 );
        const auto color = skinnable->color( aspect );

        const auto textOptions = skinnable->textOptionsHint( aspect );
        const auto font = skinnable->effectiveFont( aspect );

        return updateTextNode( skinnable, node, rect, Qt::AlignCenter,
            text, font, textOptions, color, Qsk::Normal );
    }

    return nullptr;
}

QSGNode* PlotCursorSkinlet::updateCursorLineNode(
    const QskSkinnable* skinnable, QSGNode* node ) const
{
    auto cursor = static_cast< const PlotCursor* >( skinnable );

    const auto r = cursor->scaleRect();
    if ( r.isEmpty() )
        return nullptr;

    QPointF p1 = r.topLeft();
    QPointF p2 = r.bottomRight();

    if ( cursor->orientation() == Qt::Horizontal )
    {
        const auto x = cursor->position();
        if ( x < r.left() || x > r.right() )
            return nullptr;

        p1.rx() = p2.rx() = x;
    }
    else
    {
        const auto y = cursor->position();
        if ( y < r.top() || y > r.bottom() )
            return nullptr;

        p1.ry() = p2.ry() = y;
    }

    if ( cursor->coordinateType() == QskPlotItem::CanvasCoordinates )
    {
        /*
            When having a non solid line we might want to use CanvasCoordinates
            to avoid the length of dashes/dots being affected from the
            plot transformation.
         */
        const auto transform = cursor->transformation();
        p1 = transform.map( p1 );
        p2 = transform.map( p2 );
    }

    return updateLineNode( cursor, node, QLineF( p1, p2 ), PlotCursor::Line );
}

#include "moc_PlotCursorSkinlet.cpp"
