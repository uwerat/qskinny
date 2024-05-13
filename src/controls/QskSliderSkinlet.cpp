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
    setNodeRoles( { PanelRole, GrooveRole, FillRole, HandleRole, RippleRole, LabelContainerRole, LabelTextRole } );
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

    if ( subControl == QskSlider::Ripple )
    {
        return rippleRect( slider, contentsRect );
    }

    if ( subControl == QskSlider::LabelContainer )
    {
        return labelContainerRect( slider, contentsRect );
    }

    if ( subControl == QskSlider::LabelText )
    {
        return labelContainerRect( slider, contentsRect );
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

        case RippleRole:
        {
            return updateBoxNode( slider, node, QskSlider::Ripple );
        }

        case LabelContainerRole:
        {
            return updateBoxNode( slider, node, QskSlider::LabelContainer );
        }

        case LabelTextRole:
        {
            const auto text = labelValue( slider );
            return updateTextNode( slider, node, text, QskSlider::LabelText );
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
    const QskSlider* slider, const QRectF& contentsRect ) const
{
    const auto r = qskInnerPanelRect( slider, contentsRect );
    auto grooveRect = innerRect( slider, contentsRect, QskSlider::Groove );
    const auto pos = qBound( 0.0, slider->handlePosition(), 1.0 );

    if ( slider->orientation() == Qt::Horizontal )
    {
        grooveRect.setLeft( r.left() + pos * r.width() );
        grooveRect.setRight( r.right() );
    }
    else
    {
        grooveRect.setBottom( r.bottom() - pos * r.height() );
        grooveRect.setTop( r.top() );
    }

    return grooveRect;
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

QRectF QskSliderSkinlet::rippleRect(
    const QskSlider* slider, const QRectF& rect ) const
{
    const auto rippleSize = slider->strutSizeHint( QskSlider::Ripple );
    const auto handleSize = slider->strutSizeHint( QskSlider::Handle );

    const auto w = ( rippleSize.width() - handleSize.width() ) / 2;
    const auto h = ( rippleSize.height() - handleSize.height() ) / 2;

    auto r = handleRect( slider, rect );
    r = r.marginsAdded( { w, h, w, h } );

    return r;
}

QRectF QskSliderSkinlet::labelContainerRect(
    const QskSlider* slider, const QRectF& rect ) const
{
    auto size = slider->strutSizeHint( QskSlider::LabelContainer );

    if( size.isEmpty() )
    {
        return {};
    }

    QFontMetricsF fm( slider->effectiveFont( QskSlider::LabelText ) );
    const auto w = qskHorizontalAdvance( fm, labelValue( slider ) );

    const auto padding = slider->paddingHint( QskSlider::LabelContainer );
    const auto h = fm.height() + padding.top() + padding.bottom();

    size = size.expandedTo( { w, h } );

    const auto hr = subControlRect( slider, rect, QskSlider::Handle );
    const auto margins = slider->marginHint( QskSlider::LabelContainer );

    qreal x, y;

    if( slider->orientation() == Qt::Horizontal )
    {
        x = hr.center().x() - size.width() / 2;
        y = hr.top() - margins.bottom() - size.height();
    }
    else
    {
        x = hr.left() - size.width() - margins.left();
        y = hr.center().y() - size.height() / 2;
    }

    const QRectF r( x, y, size.width(), size.height() );

    return r;
}

QSizeF QskSliderSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto panelHint = skinnable->strutSizeHint( QskSlider::Panel );
    const auto grooveHint = skinnable->strutSizeHint( QskSlider::Groove );
    const auto fillHint = skinnable->strutSizeHint( QskSlider::Fill );
    const auto handleHint = skinnable->strutSizeHint( QskSlider::Handle );

    auto hint = panelHint;
    hint = hint.expandedTo( grooveHint );
    hint = hint.expandedTo( fillHint );
    hint = hint.expandedTo( handleHint );

    return hint;
}

QString QskSliderSkinlet::labelValue( const QskSlider* slider ) const
{
    return QString::number( slider->value(), 'f', 1 );
}

#include "moc_QskSliderSkinlet.cpp"
