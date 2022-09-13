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
            setSpacing( button->spacingHint( QskPushButton::Panel ) );

            setGraphicTextElements( button,
                QskPushButton::Text, button->text(),
                QskPushButton::Graphic, button->graphic().defaultSize() );
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
    using Q = QskPushButton;

    const auto button = static_cast< const QskPushButton* >( skinnable );

    if ( ( subControl == Q::Text ) || ( subControl == Q::Graphic ) )
    {
        const auto r = button->subControlContentsRect( contentsRect, Q::Panel );

        LayoutEngine layoutEngine( button );
        layoutEngine.setGeometries( r );

        return layoutEngine.subControlRect( subControl );
    }

    if ( subControl == Q::Panel )
    {
        return contentsRect;
    }

    if ( subControl == Q::Ripple )
    {
        return rippleRect( button, contentsRect );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskPushButtonSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    using Q = QskPushButton;

    const auto button = static_cast< const QskPushButton* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            return updateBoxNode( button, node, Q::Panel );
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
            return updateGraphicNode( button, node, button->graphic(), Q::Graphic );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
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
