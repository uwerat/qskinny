/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SliderSkinlet.h"
#include "Slider.h"

#include <QskAspect.h>
#include <QskSlider.h>
#include <QskRgbValue.h>
#include <QskTextNode.h>
#include <QskTextOptions.h>
#include <QskTextColors.h>

#include <QSGFlatColorMaterial>
#include <QFontMetricsF>

#include <cmath>

#if 1
// should be skin hints
const qreal qskMinorTick = 20;
const qreal qskMajorTick = 1.5 * qskMinorTick;
const qreal qskMargin = 20;
const qreal qskPeak = 10;

QFont qskLabelFont;
#endif

class TicksNode : public QSGGeometryNode
{
public:
    TicksNode( const QColor& color ):
        m_geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
    {
        m_geometry.setDrawingMode( GL_LINES );
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
    HandleNode():
        m_geometry( QSGGeometry::defaultAttributes_Point2D(), 8 * 2 )
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
            QSGGeometry::Point2D* p =
                reinterpret_cast< QSGGeometry::Point2D* >( m_geometry.vertexData() );

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
        points[0].x = x1;
        points[0].y = y;

        points[1].x = x2;
        points[1].y = y;
    }

    QRectF m_rect;
    qreal m_peakPos;
    QColor m_color;

    QSGFlatColorMaterial m_material;
    QSGGeometry m_geometry;
};

SliderSkinlet::SliderSkinlet()
{
    qskLabelFont = QFont();
    setNodeRoles( { ScaleRole, FillRole, DecorationRole, HandleRole } );
}

SliderSkinlet::~SliderSkinlet()
{
}

QRectF SliderSkinlet::subControlRect( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl ) const
{
    const auto slider = static_cast< const QskSlider* >( skinnable );

    if ( subControl == QskSlider::Groove )
    {
        return QRectF(); // we don't have a groove
    }
    else if ( subControl == QskSlider::Fill )
    {
        return fillRect( slider );
    }
    else if ( subControl == QskSlider::Handle )
    {
        return handleRect( slider );
    }
    else if ( subControl == Slider::Scale )
    {
        return scaleRect( slider );
    }
    else if ( subControl == Slider::Decoration )
    {
        return decorationRect( slider );
    }

    return Inherited::subControlRect( skinnable, subControl );
}

QSGNode* SliderSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto slider = static_cast< const QskSlider* >( skinnable );

    switch( nodeRole )
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

QRectF SliderSkinlet::scaleRect( const QskSlider* slider ) const
{
    auto r = slider->contentsRect();

    r.setX( r.left() + qskMargin );
    r.setBottom( r.center().y() );
    r.setTop( r.bottom() - qskMajorTick );
    r.setWidth( r.width() - qskMargin );

    return r;
}

QRectF SliderSkinlet::fillRect( const QskSlider* slider ) const
{
    auto r = subControlRect( slider, Slider::Scale );

    r.setTop( r.bottom() - qskMinorTick );
    r.setWidth( r.width() * slider->position() );

    return r;
}

QRectF SliderSkinlet::decorationRect( const QskSlider* slider ) const
{
    // decoration exceeds scale !!!!

    auto r = subControlRect( slider, Slider::Scale );
    r.setBottom( r.top() );
    r.setTop( r.bottom() - QFontMetricsF( qskLabelFont ).height() );

    return r;
}

QRectF SliderSkinlet::handleRect( const QskSlider* slider ) const
{
    const QRectF contentsRect = slider->contentsRect();
    const QRectF fillRect = subControlRect( slider, QskSlider::Fill );
    const QRectF scaleRect = subControlRect( slider, Slider::Scale );

    QRectF handleRect( 0, scaleRect.bottom(), 80, 50 );
    handleRect.moveCenter( QPointF( fillRect.right(), handleRect.center().y() ) );

    if ( handleRect.left() < contentsRect.left() )
        handleRect.moveLeft( contentsRect.left() );
    else if ( handleRect.right() > contentsRect.right() )
        handleRect.moveRight( contentsRect.right() );

    return handleRect;
}

QSGNode* SliderSkinlet::updateScaleNode(
    const QskSlider* slider, QSGNode* node ) const
{
    const QRectF scaleRect = subControlRect( slider, Slider::Scale );
    if ( scaleRect.isEmpty() )
        return nullptr;

    auto ticksNode = static_cast< TicksNode* >( node );
    if ( ticksNode == nullptr )
        ticksNode = new TicksNode( slider->color( Slider::Scale ) );

    const int tickCount = std::floor( slider->range() / slider->stepSize() ) + 1;

    auto geometry = ticksNode->geometry();
    geometry->allocate( tickCount * 2 );

    auto vertexData = geometry->vertexDataAsPoint2D();
    memset( vertexData, 0, geometry->vertexCount() );

    auto stepStride = slider->stepSize() / slider->range() * scaleRect.width();

    auto x = scaleRect.x();
    const auto y = scaleRect.bottom();

    // Create a series of tickmarks from minimum to maximum
    for ( int i = 0; i < tickCount; ++i )
    {
        vertexData[0].set( x, y );
        vertexData[1].set( x, y - ( ( i % 10 ) ? qskMinorTick : qskMajorTick ) );

        vertexData += 2;
        x += stepStride;
    }
    geometry->setLineWidth( 1 );
    geometry->markVertexDataDirty();

    ticksNode->markDirty( QSGNode::DirtyGeometry );

    return ticksNode;
}

QSGNode* SliderSkinlet::updateDecorationNode(
    const QskSlider* slider, QSGNode* node ) const
{
    const QRectF decorationRect = subControlRect( slider, Slider::Decoration );
    if ( decorationRect.isEmpty() )
        return nullptr;

    auto decorationNode = static_cast< QSGTransformNode* >( node );
    if ( decorationNode == nullptr )
        decorationNode = new QSGTransformNode();

    const int tickCount = std::floor( slider->range() / slider->stepSize() ) + 1;

    auto labelNode = static_cast< QskTextNode* >( decorationNode->firstChild() );

    auto stepStride = slider->stepSize() / slider->range() * decorationRect.width();

    auto x = decorationRect.x();
    const auto y = decorationRect.y();

    for ( int i = 0; i < tickCount; i += 100 )
    {
        if ( labelNode == nullptr )
        {
            labelNode = new QskTextNode;
            decorationNode->appendChildNode( labelNode );
        }

        auto labelText = QString::number( slider->minimum() + slider->stepSize() * i, 'f', 0 );

        labelNode->setTextData( slider, labelText, QRectF( x, y, 0, 0 ),
            qskLabelFont, QskTextOptions(), QskTextColors( QskRgbValue::Grey700 ),
            Qt::AlignHCenter, Qsk::Normal );

        labelNode = static_cast< decltype( labelNode ) >( labelNode->nextSibling() );
        x += 100 * stepStride;
    }

    // Remove unused labels
    while ( labelNode )
    {
        auto sibling = static_cast< decltype( labelNode ) >( labelNode->nextSibling() );
        decorationNode->removeChildNode( labelNode );
        delete labelNode;
        labelNode = sibling;
    }

    return decorationNode;
}

QSGNode* SliderSkinlet::updateHandleNode(
    const QskSlider* slider, QSGNode* node ) const
{
    const QRectF handleRect = subControlRect( slider, QskSlider::Handle );
    if ( handleRect.isEmpty() )
        return nullptr;

    const QRectF fillRect = subControlRect( slider, QskSlider::Fill );

    auto handleNode = static_cast< HandleNode* >( node );
    if ( handleNode == nullptr )
        handleNode = new HandleNode();

    handleNode->update( handleRect, fillRect.right(),
        slider->color( QskSlider::Handle ) );

    // finally the value label
    auto labelNode = static_cast< QskTextNode* >( handleNode->firstChild() );
    if ( labelNode == nullptr  )
    {
        labelNode = new QskTextNode;
        handleNode->appendChildNode( labelNode );
    }

    QFont font( QStringLiteral( "Roboto" ) );
    font.setPixelSize( 26 );

    const qreal h = QFontMetrics( font ).height();

    auto textRect = handleRect;
    textRect.setTop( textRect.bottom() - 0.5 * ( textRect.height() - qskPeak + h ) );

    const QString text = QString::number( slider->value(), 'f', 0 );

    labelNode->setTextData( slider, text, textRect,
        font, QskTextOptions(), QskTextColors( Qt::white ),
        Qt::AlignHCenter, Qsk::Normal );

    return handleNode;
}
