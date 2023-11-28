/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskPlotGridSkinlet.h"
#include "QskPlotGrid.h"
#include "QskStippleMetrics.h"

#include <QskSGNode.h>
#include <QskIntervalF.h>
#include <QskLinesNode.h>

#include <qpen.h>

QskPlotGridSkinlet::QskPlotGridSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { MinorGrid, MajorGrid } );
}

QskPlotGridSkinlet::~QskPlotGridSkinlet()
{
}

QSGNode* QskPlotGridSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    using Q = QskPlotGrid;

    if ( nodeRole == MinorGrid )
        return updateGridNode( skinnable, Q::MinorLine, node );

    if ( nodeRole == MajorGrid )
        return updateGridNode( skinnable, Q::MajorLine, node );

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskPlotGridSkinlet::updateGridNode( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, QSGNode* node ) const
{
    using Q = QskPlotGrid;

    const auto gridType =
        ( subControl == Q::MinorLine ) ? Q::MinorGrid : Q::MajorGrid;

    auto grid = static_cast< const QskPlotGrid* >( skinnable );

    const auto r = grid->scaleRect();
    if ( r.isEmpty() )
        return nullptr;

    const auto stipple = grid->stippleMetricsHint( subControl );
    if ( !stipple.isValid() )
        return nullptr;

    const auto lineColor = grid->color( subControl );
    if ( !lineColor.isValid() || lineColor.alpha() == 0 )
        return nullptr;

    const auto lineWidth = grid->metric( subControl | QskAspect::Size );
    if ( lineWidth <= 0 )
        return nullptr;

    const auto xValues = grid->lines( gridType, Qt::Vertical );
    const auto yValues = grid->lines( gridType, Qt::Horizontal );

    if ( xValues.isEmpty() && yValues.isEmpty() )
        return nullptr;

    auto gridNode = QskSGNode::ensureNode< QskLinesNode >( node );
    gridNode->setPixelAlignment( Qt::Horizontal | Qt::Vertical );

    gridNode->updateGrid( lineColor, lineWidth,
        stipple, grid->transformation(), r, xValues, yValues );

    return gridNode;
}

#include "moc_QskPlotGridSkinlet.cpp"
