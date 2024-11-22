/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskMaterial3SliderSkinlet.h"

#include <QskSlider.h>
#include <QskClipNode.h>
#include <QskSGNode.h>

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

        if ( ( slider->graduationPolicy() == Qsk::Maybe ) && !slider->isSnapping() )
        {
            const bool hasOrigin = false;

            // min/origin/max or max
            return hasOrigin ? 3 : 1;
        }
    }

    return Inherited::sampleCount( skinnable, subControl );
}

QVector< qreal > QskMaterial3SliderSkinlet::graduation( const QskSlider* slider ) const
{
    QVector< qreal > graduation;

    if ( ( slider->graduationPolicy() == Qsk::Maybe ) && !slider->isSnapping() )
    {
        const bool hasOrigin = false;

        if ( hasOrigin )
        {
            graduation.reserve( 3 );
            graduation += slider->minimum();
#if 1
            graduation += slider->maximum(); // origin !!!
#endif
            graduation += slider->maximum();
        }
        else
        {
            graduation.reserve( 1 );
            graduation += slider->maximum();
        }
    }
    else
    {
        const auto g = Inherited::graduation( slider );

        // adding the boundaries

        graduation.reserve( g.count() + 2 );
        graduation += slider->minimum();
        graduation += g;
        graduation += slider->maximum();
    }

    return graduation;
}

#include "moc_QskMaterial3SliderSkinlet.cpp"
