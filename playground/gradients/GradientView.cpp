/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "GradientView.h"

#ifdef SHAPE_GRADIENT
    #include "GradientQuickShape.h"
#endif

#include <QskPaintedNode.h>
#include <QskRectangleNode.h>
#include <QskBoxFillNode.h>
#include <QskGradient.h>
#include <QskGradientDirection.h>

#include <QBrush>
#include <QPainter>

namespace
{
    template< typename Node >
    inline Node* gradientNode( QSGNode* node )
    {
        if ( node == nullptr )
            node = new Node();

        return static_cast< Node* >( node );
    }

    class PaintedNode final : public QskPaintedNode
    {
      public:
        void updateNode( QQuickWindow* window,
            const QRectF& rect, const QskGradient& gradient )
        {
            update( window, rect, QSizeF(), &gradient );
        }

        QskHashValue hash( const void* nodeData ) const override
        {
            const auto gradient = reinterpret_cast< const QskGradient* >( nodeData );
            return gradient->hash();
        }

      protected:
        void paint( QPainter* painter, const QSize& size, const void* nodeData ) override
        {
            const auto gradient = reinterpret_cast< const QskGradient* >( nodeData );
            const QRect rect( 0, 0, size.width(), size.height() );

            painter->fillRect( rect, gradient->toQGradient( rect ) );
        }
    };
}

GradientView::GradientView( NodeType nodeType, QQuickItem* parent )
    : QQuickItem( parent )
    , m_nodeType( nodeType )
{
    setFlag( QQuickItem::ItemHasContents, true );
}

GradientView::NodeType GradientView::GradientView::nodeType() const
{
    return m_nodeType;
}

void GradientView::setGradient( const QskGradient& gradient )
{
    m_gradient = gradient;
    update();
}

QskGradient GradientView::gradient() const
{
    return m_gradient;
}

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
void GradientView::geometryChange( const QRectF&, const QRectF& )
#else
void GradientView::geometryChanged( const QRectF&, const QRectF& )
#endif
{
    update();
}

QSGNode* GradientView::updatePaintNode(
    QSGNode* oldNode, QQuickItem::UpdatePaintNodeData* )
{
    const QRectF rect( 0, 0, width(), height() );

    switch( m_nodeType )
    {
        case Painted:
        {
            auto node = gradientNode< PaintedNode >( oldNode );
            node->updateNode( window(), rect, m_gradient );

            return node;
        }
        case Rectangle:
        {
            auto node = gradientNode< QskRectangleNode >( oldNode );
            node->updateNode( rect, m_gradient );

            return node;
        }
        case BoxFill:
        {
            auto node = gradientNode< QskBoxFillNode >( oldNode );
            node->updateNode( rect, m_gradient );

            return node;
        }
#ifdef SHAPE_GRADIENT
        case Shape:
        {
            return GradientQuickShape::updateNode(
                window(), rect, m_gradient, oldNode );
        }
#endif
        default:
            break;
    }

    return nullptr;
}

#include "moc_GradientView.cpp"
