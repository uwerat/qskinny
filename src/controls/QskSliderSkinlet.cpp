/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSliderSkinlet.h"
#include "QskSlider.h"

#include "QskAspect.h"
#include "QskBoxBorderMetrics.h"
#include "QskFunctions.h"

static inline QRectF qskInnerPanelRect(
    const QskSlider* slider, const QRectF& contentsRect )
{
    using Q = QskSlider;

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
    const auto r = qskInnerPanelRect( slider, contentsRect );
    const auto pos = qBound( 0.0, slider->handlePosition(), 1.0 );

    auto fillRect = innerRect( slider, contentsRect, QskSlider::Fill );
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

    auto handleSize = slider->strutSizeHint( Q::Handle );
    const auto pos = qBound( 0.0, slider->handlePosition(), 1.0 );

    const auto r = qskInnerPanelRect( slider, contentsRect );
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

    const auto slider = static_cast< const QskSlider* >( skinnable );

    // strutSizeHint( ... ) ???
    const qreal m1 = slider->metric( QskSlider::Panel | QskAspect::Size );
    const qreal m2 = 4 * m1;

    return ( slider->orientation() == Qt::Horizontal )
        ? QSizeF( m2, m1 ) : QSizeF( m1, m2 );
}

#include "moc_QskSliderSkinlet.cpp"
