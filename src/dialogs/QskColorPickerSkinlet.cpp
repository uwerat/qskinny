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
            if(  m_image.size() != size )
            {
                m_image = QImage( size.width(), size.height(), QImage::Format_RGB32 );
            }

            QColor color;
            float h, s;
            const float v = *reinterpret_cast< const int* >( nodeData ) / 255.0;

            for( int x = 0; x < m_image.width(); x++ )
            {
                h = ( float ) x / m_image.width();

                for( int y = 0; y < m_image.height(); y++ )
                {
                    s = 1.0 - ( float ) y / m_image.height();
                    color.setHsvF( h, s, v );
                    m_image.setPixel( x, y, color.rgb() );
                }
            }

            p->drawImage( QPointF( 0, 0 ), m_image );
        }

        void updateNode( QQuickWindow* window, const QRectF& rect, int value )
        {
            update( window, rect, QSizeF(), &value );
        }

        QColor selectedColor( const QPointF& p ) const
        {
            return m_image.pixelColor( p.toPoint() );
        }

      protected:
        QskHashValue hash( const void* nodeData ) const override
        {
            const auto* value = reinterpret_cast< const int* >( nodeData );
            return *value;
        }

      private:
        QImage m_image;
    };
}

class QskColorPickerSkinlet::PrivateData
{
  public:
    QColor selectedColor;
    ColorPaneNode* colorPaneNode = nullptr;
};

QskColorPickerSkinlet::QskColorPickerSkinlet( QskSkin* skin )
    : Inherited( skin )
    , m_data( new PrivateData )
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
            auto* n = updateBoxNode( skinnable, node, Q::Selector );
            updateSelectedColor( q );
            return n;
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
        const auto size = q->strutSizeHint( Q::Selector );
        const auto x = q->positionHint( Q::Selector | QskAspect::Horizontal );
        const auto y = q->positionHint( Q::Selector | QskAspect::Vertical );

        QRectF r( { x - size.width() / 2.0, y - size.height() / 2.0 }, size );
        return r;
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QColor QskColorPickerSkinlet::selectedColor() const
{
    return m_data->selectedColor;
}

QSGNode* QskColorPickerSkinlet::updateColorPaneNode(
    const QskColorPicker* q, QSGNode* node ) const
{
    m_data->colorPaneNode = QskSGNode::ensureNode< ColorPaneNode >( node );

    const auto rect = q->subControlRect( Q::ColorPane );
    m_data->colorPaneNode->updateNode( q->window(), rect, q->value() );

    updateSelectedColor( q );

    return m_data->colorPaneNode;
}

QPointF QskColorPickerSkinlet::selectorPos( const Q* q ) const
{
    const auto x = q->positionHint( Q::Selector | QskAspect::Horizontal );
    const auto y = q->positionHint( Q::Selector | QskAspect::Vertical );

    QPointF p( x, y );

    const auto rect = q->subControlRect( Q::ColorPane );

    p.rx() = qBound( rect.x(), p.x(), rect.right() );
    p.ry() = qBound( rect.y(), p.y(), rect.bottom() );

    return p;
}

void QskColorPickerSkinlet::updateSelectedColor( const Q* q ) const
{
    const auto color = m_data->colorPaneNode->selectedColor( selectorPos( q ) );
    m_data->selectedColor = color;

    auto* e = new QEvent( static_cast< QEvent::Type >( q->colorChangedEventType() ) );
    QCoreApplication::postEvent( const_cast< Q* >( q ), e );
}

#include "moc_QskColorPickerSkinlet.cpp"
