/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPushButtonSkinlet.h"
#include "QskPushButton.h"

#include "QskAnimationHint.h"
#include "QskGraphic.h"
#include "QskSubcontrolLayoutEngine.h"
#include "QskSGNode.h"

static inline Qt::Orientation qskOrientation( const QskPushButton* button )
{
    // For the moment we only handle the orientation TODO ...

    const auto direction = button->flagHint(
        QskPushButton::Panel | QskAspect::Direction, Qsk::LeftToRight );

    if ( direction == Qsk::LeftToRight || direction == Qsk::RightToLeft )
        return Qt::Horizontal;
    else
        return Qt::Vertical;
}

namespace
{
    class LayoutEngine : public QskSubcontrolLayoutEngine
    {
      public:
        LayoutEngine( const QskPushButton* button )
            : QskSubcontrolLayoutEngine( qskOrientation( button ) )
        {
            using Q = QskPushButton;

            setSpacing( button->spacingHint( Q::Panel ) );

            const auto graphicSourceSize = button->graphic().defaultSize();

            const bool hasText = !button->text().isEmpty();
            const bool hasGraphic = !graphicSourceSize.isEmpty();

            auto graphicElement = new GraphicElement( button, Q::Graphic );
            graphicElement->setSourceSize( graphicSourceSize );
            graphicElement->setIgnored( !hasGraphic );

            auto textElement = new TextElement( button, Q::Text );
            textElement->setText( button->text() );
            textElement->setIgnored( !hasText );

            using SP = QskSizePolicy;

            if ( hasText && !hasGraphic )
            {
                textElement->setSizePolicy( SP::Preferred, SP::Constrained );
            }
            else if ( hasGraphic && !hasText )
            {
                const auto size = graphicElement->effectiveStrutSize();

                if ( !size.isEmpty() )
                    graphicElement->setFixedSize( size );
                else
                    graphicElement->setSizePolicy( SP::Ignored, SP::ConstrainedExpanding );
            }
            else if ( hasText && hasGraphic )
            {
                if ( orientation() == Qt::Horizontal )
                {
                    graphicElement->setSizePolicy( SP::Constrained, SP::Fixed );
                    textElement->setSizePolicy( SP::Preferred, SP::Preferred );
                }
                else
                {
                    graphicElement->setSizePolicy( SP::Fixed, SP::Fixed );
                    textElement->setSizePolicy( SP::Preferred, SP::Constrained );
                }

                auto size = graphicElement->effectiveStrutSize();

                if ( size.isEmpty() )
                {
                    const auto h = 1.5 * button->effectiveFontHeight( Q::Text );

                    size.setWidth( graphicElement->widthForHeight( h ) );
                    size.setHeight( h );
                }

                graphicElement->setPreferredSize( size );
            }

            setElementAt( 0, graphicElement );
            setElementAt( 1, textElement );
        }
    };
}

QskPushButtonSkinlet::QskPushButtonSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, RippleRole, GraphicRole, TextRole } );
}

QskPushButtonSkinlet::~QskPushButtonSkinlet() = default;

QRectF QskPushButtonSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto button = static_cast< const QskPushButton* >( skinnable );

    if ( subControl == QskPushButton::Text )
    {
        return textRect( button, contentsRect );
    }

    if ( subControl == QskPushButton::Graphic )
    {
        return graphicRect( button, contentsRect );
    }

    if ( subControl == QskPushButton::Panel )
    {
        return contentsRect;
    }

    if ( subControl == QskPushButton::Ripple )
    {
        return rippleRect( button, contentsRect );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskPushButtonSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto button = static_cast< const QskPushButton* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            return updateBoxNode( button, node, QskPushButton::Panel );
        }

        case RippleRole:
        {
            return updateRippleNode( button, node );
        }

        case TextRole:
        {
            return updateTextNode( button, node );
        }

        case GraphicRole:
        {
            return updateGraphicNode( button, node,
                button->graphic(), QskPushButton::Graphic );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRectF QskPushButtonSkinlet::textRect(
    const QskPushButton* button, const QRectF& contentsRect ) const
{
    using Q = QskPushButton;

    const auto r = button->subControlContentsRect( contentsRect, Q::Panel );

    LayoutEngine layoutEngine( button );
    layoutEngine.setGeometries( r );

    return layoutEngine.elementAt( 1 )->geometry();
}

QRectF QskPushButtonSkinlet::graphicRect(
    const QskPushButton* button, const QRectF& contentsRect ) const
{
    using Q = QskPushButton;

    const auto r = button->subControlContentsRect( contentsRect, Q::Panel );

    LayoutEngine layoutEngine( button );
    layoutEngine.setGeometries( r );

    return layoutEngine.elementAt( 0 )->geometry();
}

QRectF QskPushButtonSkinlet::rippleRect(
    const QskPushButton* button, const QRectF& contentsRect ) const
{
    using Q = QskPushButton;

    QRectF rect;

    const auto ratio = button->metric( Q::Ripple | QskAspect::Size );
    if ( ratio > 0.0 )
    {
        const auto pos = button->effectiveSkinHint(
            Q::Ripple | QskAspect::Metric | QskAspect::Position ).toPointF();

        const auto panelRect = subControlRect( button, contentsRect, Q::Panel );

        rect.setSize( 2.0 * panelRect.size() * ratio );
        rect.moveCenter( pos );
    }

    return rect;
}

QSGNode* QskPushButtonSkinlet::updateTextNode(
    const QskPushButton* button, QSGNode* node ) const
{
    using Q = QskPushButton;

    const auto rect = button->subControlRect( Q::Text ).toAlignedRect();

    const auto textHeight = button->effectiveFontHeight( Q::Text );
    if ( !button->clip() && ( rect.height() < textHeight ) )
        return nullptr;

    const auto alignment = button->alignmentHint( Q::Text, Qt::AlignCenter );

    return QskSkinlet::updateTextNode( button, node, rect,
        alignment, button->text(), Q::Text );
}

QSGNode* QskPushButtonSkinlet::updateRippleNode(
    const QskPushButton* button, QSGNode* node ) const
{
    using Q = QskPushButton;

    const auto rippleRect = button->subControlRect( Q::Ripple );
    if ( rippleRect.isEmpty() )
        return nullptr;

    auto clipNode = updateBoxClipNode( button, node,
        button->subControlRect( Q::Panel ), Q::Panel );

    if ( clipNode )
    {
        auto boxNode = QskSGNode::findChildNode( clipNode, RippleRole );
        boxNode = updateBoxNode( button, boxNode, rippleRect, Q::Ripple );

        if ( boxNode == nullptr )
            return nullptr;

        QskSGNode::setNodeRole( boxNode, RippleRole );
        if ( boxNode->parent() != clipNode )
            clipNode->appendChildNode( boxNode );
    }

    return clipNode;
}

QSizeF QskPushButtonSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    using Q = QskPushButton;

    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto button = static_cast< const QskPushButton* >( skinnable );

    LayoutEngine layoutEngine( button );

    auto size = layoutEngine.sizeHint( which, QSizeF() );

    size = size.expandedTo( button->strutSizeHint( Q::Panel ) );
    size = button->outerBoxSize( Q::Panel, size );

    return size;
}

#include "moc_QskPushButtonSkinlet.cpp"
