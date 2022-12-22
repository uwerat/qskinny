/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "GradientView.h"

#ifdef SHAPE_GRADIENT
    #include "GradientQuickShape.h"
#endif

#include <QskPaintedNode.h>
#include <QskBoxFillNode.h>
#include <QskBoxRectangleNode.h>
#include <QskGradient.h>
#include <QskGradientDirection.h>
#include <QskBoxRenderer.h>
#include <QskBoxShapeMetrics.h>
#include <QskTextLabel.h>

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
        PaintedNode()
        {
            setRenderHint( QskPaintedNode::Raster );
        }

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

            painter->fillRect( rect, gradient->stretchedTo( rect ).toQGradient() );
        }
    };

    class InfoLabel : public QskTextLabel
    {
      public:
        InfoLabel( GradientView::NodeType nodeType, QQuickItem* parent )
            : QskTextLabel( parent )
        {
            QString text;

            switch( nodeType )
            {
                case GradientView::Painted:
                    text = "QskPaintedNode";
                    break;

                case GradientView::BoxRectangle:
                    text = "QskBoxRectangleNode";
                    break;

                case GradientView::BoxFill:
                    text = "QskBoxFillNode";
                    break;

        #ifdef SHAPE_GRADIENT
                case GradientView::Shape:
                    text = "QQuickShapeGenericNode";
                    break;
        #endif

                default:
                    break;
            }

            if ( !text.isEmpty() )
            {
                QColor c( Qt::white );
                c.setAlpha( 200 );
                setBackgroundColor( c );

                setText( text );
            }
        }
    };
}

GradientView::GradientView( NodeType nodeType, QQuickItem* parent )
    : QQuickItem( parent )
    , m_nodeType( nodeType )
{
    setFlag( QQuickItem::ItemHasContents, true );

    auto label = new InfoLabel( nodeType, this );
    label->setPosition( 10, 10 );
    label->setSize( label->sizeHint() );
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
        case BoxFill:
        {
            auto node = gradientNode< QskBoxFillNode >( oldNode );
            node->updateNode( rect, m_gradient );

            return node;
        }
        case BoxRectangle:
        {
            if ( !QskBoxRenderer::isGradientSupported(
                QskBoxShapeMetrics(), m_gradient ) )
            {
                delete oldNode;
                return nullptr;
            }

            auto node = gradientNode< QskBoxRectangleNode >( oldNode );
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
