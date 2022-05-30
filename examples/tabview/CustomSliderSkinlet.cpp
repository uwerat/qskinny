/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "CustomSliderSkinlet.h"
#include "CustomSlider.h"

#include <QskAspect.h>
#include <QskRgbValue.h>
#include <QskSlider.h>
#include <QskTextColors.h>
#include <QskTextOptions.h>
#include <QskSGNode.h>
#include <QskFunctions.h>

#include <QFontMetricsF>
#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>

#include <cmath>

#if 1
// should be skin hints
static const qreal qskMinorTick = 20;
static const qreal qskMajorTick = 1.5 * qskMinorTick;
static const qreal qskMargin = 20;
static const qreal qskPeak = 10;

static QFont qskLabelFont;

#endif

namespace
{
    class TicksNode : public QSGGeometryNode
    {
      public:
        TicksNode( const QColor& color )
            : m_geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
        {
            m_geometry.setDrawingMode( QSGGeometry::DrawLines );
            m_geometry.setVertexDataPattern( QSGGeometry::StaticPattern );

            m_material.setColor( color );

            setGeometry( &m_geometry );
            setMaterial( &m_material );
        }

      private:
        QSGFlatColorMaterial m_material;
        QSGGeometry m_geometry;
    };

    class HandleNode : public QSGGeometryNode
    {
      public:
        HandleNode()
            : m_geometry( QSGGeometry::defaultAttributes_Point2D(), 8 * 2 )
        {
            setGeometry( &m_geometry );
            setMaterial( &m_material );
        }

        void update( const QRectF& rect, qreal peakPos, const QColor& color )
        {
            if ( color != m_color )
            {
                m_material.setColor( color );

                m_color = color;
                markDirty( QSGNode::DirtyMaterial );
            }

            if ( rect != m_rect || peakPos != m_peakPos )
            {
                auto p = reinterpret_cast< QSGGeometry::Point2D* >( m_geometry.vertexData() );

                const qreal y0 = rect.y() + qskPeak;

                setLine( p, peakPos, peakPos, rect.y() );
                setLine( p + 2, peakPos - 5, peakPos + 5, y0 );

                // corners manually "rounded" by 3 pixels

                setLine( p + 4, rect.left() + 2, rect.right() - 2, y0 );
                setLine( p + 6, rect.left() + 1, rect.right() - 1, y0 + 1 );
                setLine( p + 8, rect.left(), rect.right(), y0 + 2 );
                setLine( p + 10, rect.left(), rect.right(), rect.bottom() - 1 );
                setLine( p + 12, rect.left() + 1, rect.right() - 1, rect.bottom() - 1 );
                setLine( p + 14, rect.left() + 2, rect.right() - 2, rect.bottom() );

                m_rect = rect;
                m_peakPos = peakPos;
                markDirty( QSGNode::DirtyGeometry );
            }
        }

      private:
        inline void setLine( QSGGeometry::Point2D* points, float x1, float x2, qreal y )
        {
            points[ 0 ].x = x1;
            points[ 0 ].y = y;

            points[ 1 ].x = x2;
            points[ 1 ].y = y;
        }

        QRectF m_rect;
        qreal m_peakPos;
        QColor m_color;

        QSGFlatColorMaterial m_material;
        QSGGeometry m_geometry;
    };
}

CustomSliderSkinlet::CustomSliderSkinlet()
{
    qskLabelFont = QFont();
    setNodeRoles( { ScaleRole, FillRole, DecorationRole, HandleRole } );
}

QRectF CustomSliderSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    const auto slider = static_cast< const QskSlider* >( skinnable );

    if ( subControl == QskSlider::Groove )
    {
        return QRectF(); // we don't have a groove
    }
    else if ( subControl == QskSlider::Fill )
    {
        return fillRect( slider, contentsRect );
    }
    else if ( subControl == QskSlider::Handle )
    {
        return handleRect( slider, contentsRect );
    }
    else if ( subControl == CustomSlider::Scale )
    {
        return scaleRect( contentsRect );
    }
    else if ( subControl == CustomSlider::Decoration )
    {
        return decorationRect( slider, contentsRect );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* CustomSliderSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto slider = static_cast< const QskSlider* >( skinnable );

    switch ( nodeRole )
    {
        case ScaleRole:
            return updateScaleNode( slider, node );

        case DecorationRole:
            return updateDecorationNode( slider, node );

        case FillRole:
            return Inherited::updateSubNode( skinnable, nodeRole, node );

        case HandleRole:
            return updateHandleNode( slider, node );

        default:
            return nullptr;
    }
}

QRectF CustomSliderSkinlet::scaleRect( const QRectF& contentsRect ) const
{
    auto r = contentsRect;

    r.setX( r.left() + qskMargin );
    r.setBottom( r.center().y() );
    r.setTop( r.bottom() - qskMajorTick );
    r.setWidth( r.width() - qskMargin );

    return r;
}

QRectF CustomSliderSkinlet::fillRect(
    const QskSlider* slider, const QRectF& contentsRect ) const
{
    auto r = subControlRect( slider, contentsRect, CustomSlider::Scale );

    r.setTop( r.bottom() - qskMinorTick );
    r.setWidth( r.width() * slider->valueAsRatio() );

    return r;
}

QRectF CustomSliderSkinlet::decorationRect(
    const QskSlider* slider, const QRectF& contentsRect ) const
{
    // decoration exceeds scale !!!!

    auto r = subControlRect( slider, contentsRect, CustomSlider::Scale );
    r.setBottom( r.top() );
    r.setTop( r.bottom() - QFontMetricsF( qskLabelFont ).height() );

    return r;
}

QRectF CustomSliderSkinlet::handleRect(
    const QskSlider* slider, const QRectF& contentsRect ) const
{
    const QRectF fillRect = subControlRect( slider, contentsRect, QskSlider::Fill );
    const QRectF scaleRect = subControlRect( slider, contentsRect, CustomSlider::Scale );

    QRectF handleRect( 0, scaleRect.bottom(), 80, 50 );
    handleRect.moveCenter( QPointF( fillRect.right(), handleRect.center().y() ) );

    if ( handleRect.left() < contentsRect.left() )
    {
        handleRect.moveLeft( contentsRect.left() );
    }
    else if ( handleRect.right() > contentsRect.right() )
    {
        handleRect.moveRight( contentsRect.right() );
    }

    return handleRect;
}

QSGNode* CustomSliderSkinlet::updateScaleNode(
    const QskSlider* slider, QSGNode* node ) const
{
    const auto scaleRect = subControlRect(
        slider, slider->contentsRect(), CustomSlider::Scale );

    if ( scaleRect.isEmpty() )
        return nullptr;

    auto ticksNode = static_cast< TicksNode* >( node );
    if ( ticksNode == nullptr )
        ticksNode = new TicksNode( slider->color( CustomSlider::Scale ) );

    const int tickCount = std::floor( slider->boundaryLength() / slider->stepSize() ) + 1;

    auto geometry = ticksNode->geometry();
    geometry->allocate( tickCount * 2 );

    auto vertexData = geometry->vertexDataAsPoint2D();
    memset( vertexData, 0, geometry->vertexCount() );

    auto stepStride = slider->stepSize() / slider->boundaryLength() * scaleRect.width();

    auto x = scaleRect.x();
    const auto y = scaleRect.bottom();

    // Create a series of tickmarks from minimum to maximum
    for ( int i = 0; i < tickCount; ++i )
    {
        vertexData[ 0 ].set( x, y );
        vertexData[ 1 ].set( x, y - ( ( i % 10 ) ? qskMinorTick : qskMajorTick ) );

        vertexData += 2;
        x += stepStride;
    }
    geometry->setLineWidth( 1 );
    geometry->markVertexDataDirty();

    ticksNode->markDirty( QSGNode::DirtyGeometry );

    return ticksNode;
}

QSGNode* CustomSliderSkinlet::updateDecorationNode(
    const QskSlider* slider, QSGNode* node ) const
{
    const QRectF decorationRect = subControlRect(
        slider, slider->contentsRect(), CustomSlider::Decoration );

    if ( decorationRect.isEmpty() )
        return nullptr;

    auto decorationNode = static_cast< QSGTransformNode* >( node );
    if ( decorationNode == nullptr )
        decorationNode = new QSGTransformNode();

    const int tickCount = std::floor( slider->boundaryLength() / slider->stepSize() ) + 1;

    auto stepStride = slider->stepSize() / slider->boundaryLength() * decorationRect.width();

    auto x = decorationRect.x();
    const auto y = decorationRect.y();

    auto labelNode = decorationNode->firstChild();

    const QFontMetrics fm( qskLabelFont );

    for ( int i = 0; i < tickCount; i += 100 )
    {
        const auto text = QString::number( slider->minimum() + slider->stepSize() * i, 'f', 0 );

        const auto w = qskHorizontalAdvance( fm, text );
        const auto h = fm.height();

        labelNode = QskSkinlet::updateTextNode( slider, labelNode,
            QRectF( x - 0.5 * w, y, w, h ), Qt::AlignHCenter, text, qskLabelFont,
            QskTextOptions(), QskTextColors( QskRgb::Grey700 ), Qsk::Normal );

        if ( labelNode )
        {
            if ( labelNode->parent() != decorationNode )
                decorationNode->appendChildNode( labelNode );

            labelNode = labelNode->nextSibling();
        }

        x += 100 * stepStride;
    }

    QskSGNode::removeAllChildNodesAfter( decorationNode, labelNode );
    return decorationNode;
}

QSGNode* CustomSliderSkinlet::updateHandleNode(
    const QskSlider* slider, QSGNode* node ) const
{
    const auto cr = slider->contentsRect();

    const auto handleRect = subControlRect( slider, cr, QskSlider::Handle );

    if ( handleRect.isEmpty() )
        return nullptr;

    const auto fillRect = subControlRect( slider, cr, QskSlider::Fill );

    auto handleNode = static_cast< HandleNode* >( node );
    if ( handleNode == nullptr )
        handleNode = new HandleNode();

    const auto handleColor = slider->color( QskSlider::Handle );
    handleNode->update( handleRect, fillRect.right(), handleColor );

    // finally the value label

    QFont font( QStringLiteral( "Roboto" ) );
    font.setPixelSize( 26 );

    const qreal h = QFontMetrics( font ).height();

    auto textRect = handleRect;
    textRect.setTop( textRect.bottom() - 0.5 * ( textRect.height() - qskPeak + h ) );

    const auto text = QString::number( slider->value(), 'f', 0 );

    auto labelNode = QskSkinlet::updateTextNode( slider, handleNode->firstChild(),
        textRect, Qt::AlignHCenter, text, font, QskTextOptions(),
        QskTextColors( Qt::white ), Qsk::Normal );

    if ( labelNode->parent() != handleNode )
        handleNode->appendChildNode( labelNode );

    return handleNode;
}

QSizeF CustomSliderSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    auto size = Inherited::sizeHint( skinnable, which, constraint );

    if ( which == Qt::PreferredSize && size.height() >= 0 )
        size.rheight() += 60;

    return size;
}
