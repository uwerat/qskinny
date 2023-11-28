/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskPlotViewSkinlet.h"
#include "QskPlotView.h"

#include <QskGraduationRenderer.h>
#include <QskGraduationMetrics.h>
#include <QskTickmarks.h>
#include <QskTextColors.h>
#include <QskFunctions.h>
#include <QskSGNode.h>
#include <QskBoxBorderMetrics.h>
#include <QskMargins.h>
#include <QskSkin.h>

#include <qfontmetrics.h>
#include <qquickwindow.h>

static inline QskTextColors qskTextColors(
    const QskSkinnable* skinnable, QskAspect aspect )
{
    using A = QskAspect;

    QskSkinHintStatus status;

    QskTextColors c;
    c.textColor = skinnable->color( aspect | A::TextColor, &status );

    if ( status.aspect.subControl() != aspect.subControl() )
    {
        // using the same color as the one for the ticks
        c.textColor = skinnable->color( aspect );
    }

    c.styleColor = skinnable->color( aspect | A::StyleColor );
    c.linkColor = skinnable->color( aspect | A::LinkColor );

    return c;
}

static inline QskAspect qskAxisAspect( QskPlot::Axis axis )
{
    using Q = QskPlotView;
    using A = QskAspect;

    return Q::AxisScale | ( ( axis == QskPlot::XBottom ) ? A::Bottom : A::Left );
}

namespace
{
    class Renderer : public QskGraduationRenderer
    {
        using Inherited = QskGraduationRenderer;

      public:
        Renderer( const QskPlotView* view, QskPlot::Axis axis )
            : m_view( view )
            , m_axis( axis )
        {
            using Q = QskPlotView;
            using A = QskAspect;

            setEdge( axis == QskPlot::XBottom ? Qt::BottomEdge : Qt::LeftEdge );

            setBoundaries( view->boundaries( axis ) );
            setTickmarks( view->tickmarks( axis ) );

            const auto aspect = Q::AxisScale
                | ( ( axis == QskPlot::XBottom ) ? A::Bottom : A::Left );

            const auto flags = view->flagHint< QskGraduationRenderer::Flags >(
                aspect | QskAspect::Style, QskGraduationRenderer::Backbone );

            setFlags( flags );

            setTickColor( view->color( aspect ) );

            const auto graduation = view->effectiveSkinHint(
                aspect | QskAspect::Metric | QskAspect::Graduation );

            setTickMetrics( graduation.value< QskGraduationMetrics >() );

            setSpacing( view->spacingHint( aspect ) );

            const auto fontRole = view->fontRoleHint( aspect );
            setFont( view->effectiveSkin()->font( fontRole ) );

            setTextColors( qskTextColors( view, aspect ) );
        }

        qreal labelOffset() const
        {
            const auto length = tickMetrics().maxLength();

            qreal off = length + spacing();
            if ( flags() & CenteredTickmarks )
                off -= 0.5 * length;

            return off;
        }

        QVariant labelAt( qreal pos ) const override
        {
            return m_view->labelAt( m_axis, pos );
        }

      private:
        const QskPlotView* m_view;
        const QskPlot::Axis m_axis;
    };
}

QskPlotViewSkinlet::QskPlotViewSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, CanvasRole, AxisRole } );
}

QskPlotViewSkinlet::~QskPlotViewSkinlet() = default;

QRectF QskPlotViewSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    using Q = QskPlotView;

    if ( subControl == Q::Panel )
        return contentsRect;

    if ( subControl == Q::Canvas )
    {
        const auto view = static_cast< const QskPlotView* >( skinnable );

        const auto rectX = sampleRect(
            skinnable, contentsRect, Q::AxisScale, QskPlot::XBottom );

        const auto rectY = sampleRect(
            skinnable, contentsRect, Q::AxisScale, QskPlot::YLeft );

        auto rect = view->subControlContentsRect( Q::Panel );

        rect.setBottom( rectX.top() );
        rect.setLeft( rectY.right() );

        return rect;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskPlotViewSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    using Q = QskPlotView;

    switch ( nodeRole )
    {
        case PanelRole:
            return updateBoxNode( skinnable, node, Q::Panel );

        case CanvasRole:
            return updateBoxNode( skinnable, node, Q::Canvas );

        case AxisRole:
            return updateSeriesNode( skinnable, Q::AxisScale, node );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

int QskPlotViewSkinlet::sampleCount(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    using Q = QskPlotView;

    if ( subControl == Q::AxisScale )
        return 2; // for the moment only 2 axes

    return Inherited::sampleCount( skinnable, subControl );
}

QRectF QskPlotViewSkinlet::sampleRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl, int index ) const
{
    if ( subControl == QskPlotView::AxisScale )
    {
        const auto axis = static_cast< QskPlot::Axis >( index );
        return axisRect( skinnable, axis );
    }

    return Inherited::sampleRect( skinnable, contentsRect, subControl, index );
}

QSGNode* QskPlotViewSkinlet::updateSampleNode( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, int index, QSGNode* node ) const
{
    using Q = QskPlotView;

    if ( subControl == Q::AxisScale )
    {
        const auto axis = static_cast< QskPlot::Axis >( index );
        return updateAxisNode( skinnable, axis, node );
    }

    return Inherited::updateSampleNode( skinnable, subControl, index, node );
}

QSizeF QskPlotViewSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    return Inherited::sizeHint( skinnable, which, constraint );
}

QRectF QskPlotViewSkinlet::axisRect(
    const QskSkinnable* skinnable, QskPlot::Axis axis ) const
{
    using Q = QskPlotView;
    using A = QskAspect;

    const auto view = static_cast< const QskPlotView* >( skinnable );
    auto rect = view->subControlContentsRect( Q::Panel );

    const QskMargins paddingLeft = view->paddingHint( Q::AxisScale | A::Left );
    const QskMargins paddingBottom = view->paddingHint( Q::AxisScale | A::Bottom );

    qreal x0, y0;
    {
        const Renderer renderer( view, QskPlot::XBottom );
        const auto sz = renderer.boundingLabelSize();

        y0 = rect.bottom() - sz.height()
            - renderer.labelOffset() - paddingBottom.height();
    }
    {
        const Renderer renderer( view, QskPlot::YLeft );
        const auto sz = renderer.boundingLabelSize();

        x0 = rect.left() + sz.width()
            + renderer.labelOffset() + paddingLeft.width();
    }

    const auto canvasBorder = view->boxBorderMetricsHint( Q::Canvas );

    if ( axis == QskPlot::XBottom )
    {
        rect.setTop( y0 );
        rect.setLeft( x0 + canvasBorder.left() );
        rect.setRight( rect.right() - canvasBorder.right() );
    }
    else
    {
        rect.setRight( x0 );
        rect.setTop( rect.top() + canvasBorder.top() );
        rect.setBottom( y0 - canvasBorder.bottom() );
    }

    return rect;
}

QSGNode* QskPlotViewSkinlet::updateAxisNode(
    const QskSkinnable* skinnable, QskPlot::Axis axis, QSGNode* node ) const
{
    using Q = QskPlotView;

    const auto view = static_cast< const QskPlotView* >( skinnable );

    const auto axisRect = sampleRect( view, view->contentsRect(), Q::AxisScale, axis );
    const auto padding = view->paddingHint( qskAxisAspect( axis ) );

    Renderer renderer( view, axis );
    if ( axis == QskPlot::XBottom )
    {
        renderer.setPosition( axisRect.top() + padding.top() );
        renderer.setRange( axisRect.left(), axisRect.right() );
    }
    else
    {
        renderer.setPosition( axisRect.right() - padding.right() );
        renderer.setRange( axisRect.top(), axisRect.bottom() );
    }

    return renderer.updateNode( view, node );
}

#include "moc_QskPlotViewSkinlet.cpp"
