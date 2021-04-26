/****************************************************************************
**
** Copyright 2021 Edelhirsch Software GmbH. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of the copyright holder nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/

#include "PieChartSkinlet.h"
#include "PieChart.h"

#include <QskBoxNode.h>
#include <QskBoxShapeMetrics.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxBorderColors.h>

PieChartSkinlet::PieChartSkinlet( QskSkin* skin ) : QskSkinlet( skin )
{
    setNodeRoles( { PanelRole, LabelsRole } );
}

QRectF PieChartSkinlet::subControlRect( const QskSkinnable*, const QRectF& contentsRect, QskAspect::Subcontrol ) const
{
    return contentsRect;
}

QSGNode* PieChartSkinlet::updateSubNode( const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto pieChart = static_cast< const PieChart* >( skinnable );

    switch( nodeRole )
    {
        case PanelRole:
            return updatePanelNode( pieChart, node );

        case LabelsRole:
            return updateLabelsNode( pieChart, node );

        default:
            return nullptr;
    }
}

QSGNode* PieChartSkinlet::updatePanelNode( const PieChart* pieChart, QSGNode* node ) const
{
    auto boxNode = static_cast< QskBoxNode* >( node );

    if( boxNode == nullptr )
    {
        boxNode = new QskBoxNode;
    }

    auto panelRect = subControlRect( pieChart, pieChart->contentsRect(), PieChart::Panel );

    // ### when displaying a legend we might want to revise this
    if( panelRect.width() > panelRect.height() )
    {
        panelRect.setWidth( panelRect.height() );
    }
    else if( panelRect.width() < panelRect.height() )
    {
        panelRect.setHeight( panelRect.width() );
    }

    const qreal radius = panelRect.width() / 2;

    QskBoxShapeMetrics shapeMetrics( radius, radius, radius, radius );
    QskBoxBorderMetrics borderMetrics = pieChart->boxBorderMetricsHint( PieChart::Panel );
    QskBoxBorderColors borderColors = pieChart->boxBorderColorsHint( PieChart::Panel );
    QskGradient gradient = pieChart->gradientHint( PieChart::Panel );
    boxNode->setBoxData( panelRect, shapeMetrics, borderMetrics, borderColors, gradient );

    return boxNode;
}

QSGNode* PieChartSkinlet::updateLabelsNode( const PieChart* pieChart, QSGNode* /*node*/ ) const
{
    const int labelsCount = pieChart->labels().count();

    if( labelsCount < 1 )
    {
        return nullptr;
    }

    return nullptr;
}
