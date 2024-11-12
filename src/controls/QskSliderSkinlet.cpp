/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSliderSkinlet.h"
#include "QskSlider.h"

#include "QskAspect.h"
#include "QskBoxBorderMetrics.h"
#include "QskFunctions.h"

#include <QFontMetricsF>
#include <QtMath>

using Q = QskSlider;

namespace
{
    inline QRectF qskInnerPanelRect(
        const QskSlider* slider, const QRectF& contentsRect )
    {
    #if 1
        auto padding = slider->paddingHint( Q::Panel );
        padding += slider->boxBorderMetricsHint( Q::Panel ).widths();

        auto r = slider->subControlRect( contentsRect, Q::Panel );
        r = r.marginsRemoved( padding );
    #else
        r = slider->subControlContentsRect( contentsRect, Q::Panel );
    #endif

        return r;
    }

    QRectF qskInnerValueRect( const QskSlider* slider, const QRectF& contentsRect )
    {
        // For M3 the stop indicators have some padding related to the groove (and fill),
        // so we use the rect between first and last stop indicator as authoritative for
        // indicators, handle etc.
        const auto grooveIndicatorMargins = slider->paddingHint( Q::GrooveStopIndicators );
        const auto r = qskInnerPanelRect( slider, contentsRect ).marginsRemoved( grooveIndicatorMargins );
        return r;
    }
}

QskSliderSkinlet::QskSliderSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( {
        PanelRole,
        GrooveRole,
        FillRole,
        FillStopIndicatorsRole,
        GrooveStopIndicatorsRole,
        HandleRole
    } );
}

QskSliderSkinlet::~QskSliderSkinlet()
{
}

QRectF QskSliderSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto slider = static_cast< const QskSlider* >( skinnable );

    if ( subControl == Q::Panel )
    {
        return panelRect( slider, contentsRect );
    }

    if ( subControl == Q::Groove )
    {
        return grooveRect( slider, contentsRect );
    }

    if ( subControl == Q::Fill )
    {
        return fillRect( slider, contentsRect );
    }

    if ( subControl == Q::Handle )
    {
        return handleRect( slider, contentsRect );
    }

    if ( subControl == Q::Scale )
    {
        return scaleRect( slider, contentsRect );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

int QskSliderSkinlet::sampleCount( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl ) const
{
    const auto slider = static_cast< const QskSlider* >( skinnable );

    if( slider->snap() )
    {
        const auto num = qCeil( slider->boundaryLength() / slider->stepSize() ) + 1;
        return num;
    }
    else
    {
        return ( subControl == Q::GrooveStopIndicators ) ? 1 : 0;
    }
}

QRectF QskSliderSkinlet::sampleRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl, int index ) const
{
    const auto slider = static_cast< const QskSlider* >( skinnable );

    auto r = qskInnerValueRect( slider, contentsRect );

    const auto size = slider->strutSizeHint( subControl );

    const auto filledPoints = qFloor( ( slider->value() - slider->minimum() ) / slider->stepSize() );

    if( slider->snap())
    {
        if( slider->snap()
            && ( ( index >= filledPoints && subControl == Q::FillStopIndicators )
                || ( index < filledPoints && subControl == Q::GrooveStopIndicators ) ) )
        {
            return {};
        }
    }

    const auto pos = slider->snap() ? slider->minimum() + index * slider->stepSize() : slider->maximum();

    if( slider->orientation() == Qt::Horizontal )
    {
        const auto y = r.center().y() - size.height() / 2;
        r.setTop( qMax( 0.0, y ) );
        const auto x = r.left() + slider->valueAsRatio( pos ) * r.width() - size.width() / 2;
        r.setLeft( qMax( 0.0, x ) );
    }
    else
    {
        const auto x = r.center().x() - size.width() / 2;
        r.setLeft( qMax( 0.0, x ) );
        const auto y = r.bottom() - slider->valueAsRatio( pos ) * r.height() - size.height() / 2;
        r.setTop( qMax( 0.0, y ) );
    }

    r.setHeight( size.height() );
    r.setWidth( size.width() );

    return r;
}

QskAspect::States QskSliderSkinlet::sampleStates( const QskSkinnable*, QskAspect::Subcontrol, int ) const
{
    return {};
}

QSGNode* QskSliderSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto slider = static_cast< const QskSlider* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            return updateBoxNode( slider, node, Q::Panel );
        }

        case GrooveRole:
        {
            return updateBoxNode( slider, node, Q::Groove );
        }

        case FillRole:
        {
            return updateBoxNode( slider, node, Q::Fill );
        }

        case GrooveStopIndicatorsRole:
        {
            return updateSeriesNode( slider, Q::GrooveStopIndicators, node );
        }

        case FillStopIndicatorsRole:
        {
            return updateSeriesNode( slider, Q::FillStopIndicators, node );
        }

        case HandleRole:
        {
            return updateBoxNode( slider, node, Q::Handle );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskSliderSkinlet::updateSampleNode( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, int index, QSGNode* node ) const
{
    const auto slider = static_cast< const QskSlider* >( skinnable );
    const auto rect = sampleRect( slider, slider->contentsRect(), subControl, index );

    return updateBoxNode( skinnable, node, rect, subControl );
}

QRectF QskSliderSkinlet::panelRect(
    const QskSlider* slider, const QRectF& contentsRect ) const
{
    auto r = contentsRect;

    const qreal size = slider->metric( Q::Panel | QskAspect::Size ); // 0: no hint
    if ( size > 0 && size < r.height() )
    {
        const auto alignment = slider->alignmentHint( Q::Panel );

        if ( slider->orientation() == Qt::Horizontal )
            r = qskAlignedRectF( r, r.width(), size, alignment & Qt::AlignVertical_Mask );
        else
            r = qskAlignedRectF( r, size, r.height(), alignment & Qt::AlignHorizontal_Mask );
    }

    return r;
}

QRectF QskSliderSkinlet::innerRect( const QskSlider* slider,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    auto r = qskInnerPanelRect( slider, contentsRect );

    QskSkinHintStatus status;

    const qreal extent = slider->metric( subControl | QskAspect::Size, &status );

    if ( slider->orientation() == Qt::Horizontal )
    {
        if ( status.isValid() && ( extent < r.height() ) )
        {
            r.setTop( r.center().y() - 0.5 * extent );
            r.setHeight( extent );
        }
    }
    else
    {
        if ( status.isValid() && ( extent < r.width() ) )
        {
            r.setLeft( r.center().x() - 0.5 * extent );
            r.setWidth( extent );
        }
    }

    return r;
}

QRectF QskSliderSkinlet::grooveRect(
    const QskSlider* slider, const QRectF& contentsRect ) const
{
    const auto r = qskInnerPanelRect( slider, contentsRect );
    auto grooveRect = innerRect( slider, contentsRect, Q::Groove );
    const auto handleRect = slider->subControlRect( Q::Handle );

    if ( slider->orientation() == Qt::Horizontal )
    {
        grooveRect.setLeft( handleRect.right() );
        grooveRect.setRight( r.right() );
    }
    else
    {
        grooveRect.setBottom( handleRect.top() );
        grooveRect.setTop( r.top() );
    }

    return grooveRect;
}

QRectF QskSliderSkinlet::scaleRect(
    const QskSlider* slider, const QRectF& rect ) const
{
    return innerRect( slider, rect, Q::Groove );
}

QRectF QskSliderSkinlet::fillRect(
    const QskSlider* slider, const QRectF& contentsRect ) const
{
    const auto r = qskInnerPanelRect( slider, contentsRect );
    const auto handleRect = slider->subControlRect( Q::Handle );

    auto fillRect = innerRect( slider, contentsRect, Q::Fill );
    if ( slider->orientation() == Qt::Horizontal )
    {
        fillRect.setLeft( r.left() );
        fillRect.setRight( handleRect.left() );
    }
    else
    {
        fillRect.setBottom( r.bottom() );
        fillRect.setTop( handleRect.bottom() );
    }

    return fillRect;
}

QRectF QskSliderSkinlet::handleRect(
    const QskSlider* slider, const QRectF& contentsRect ) const
{
    auto handleSize = slider->strutSizeHint( Q::Handle );
    const auto pos = qBound( 0.0, slider->handlePosition(), 1.0 );

    const auto r = qskInnerValueRect( slider, contentsRect );
    auto center = r.center();

    if ( slider->orientation() == Qt::Horizontal )
    {
        if ( handleSize.height() < 0.0 )
            handleSize.setHeight( r.height() );

        if ( handleSize.width() < 0.0 )
            handleSize.setWidth( handleSize.height() );

        center.setX( r.left() + pos * r.width() );
    }
    else
    {
        if ( handleSize.width() < 0.0 )
            handleSize.setWidth( r.width() );

        if ( handleSize.height() < 0.0 )
            handleSize.setHeight( handleSize.width() );

        center.setY( r.bottom() - pos * r.height() );
    }

    QRectF handleRect( 0, 0, handleSize.width(), handleSize.height() );
    handleRect.moveCenter( center );

    handleRect = handleRect.marginsRemoved( slider->marginHint( Q::Handle ) );

    return handleRect;
}

QSizeF QskSliderSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto panelHint = skinnable->strutSizeHint( Q::Panel );
    const auto grooveHint = skinnable->strutSizeHint( Q::Groove );
    const auto fillHint = skinnable->strutSizeHint( Q::Fill );
    const auto handleHint = skinnable->strutSizeHint( Q::Handle );

    auto hint = panelHint;
    hint = hint.expandedTo( grooveHint );
    hint = hint.expandedTo( fillHint );
    hint = hint.expandedTo( handleHint );

    return hint;
}

#include "moc_QskSliderSkinlet.cpp"
