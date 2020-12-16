/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSliderSkinlet.h"
#include "QskSlider.h"

#include "QskAspect.h"
#include "QskBoxBorderMetrics.h"
#include "QskFunctions.h"

static inline QMarginsF qskPadding(
    const QskSlider* slider, QskAspect::Subcontrol subControl )
{
    return slider->paddingHint( subControl ) +
        slider->boxBorderMetricsHint( subControl ).widths();
}

QskSliderSkinlet::QskSliderSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, GrooveRole, FillRole, HandleRole } );
}

QskSliderSkinlet::~QskSliderSkinlet()
{
}

QRectF QskSliderSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto slider = static_cast< const QskSlider* >( skinnable );

    if ( subControl == QskSlider::Panel )
    {
        return panelRect( slider, contentsRect );
    }

    if ( subControl == QskSlider::Groove )
    {
        return grooveRect( slider, contentsRect );
    }

    if ( subControl == QskSlider::Fill )
    {
        return fillRect( slider, contentsRect );
    }

    if ( subControl == QskSlider::Handle )
    {
        return handleRect( slider, contentsRect );
    }

    if ( subControl == QskSlider::Scale )
    {
        return scaleRect( slider, contentsRect );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskSliderSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto slider = static_cast< const QskSlider* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            return updateBoxNode( slider, node, QskSlider::Panel );
        }

        case GrooveRole:
        {
            return updateBoxNode( slider, node, QskSlider::Groove );
        }

        case FillRole:
        {
            return updateBoxNode( slider, node, QskSlider::Fill );
        }

        case HandleRole:
        {
            return updateBoxNode( slider, node, QskSlider::Handle );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRectF QskSliderSkinlet::panelRect(
    const QskSlider* slider, const QRectF& contentsRect ) const
{
    auto r = contentsRect;

    const qreal size = slider->metric( QskSlider::Panel | QskAspect::Size ); // 0: no hint
    if ( size > 0 && size < r.height() )
    {
        const auto alignment = slider->alignmentHint( QskSlider::Panel );

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
    QRectF r = subControlRect( slider, contentsRect, QskSlider::Panel );
    r = r.marginsRemoved( qskPadding( slider, QskSlider::Panel ) );

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
    const QskSlider* slider, const QRectF& rect ) const
{
    return innerRect( slider, rect, QskSlider::Groove );
}

QRectF QskSliderSkinlet::scaleRect(
    const QskSlider* slider, const QRectF& rect ) const
{
    return innerRect( slider, rect, QskSlider::Groove );
}

QRectF QskSliderSkinlet::fillRect(
    const QskSlider* slider, const QRectF& contentsRect ) const
{
   using Q = QskSlider;

    auto r = subControlRect( slider, contentsRect, Q::Panel );
    r = r.marginsRemoved( qskPadding( slider, Q::Panel ) );

    qreal pos = slider->metric( Q::Handle | QskAspect::Position );
    pos = qBound( 0.0, pos, 1.0 );

    auto fillRect = innerRect( slider, contentsRect, Q::Fill );
    if ( slider->orientation() == Qt::Horizontal )
    {
        fillRect.setLeft( r.left() );
        fillRect.setRight( r.left() + pos * r.width() );
    }
    else
    {
        fillRect.setBottom( r.bottom() );
        fillRect.setTop( r.bottom() - pos * r.height() );
    }

    return fillRect;
}

QRectF QskSliderSkinlet::handleRect(
    const QskSlider* slider, const QRectF& contentsRect ) const
{
    using Q = QskSlider;

    auto r = subControlRect( slider, contentsRect, Q::Panel );
    r = r.marginsRemoved( qskPadding( slider, Q::Panel ) );

    const bool isHorizontal = slider->orientation() == Qt::Horizontal;

    qreal extent;

    {
        QskSkinHintStatus status;

        extent = slider->metric( Q::Handle | QskAspect::Size, &status );
        if ( !status.isValid() )
            extent = isHorizontal ? r.height() : r.width();
    }

    QRectF handleRect( 0, 0, extent, extent );
    if ( extent > 0 )
    {
        handleRect = handleRect.marginsRemoved(
            slider->marginHint( Q::Handle) );
    }

    qreal pos = slider->metric( Q::Handle | QskAspect::Position );
    pos = qBound( 0.0, pos, 1.0 );

    if ( slider->orientation() == Qt::Horizontal )
    {
        pos = r.left() + pos * r.width();
        handleRect.moveCenter( QPointF( pos, r.center().y() ) );
    }
    else
    {
        pos = r.bottom() - pos * r.height();
        handleRect.moveCenter( QPointF( r.center().x(), pos ) );
    }

    return handleRect;
}

#include "moc_QskSliderSkinlet.cpp"
