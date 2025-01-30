/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskColorPickerSkinlet.h"
#include "QskColorPicker.h"

#include "QskSGNode.h"
#include "QskPaintedNode.h"

#include <QPainter>

using Q = QskColorPicker;

namespace
{
    class ColorPaneNode : public QskPaintedNode
    {
      public:
        ColorPaneNode()
        {
        }

        void paint( QPainter* p, const QSize& size, const void* nodeData ) override
        {
            const Q* q = static_cast< const Q* >( nodeData );
            const auto image = q->image().scaled( size );
            p->drawImage( QPointF( 0, 0 ), image );
        }

        void updateNode( QQuickWindow* window, const QRectF& rect, const Q* q )
        {
            update( window, rect, QSizeF(), q );
        }

      protected:
        QskHashValue hash( const void* nodeData ) const override
        {
            const Q* q = static_cast< const Q* >( nodeData );
            const auto r = q->subControlRect( Q::ColorPane );

            QskHashValue h = qHash( r.width() );
            h = qHash( r.height() );
            h = qHash( q->value() );

            return h;
        }
    };
}

QskColorPickerSkinlet::QskColorPickerSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { PanelRole, ColorPaneRole, SelectorRole } );
}

QskColorPickerSkinlet::~QskColorPickerSkinlet() = default;

QSGNode* QskColorPickerSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto q = static_cast< const Q* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
        {
            return updateBoxNode( skinnable, node, Q::Panel );
        }
        case ColorPaneRole:
        {
            return updateColorPaneNode( q, node );
        }
        case SelectorRole:
        {
            return updateBoxNode( skinnable, node, Q::Selector );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QRectF QskColorPickerSkinlet::subControlRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl ) const
{
    const auto q = static_cast< const Q* >( skinnable );

    if( subControl == Q::Panel || subControl == Q::ColorPane )
    {
        return contentsRect;
    }

    if( subControl == Q::Selector )
    {
        const auto s = q->strutSizeHint( Q::Selector );
        const auto p = q->position();

        const QRectF r( { p.x() - s.width() / 2.0, p.y() - s.height() / 2.0 }, s );
        return r;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskColorPickerSkinlet::updateColorPaneNode(
    const QskColorPicker* q, QSGNode* node ) const
{
    auto* colorPaneNode = QskSGNode::ensureNode< ColorPaneNode >( node );

    const auto rect = q->subControlRect( Q::ColorPane );

    colorPaneNode->updateNode( q->window(), rect, q );

    return colorPaneNode;
}

#include "moc_QskColorPickerSkinlet.cpp"
