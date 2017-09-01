/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSliderSkinlet.h"
#include "QskSlider.h"
#include "QskAspect.h"
#include "QskSkin.h"
#include "QskSkinRenderer.h"
#include "QskBoxOptions.h"
#include "QskBoxNode.h"
#include "QskFunctions.h"

static QMarginsF qskMargins(
    const QskSlider* slider, QskAspect::Subcontrol subControl )
{
    QMarginsF m = slider->marginsHint( subControl | QskAspect::Margin );

    if ( slider->orientation() == Qt::Vertical )
        m = QMarginsF( m.top(), m.right(), m.bottom(), m.left() );

    return m;
}

static QMarginsF qskPadding(
    const QskSlider* slider, QskAspect::Subcontrol subControl )
{
    QMarginsF m = slider->marginsHint( subControl | QskAspect::Padding )
        + slider->borderMetrics( subControl );

    if ( slider->orientation() == Qt::Vertical )
        m = QMarginsF( m.top(), m.right(), m.bottom(), m.left() );

    return m;
}

QskSliderSkinlet::QskSliderSkinlet( QskSkin* skin ):
    Inherited( skin )
{
    setNodeRoles( { PanelRole, GrooveRole, FillRole, HandleRole } );
}

QskSliderSkinlet::~QskSliderSkinlet() = default;

QRectF QskSliderSkinlet::subControlRect(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    const auto slider = static_cast< const QskSlider* >( skinnable );

    if ( subControl == QskSlider::Panel )
    {
        return panelRect( slider );
    }

    if ( subControl == QskSlider::Groove )
    {
        return grooveRect( slider );
    }

    if ( subControl == QskSlider::Fill )
    {
        return fillRect( slider );
    }

    if ( subControl == QskSlider::Handle )
    {
        return handleRect( slider );
    }

    if ( subControl == QskSlider::Scale )
    {
        return scaleRect( slider );
    }

    return Inherited::subControlRect( skinnable, subControl );
}

QSGNode* QskSliderSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto slider = static_cast< const QskSlider* >( skinnable );

    switch( nodeRole )
    {
        case PanelRole:
        {
            return updateBoxNode( slider, QskSlider::Panel, node );
        }

        case GrooveRole:
        {
            return updateBoxNode( slider, QskSlider::Groove, node );
        }

        case FillRole:
        {
            return updateBoxNode( slider, QskSlider::Fill, node );
        }

        case HandleRole:
        {
            return updateBoxNode( slider, QskSlider::Handle, node );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRectF QskSliderSkinlet::panelRect( const QskSlider* slider ) const
{
    using namespace QskAspect;

    const QskAspect::Subcontrol subControl = QskSlider::Panel;

    qreal size = slider->metric( subControl | Size ); // 0: no hint

    {
        /*
           We use MinimumHeight/MaximumHeight/Size for the "thickness" of
           to slider even if "Height" is not the expected value for
           vertical sliders. Maybe we should have the orientation as an aspect bit ?
         */

        const qreal minSize = slider->metric( subControl | MinimumHeight );
        const qreal maxSize = slider->metric( subControl | MaximumHeight );

        size = qMax( size, minSize );
        if ( maxSize > minSize )
            size = qMin( size, maxSize );
    }

    QRectF r = slider->contentsRect();

    if ( size > 0 && size < r.height() )
    {
        const Qt::Alignment alignment = 
            slider->flagHint<Qt::Alignment>( subControl | Alignment );

        if ( slider->orientation() == Qt::Horizontal )
            r = qskAlignedRectF( r, r.width(), size, alignment & Qt::AlignVertical_Mask );
        else
            r = qskAlignedRectF( r, size, r.height(), alignment & Qt::AlignHorizontal_Mask );
    }

    return r;
}

QRectF QskSliderSkinlet::innerRect( const QskSlider* slider,
    QskAspect::Subcontrol subControl ) const
{
    using namespace QskAspect;

    QRectF r = subControlRect( slider, QskSlider::Panel );
    r = r.marginsRemoved( qskPadding( slider, QskSlider::Panel ) );

    QskSkinHintStatus status;

    qreal thickness = slider->metric( subControl | Size, &status );

    if ( slider->orientation() == Qt::Horizontal )
    {
        if ( status.isValid() && thickness < r.height() )
        {
            r.setTop( r.center().y() - 0.5 * thickness );
            r.setHeight( thickness );
        }
    }
    else
    {
        if ( status.isValid() && thickness < r.width() )
        {
            r.setLeft( r.center().x() - 0.5 * thickness );
            r.setWidth( thickness );
        }
    }

    return r;
}

QRectF QskSliderSkinlet::grooveRect( const QskSlider* slider ) const
{
    return innerRect( slider, QskSlider::Groove );
}

QRectF QskSliderSkinlet::scaleRect( const QskSlider* slider ) const
{
    return innerRect( slider, QskSlider::Groove );
}

QRectF QskSliderSkinlet::fillRect( const QskSlider* slider ) const
{
    using namespace QskAspect;

    QRectF r = subControlRect( slider, QskSlider::Panel );
    r = r.marginsRemoved( qskPadding( slider, QskSlider::Panel ) );

    qreal pos = slider->metric( QskSlider::Handle | Position );
    pos = qBound( 0.0, pos, 1.0 );

    QRectF fillRect = innerRect( slider, QskSlider::Fill );
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

QRectF QskSliderSkinlet::handleRect( const QskSlider* slider ) const
{
    using namespace QskAspect;

    QRectF r = subControlRect( slider, QskSlider::Panel );
    r = r.marginsRemoved( qskPadding( slider, QskSlider::Panel ) );

    const bool isHorizontal = slider->orientation() == Qt::Horizontal;

    qreal thickness;

    {
        QskSkinHintStatus status;

        thickness = slider->metric( QskSlider::Handle | Size, &status );
        if ( !status.isValid() )
            thickness = isHorizontal ? r.height() : r.width();
    }

    QRectF handleRect( 0, 0, thickness, thickness );
    if ( thickness > 0 )
        handleRect = handleRect.marginsRemoved( qskMargins( slider, QskSlider::Handle ) );

    qreal pos = slider->metric( QskSlider::Handle | Position );
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

QSGNode* QskSliderSkinlet::updateBoxNode( const QskSlider* slider,
    QskAspect::Subcontrol subControl, QSGNode* node ) const
{
    const QRectF rect = subControlRect( slider, subControl );
    if ( rect.isEmpty() )
        return nullptr;

    int rotation = 0;
    if ( subControl != QskSlider::Handle )
        rotation = ( slider->orientation() == Qt::Horizontal ) ? 0 : 1;

    const QskBoxOptions options =
        QskSkinRenderer::boxOptions( slider, rect, subControl, rotation );

    const QRectF boxRect = rect.marginsAdded( options.shadows );

    if ( boxRect.isEmpty() || !options.isVisible() )
        return nullptr;

    auto boxNode = static_cast< QskBoxNode* >( node );
    if ( boxNode == nullptr )
        boxNode = new QskBoxNode;

    boxNode->setBoxData( boxRect, options );
    return boxNode;
}

#include "moc_QskSliderSkinlet.cpp"
