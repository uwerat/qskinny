/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSliderSkinlet.h"
#include "QskSlider.h"
#include "QskFunctions.h"
#include "QskIntervalF.h"

#include <qvector.h>
#include <qpair.h>
#include <qmath.h>
#include <qfontmetrics.h>

// the color of graduation ticks might different, when being on top of the filling
QSK_SYSTEM_STATE( QskSliderSkinlet, Filled, QskAspect::FirstUserState >> 1 )

using Q = QskSlider;

static inline qreal qskSubcontrolExtent(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl )
{
    return skinnable->metric( subControl | QskAspect::Size, -1.0 );
}

static inline bool qskHasFilling( const QskSlider* slider )
{
    const auto policy = slider->flagHint< Qsk::Policy >(
        Q::Fill | QskAspect::Option, Qsk::Always );

    switch( policy )
    {
        case Qsk::Never:
            return false;

        case Qsk::Maybe:
            return qskFuzzyCompare( slider->origin(), slider->minimum() );

        default:
            return true;
    }
}

static QRectF qskInnerRect( const QskSlider* slider,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl )
{
    auto r = slider->subControlContentsRect( contentsRect, Q::Panel );

    const qreal extent = qskSubcontrolExtent( slider, subControl );

    if ( extent >= 0.0 )
    {
        if ( slider->orientation() == Qt::Horizontal )
        {
            if ( extent < r.height() )
            {
                r.setTop( r.center().y() - 0.5 * extent );
                r.setHeight( extent );
            }
        }
        else
        {
            if ( extent < r.width() )
            {
                r.setLeft( r.center().x() - 0.5 * extent );
                r.setWidth( extent );
            }
        }
    }

    return r;
}

static inline QPair< qreal, qreal > qskTickSpan( qreal min, qreal max, qreal length )
{
    if ( length >= 0.0 )
    {
        // using the center of [min,max]
        min += 0.5 * ( max - min - length );
        max = min + length;
    }

    return { min, max };
}


QskSliderSkinlet::QskSliderSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, GrooveRole, FillRole, TicksRole, HandleRole,
        LabelContainerRole, LabelTextRole } );
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

    if ( subControl == Q::LabelContainer )
        return labelContainerRect( slider, contentsRect );

    if ( subControl == Q::LabelText )
        return labelContainerRect( slider, contentsRect );

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

        case LabelContainerRole:
            return updateBoxNode( slider, node, Q::LabelContainer );

        case LabelTextRole:
            return updateTextNode( slider, node, slider->valueText(), Q::LabelText );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

int QskSliderSkinlet::sampleCount( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == Q::Tick )
    {
        const auto slider = static_cast< const QskSlider* >( skinnable );
        return graduation( slider ).count();
    }

    return Inherited::sampleCount( skinnable, subControl );
}

QVariant QskSliderSkinlet::sampleAt( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, int index ) const
{
    if ( subControl == Q::Tick )
    {
        const auto slider = static_cast< const QskSlider* >( skinnable );
        return graduation( slider ).value( index );
    }

    return Inherited::sampleAt( skinnable, subControl, index );
}

QskAspect::States QskSliderSkinlet::sampleStates(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl, int index ) const
{
    auto states = Inherited::sampleStates( skinnable, subControl, index );

    const auto slider = static_cast< const QskSlider* >( skinnable );

    if ( subControl == Q::Tick && qskHasFilling( slider ) )
    {
        const auto tickValue = sampleAt( skinnable, subControl, index );
        if ( tickValue.canConvert< qreal >() )
        {
            const auto intv = QskIntervalF::normalized(
                slider->origin(), slider->value() );

            if ( intv.contains( tickValue.value< qreal >() ) )
                states |= Filled;
        }
    }

    return states;
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

    const qreal extent = qskSubcontrolExtent( slider, Q::Panel );
    if ( extent >= 0 && extent < r.height() )
    {
        const auto alignment = slider->alignmentHint( Q::Panel );

        if ( slider->orientation() == Qt::Horizontal )
        {
            r = qskAlignedRectF( r, r.width(),
                extent, alignment & Qt::AlignVertical_Mask );
        }
        else
        {
            r = qskAlignedRectF( r, extent, r.height(),
                alignment & Qt::AlignHorizontal_Mask );
        }
    }

    return r;
}

QRectF QskSliderSkinlet::fillRect(
    const QskSlider* slider, const QRectF& contentsRect ) const
{
    if ( !qskHasFilling( slider ) )
        return QRectF();

    auto pos1 = slider->valueAsRatio( slider->origin() );
    auto pos2 = qBound( 0.0, slider->positionHint( Q::Handle ), 1.0 );

    if ( slider->isInverted() )
    {
        pos1 = 1.0 - pos1;
        pos2 = 1.0 - pos2;
    }

    if ( pos1 > pos2 )
        qSwap( pos1, pos2 );

    auto r = qskInnerRect( slider, contentsRect, Q::Fill );

    auto scaleRect = subControlRect( slider, contentsRect, Q::Scale );

    if ( slider->orientation() == Qt::Horizontal )
    {
        if ( !qFuzzyIsNull( pos1 ) )
            r.setLeft( scaleRect.left() + pos1 * scaleRect.width() );

        r.setRight( scaleRect.left() + pos2 * scaleRect.width() );
    }
    else
    {
        if ( !qFuzzyIsNull( pos1 ) )
            r.setBottom( scaleRect.bottom() - pos1 * scaleRect.height() );

        r.setTop( scaleRect.bottom() - pos2 * scaleRect.height() );
    }

    return r;
}

QRectF QskSliderSkinlet::handleRect(
    const QskSlider* slider, const QRectF& contentsRect ) const
{
    auto handleSize = slider->strutSizeHint( Q::Handle );
    const auto pos = qBound( 0.0, slider->positionHint( Q::Handle ), 1.0 );

    const auto r = subControlRect( slider, contentsRect, Q::Scale );
    auto center = r.center();

    if ( slider->orientation() == Qt::Horizontal )
    {
        if ( handleSize.height() < 0.0 )
            handleSize.setHeight( r.height() );

        if ( handleSize.width() < 0.0 )
            handleSize.setWidth( handleSize.height() );

        if ( slider->isInverted() )
            center.setX( r.right() - pos * r.width() );
        else
            center.setX( r.left() + pos * r.width() );
    }
    else
    {
        if ( handleSize.width() < 0.0 )
            handleSize.setWidth( r.width() );

        if ( handleSize.height() < 0.0 )
            handleSize.setHeight( handleSize.width() );

        if ( slider->isInverted() )
            center.setY( r.top() + pos * r.height() );
        else
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

    auto tickPos = slider->valueAsRatio( tickValue.value< qreal >() );
    if ( slider->isInverted() )
        tickPos = 1.0 - tickPos;

    const auto r = subControlRect( slider, contentsRect, Q::Scale );

    const auto padding = slider->paddingHint( Q::Scale );
    const auto size = slider->strutSizeHint( Q::Tick );

    if( slider->orientation() == Qt::Horizontal )
    {
        const auto x = tickPos * r.width() - 0.5 * size.width();

        const auto span = qskTickSpan(
            padding.top(), r.height() - padding.bottom(), size.height() );

        return QRectF( r.x() + x, r.y() + span.first,
            size.width(), span.second - span.first );
    }
    else
    {
        const auto y = tickPos * r.height() + 0.5 * size.height();

        const auto span = qskTickSpan(
            padding.left(), r.width() - padding.right(), size.width() );

        return QRectF( r.x() + span.first, r.bottom() - y,
            span.second - span.first, size.height() );
    }
}

QRectF QskSliderSkinlet::labelContainerRect(
    const QskSlider* slider, const QRectF& rect ) const
{
    auto size = slider->strutSizeHint( Q::LabelContainer );

    if( size.isEmpty() )
        return QRectF();

    const QFontMetricsF fm( slider->effectiveFont( Q::LabelText ) );
    const auto w = qskHorizontalAdvance( fm, slider->valueText() );

    const auto padding = slider->paddingHint( Q::LabelContainer );
    const auto h = fm.height() + padding.top() + padding.bottom();

    size = size.expandedTo( { w, h } );

    const auto hr = subControlRect( slider, rect, Q::Handle );
    const auto margins = slider->marginHint( Q::LabelContainer );

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

    return QRectF( x, y, size.width(), size.height() );
}
QSizeF QskSliderSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    auto extent = qskSubcontrolExtent( skinnable, Q::Panel );
    extent = qMax( extent, qskSubcontrolExtent( skinnable, Q::Groove ) );
    extent = qMax( extent, qskSubcontrolExtent( skinnable, Q::Fill ) );

    const auto slider = static_cast< const QskSlider* >( skinnable );

    auto hint = skinnable->strutSizeHint( Q::Handle );

    if ( slider->orientation() == Qt::Horizontal )
        hint.setHeight( qMax( hint.height(), extent ) );
    else
        hint.setWidth( qMax( hint.width(), extent ) );

    return hint;
}

bool QskSliderSkinlet::hasGraduation( const QskSlider* slider ) const
{
    if ( slider->stepSize() )
    {
        const auto policy = slider->flagHint< Qsk::Policy >(
            Q::Tick | QskAspect::Option, Qsk::Never );

        switch( policy )
        {
            case Qsk::Always:
                return true;

            case Qsk::Maybe:
                return slider->isSnapping();

            case Qsk::Never:
                return false;
        }
    }

    return false;
}

QVector< qreal > QskSliderSkinlet::graduation( const QskSlider* slider ) const
{
    QVector< qreal > graduation;

    if ( hasGraduation( slider ) )
    {
        const auto from = slider->minimum();
        const auto to = slider->maximum();

        auto step = slider->stepSize();
        if ( from > to )
            step = -step;

        const auto n = qCeil( ( to - from ) / step ) - 1;

        graduation.reserve( n );

        for ( int i = 1; i <= n; i++ )
            graduation += from + i * step;
    }

    return graduation;
}

#include "moc_QskSliderSkinlet.cpp"
