/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskMaterial3SliderSkinlet.h"

#include <QskSlider.h>
#include <QskClipNode.h>
#include <QskSGNode.h>

#include <qmath.h>

// the color of stop indicators is different, when being on top of the filling
QSK_SYSTEM_STATE( QskMaterial3SliderSkinlet, Filled, QskAspect::FirstUserState >> 1 )

using Q = QskSlider;

static inline bool qskHasOrigin( const QskSlider* )
{
    return false; // TODO
}

static inline qreal qskTickValue( const QskSlider* slider, int index )
{
    if( slider->snap() )
        return slider->minimum() + index * slider->stepSize();

    if ( qskHasOrigin( slider ) )
    {
        switch( index )
        {
            case 0:
                return slider->minimum();

#if 0
            case 1:
                return slider->origin();
#endif
        }
    }

    return slider->maximum();
}

QskMaterial3SliderSkinlet::QskMaterial3SliderSkinlet( QskSkin* skin )
    : Inherited( skin )
{
}

QRectF QskMaterial3SliderSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == Q::Scale )
    {
        auto r = Inherited::subControlRect( skinnable, contentsRect, Q::Scale );

        const auto handleSize = skinnable->strutSizeHint( Q::Handle );

        const auto slider = static_cast< const QskSlider* >( skinnable );
        if( slider->orientation() == Qt::Horizontal )
        {
            const auto m = 0.5 * handleSize.width();
            r.adjust( m, 0.0, -m, 0.0 );
        }
        {
            const auto m = 0.5 * handleSize.height();
            r.adjust( 0.0, m, 0.0, -m );
        }

        return r;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskMaterial3SliderSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    switch( nodeRole )
    {
        case GrooveRole:
        case FillRole:
        {
            auto clippedNode = QskSGNode::findChildNode( node, nodeRole ); 
            clippedNode = Inherited::updateSubNode( skinnable, nodeRole, clippedNode );

            if ( clippedNode )
            {
                const auto slider = static_cast< const QskSlider* >( skinnable );

                auto clipNode = QskSGNode::ensureNode< QskClipNode >( node );

                clipNode->setRegion( slider->subControlRect( Q::Panel ),
                    slider->subControlRect( Q::Handle ) );

                QskSGNode::setNodeRole( clippedNode, nodeRole );
                QskSGNode::setParentNode( clippedNode, clipNode );

                return clipNode;
            }

            return nullptr;
        }

        case TicksRole:
        {
            return updateSeriesNode( skinnable, Q::Ticks, node );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

int QskMaterial3SliderSkinlet::sampleCount( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == Q::Ticks )
    {
        const auto slider = static_cast< const QskSlider* >( skinnable );

        if( slider->snap() )
            return qCeil( slider->boundaryLength() / slider->stepSize() ) + 1;

        // min/origin/max or max
        return qskHasOrigin( slider ) ? 3 : 1;
    }

    return Inherited::sampleCount( skinnable, subControl );
}

QRectF QskMaterial3SliderSkinlet::sampleRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl, int index ) const
{
    if ( subControl != Q::Ticks )
        return Inherited::sampleRect( skinnable, contentsRect, subControl, index );

    const auto slider = static_cast< const QskSlider* >( skinnable );

    const auto tickPos = slider->valueAsRatio( qskTickValue( slider, index ) );

    const auto size = skinnable->strutSizeHint( Q::Ticks );
    const auto r = subControlRect( skinnable, contentsRect, Q::Scale );

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

QskAspect::States QskMaterial3SliderSkinlet::sampleStates(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl, int index ) const
{
    auto states = Inherited::sampleStates( skinnable, subControl, index );

    if ( subControl == Q::Ticks )
    {
        const auto slider = static_cast< const QskSlider* >( skinnable );
        if ( qskTickValue( slider, index ) <= slider->value() )
            states |= QskMaterial3SliderSkinlet::Filled;
    }

    return states;
}

QSGNode* QskMaterial3SliderSkinlet::updateSampleNode( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, int index, QSGNode* node ) const
{
    if ( subControl == Q::Ticks )
    {
        const auto slider = static_cast< const QskSlider* >( skinnable );
        const auto rect = sampleRect( slider, slider->contentsRect(), subControl, index );

        return updateBoxNode( skinnable, node, rect, subControl );
    }

    return Inherited::updateSampleNode( skinnable, subControl, index, node );
}

#include "moc_QskMaterial3SliderSkinlet.cpp"
