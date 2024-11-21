/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSliderSkinlet.h"
#include "QskSlider.h"
#include "QskFunctions.h"

using Q = QskSlider;

static QRectF qskInnerRect( const QskSlider* slider,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl )
{
    auto r = slider->subControlContentsRect( contentsRect, Q::Panel );

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

QskSliderSkinlet::QskSliderSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, GrooveRole, FillRole, TicksRole, HandleRole } );
}

QskSliderSkinlet::~QskSliderSkinlet()
{
}

QRectF QskSliderSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto slider = static_cast< const QskSlider* >( skinnable );

    if ( subControl == Q::Panel )
        return panelRect( slider, contentsRect );

    if ( subControl == Q::Groove )
        return qskInnerRect( slider, contentsRect, Q::Groove );

    if ( subControl == Q::Fill )
        return fillRect( slider, contentsRect );

    if ( subControl == Q::Scale )
        return subControlRect( skinnable, contentsRect, Q::Groove );

    if ( subControl == Q::Handle )
        return handleRect( slider, contentsRect );

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskSliderSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto slider = static_cast< const QskSlider* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
            return updateBoxNode( slider, node, Q::Panel );

        case GrooveRole:
            return updateBoxNode( slider, node, Q::Groove );

        case FillRole:
            return updateBoxNode( slider, node, Q::Fill );

        case HandleRole:
            return updateBoxNode( slider, node, Q::Handle );

        case TicksRole:
            return updateSeriesNode( slider, Q::Tick, node );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

int QskSliderSkinlet::sampleCount( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == Q::Tick )
    {
        const auto slider = static_cast< const QskSlider* >( skinnable );
        return slider->visualGraduation().count();
    }

    return Inherited::sampleCount( skinnable, subControl );
}

QVariant QskSliderSkinlet::sampleAt( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, int index ) const
{
    if ( subControl == Q::Tick )
    {
        const auto slider = static_cast< const QskSlider* >( skinnable );
        return slider->visualGraduation().value( index );
    }

    return Inherited::sampleAt( skinnable, subControl, index );
}

QRectF QskSliderSkinlet::sampleRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl, int index ) const
{
    if ( subControl == Q::Tick )
    {
        const auto slider = static_cast< const QskSlider* >( skinnable );
        return tickRect( slider, contentsRect, index );
    }

    return Inherited::sampleRect( skinnable, contentsRect, subControl, index );
}

QSGNode* QskSliderSkinlet::updateSampleNode( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, int index, QSGNode* node ) const
{
    if ( subControl == Q::Tick )
    {
        const auto slider = static_cast< const QskSlider* >( skinnable );
        const auto rect = sampleRect( slider, slider->contentsRect(), subControl, index );

        return updateBoxNode( skinnable, node, rect, subControl );
    }

    return Inherited::updateSampleNode( skinnable, subControl, index, node );
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

QRectF QskSliderSkinlet::fillRect(
    const QskSlider* slider, const QRectF& contentsRect ) const
{
    const auto pos = qBound( 0.0, slider->handlePosition(), 1.0 );

    auto r = qskInnerRect( slider, contentsRect, QskSlider::Fill );

    auto scaleRect = subControlRect( slider, contentsRect, Q::Scale );

    if ( slider->orientation() == Qt::Horizontal )
        r.setRight( scaleRect.left() + pos * scaleRect.width() );
    else
        r.setTop( scaleRect.bottom() - pos * scaleRect.height() );

    return r;
}

QRectF QskSliderSkinlet::handleRect(
    const QskSlider* slider, const QRectF& contentsRect ) const
{
    auto handleSize = slider->strutSizeHint( Q::Handle );
    const auto pos = qBound( 0.0, slider->handlePosition(), 1.0 );

    const auto r = subControlRect( slider, contentsRect, Q::Scale );
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

    return handleRect;
}

QRectF QskSliderSkinlet::tickRect( const QskSlider* slider,
    const QRectF& contentsRect, int index ) const
{
    const auto tickValue = sampleAt( slider, Q::Tick, index );
    if ( !tickValue.canConvert< qreal >() )
        return QRectF();

    const auto tickPos = slider->valueAsRatio( tickValue.value< qreal >() );

    const auto size = slider->strutSizeHint( Q::Tick );
    const auto r = subControlRect( slider, contentsRect, Q::Scale );

    qreal x, y;

    if( slider->orientation() == Qt::Horizontal )
    {
        x = tickPos * r.width() - 0.5 * size.width();
        y = 0.5 * ( r.height() - size.height() );
    }
    else
    {
        y = r.height() - ( tickPos * r.height() ) - 0.5 * size.height();
        x = 0.5 * ( r.width() - size.width() );
    }

    return QRectF( r.x() + x, r.y() + y, size.width(), size.height() );
}

QSizeF QskSliderSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    auto hint = skinnable->strutSizeHint( Q::Panel );
    hint = hint.expandedTo( skinnable->strutSizeHint( Q::Groove ) );
    hint = hint.expandedTo( skinnable->strutSizeHint( Q::Fill ) );
    hint = hint.expandedTo( skinnable->strutSizeHint( Q::Handle ) );

    return hint;
}

#include "moc_QskSliderSkinlet.cpp"
