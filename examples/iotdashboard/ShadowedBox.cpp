/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "ShadowedBox.h"


#include <QskBoxNode.h>
#include <QskBoxShadowNode.h>
#include <QskSGNode.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxBorderColors.h>
#include <QskGradient.h>
#include <QskSkinlet.h>

namespace
{
    class Skinlet : public QskSkinlet
    {
      public:
        enum NodeRole { ShadowRole, PanelRole };

        Skinlet()
        {
            setOwnedBySkinnable( true );
            setNodeRoles( { ShadowRole, PanelRole } );
        }

        QRectF subControlRect( const QskSkinnable*,
            const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const override
        {
            if ( subControl == ShadowedBox::Panel )
            {
                return contentsRect;
            }

            return QRectF();
        }

        QSGNode* updateSubNode( const QskSkinnable* skinnable,
            quint8 nodeRole, QSGNode* node ) const override
        {
            const auto box = static_cast< const ShadowedBox* >( skinnable );

            const auto r = box->subControlRect( ShadowedBox::Panel );
            if ( r.isEmpty() )
                return nullptr;

            switch ( nodeRole )
            {
                case ShadowRole:
                {
                    const auto& shadowMetrics = box->shadow();

                    auto shadowNode = QskSGNode::ensureNode< QskBoxShadowNode >( node );

                    shadowNode->setRect( shadowMetrics.shadowRect( r ) );
                    shadowNode->setShape( box->shape() );
                    shadowNode->setBlurRadius( shadowMetrics.blurRadius() );
                    shadowNode->setColor( box->shadowColor() );
                    shadowNode->setClipRect( r );

                    shadowNode->updateGeometry();

                    return shadowNode;
                }
                case PanelRole:
                {
                    const auto r = box->subControlRect( ShadowedBox::Panel );

                    auto boxNode = QskSGNode::ensureNode< QskBoxNode >( node );
                    boxNode->setBoxData( r, box->shape(), box->borderWidth(),
                        box->borderColor(), box->gradient() );

                    return boxNode;
                }
            }

            return nullptr;
        }
    };
}

QSK_SUBCONTROL( ShadowedBox, Panel )

ShadowedBox::ShadowedBox(Qt::Orientation orientation, QQuickItem* parentItem )
    : QskLinearBox( orientation, parentItem )
{
    setFlag( QQuickItem::ItemHasContents, true );
    setSkinlet( new Skinlet() );

    // ### move to Skin:
    setGradient( Qt::white );
    setShadow( { 0, 10 } );
    setShadowColor( 0xe5e5e5 );
    setShape( 6 );
}

ShadowedBox::~ShadowedBox()
{
}

void ShadowedBox::setShadow( const QskShadowMetrics& shadow )
{
    m_shadow = shadow;
    update();
}

const QskShadowMetrics& ShadowedBox::shadow() const
{
    return m_shadow;
}

void ShadowedBox::setShadowColor( const QColor& color )
{
    m_shadowColor = color;
    update();
}

QColor ShadowedBox::shadowColor() const
{
    return m_shadowColor;
}

QRectF ShadowedBox::layoutRectForSize( const QSizeF& size ) const
{
    auto padding = paddingHint( Panel );
    return { padding.left() / 2, padding.top() / 2,
        size.width() - padding.right(), size.height() - padding.bottom() };
}

void ShadowedBox::setGradient( const QskGradient& gradient )
{
    m_gradient = gradient;
    update();
}

const QskGradient& ShadowedBox::gradient() const
{
    return m_gradient;
}

void ShadowedBox::setShape( const QskBoxShapeMetrics& shape )
{
    m_shape = shape;
    update();
}

const QskBoxShapeMetrics& ShadowedBox::shape() const
{
    return m_shape;
}

void ShadowedBox::setBorderWidth( qreal width )
{
    m_borderWidth = qMax( width, 0.0 );
    update();
}

qreal ShadowedBox::borderWidth() const
{
    return m_borderWidth;
}

void ShadowedBox::setBorderColor( const QColor& color )
{
    m_borderColor = color;
    update();
}

QColor ShadowedBox::borderColor() const
{
    return m_borderColor;
}

#include "moc_ShadowedBox.cpp"
