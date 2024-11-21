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

static inline bool qskHasBoundaryTicks( const QskSlider* slider )
{
    return ( slider->graduationPolicy() == Qsk::Maybe ) && !slider->isSnapping();
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
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

int QskMaterial3SliderSkinlet::sampleCount( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == Q::Tick )
    {
        const auto slider = static_cast< const QskSlider* >( skinnable );

        if ( qskHasBoundaryTicks( slider ) )
        {
            const bool hasOrigin = false;

            // min/origin/max or max
            return hasOrigin ? 3 : 1;
        }
    }

    return Inherited::sampleCount( skinnable, subControl );
}

QVariant QskMaterial3SliderSkinlet::sampleAt( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, int index ) const
{
    if ( subControl == Q::Tick )
    {
        const auto slider = static_cast< const QskSlider* >( skinnable );

        if ( qskHasBoundaryTicks( slider ) )
        {
            switch( index )
            {
                case 1:
                    return slider->minimum();

        #if 0
                case 2:
                    return slider->origin();
        #endif

                default:
                    return slider->maximum();
            }

            return QVariant();
        }
    }

    return Inherited::sampleAt( skinnable, subControl, index );
}

QskAspect::States QskMaterial3SliderSkinlet::sampleStates(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl, int index ) const
{
    auto states = Inherited::sampleStates( skinnable, subControl, index );

    if ( subControl == Q::Tick )
    {
        const auto tickValue = sampleAt( skinnable, subControl, index );
        if ( tickValue.canConvert< qreal >() )
        {
            const auto slider = static_cast< const QskSlider* >( skinnable );

            if ( tickValue.value< qreal >() <= slider->value() )
                states |= QskMaterial3SliderSkinlet::Filled;
        }
    }

    return states;
}

#include "moc_QskMaterial3SliderSkinlet.cpp"
