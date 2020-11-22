/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Frame.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxNode.h>
#include <QskBoxShapeMetrics.h>
#include <QskGradient.h>
#include <QskSGNode.h>

static inline qreal effectiveRadius( const QRectF& rect, qreal percentage )
{
    return percentage / 100.0 * 0.5 * qMin( rect.width(), rect.height() );
}

Frame::Frame( QQuickItem* parent )
    : Inherited( parent )
    , m_style( Frame::Plain )
    , m_color( Qt::gray )
    , m_frameWidth( 1.0 )
    , m_radius( 0.0 )
{
}

Frame::~Frame()
{
}

void Frame::setStyle( Style style )
{
    if ( m_style != style )
    {
        m_style = style;
        update();

        Q_EMIT styleChanged();
    }
}

Frame::Style Frame::style() const
{
    return m_style;
}

void Frame::setColor( const QColor& color )
{
    if ( m_color != color )
    {
        m_color = color;
        Q_EMIT colorChanged();
    }
}

QColor Frame::color() const
{
    return m_color;
}

void Frame::setFrameWidth( qreal width )
{
    width = qMax( width, 0.0 );

    if ( m_frameWidth != width )
    {
        m_frameWidth = width;
        Q_EMIT frameWidthChanged();
    }
}

qreal Frame::frameWidth() const
{
    return m_frameWidth;
}

void Frame::setRadius( qreal radius )
{
    radius = qBound( 0.0, radius, 100.0 );
    if ( radius != m_radius )
    {
        m_radius = radius;

        update();
        Q_EMIT radiusChanged();
    }
}

qreal Frame::radius() const
{
    return m_radius;
}

void Frame::updateNode( QSGNode* parentNode )
{
    // boilerplate code, that is not needed, when using a skinlet.

    const quint8 nodeRole = 0;

    auto node = static_cast< QskBoxNode* >(
        QskSGNode::findChildNode( parentNode, nodeRole ) );

    const QRectF rect = contentsRect();
    if ( rect.isEmpty() )
    {
        delete node;
        return;
    }

    if ( node == nullptr )
    {
        node = new QskBoxNode;
        QskSGNode::setNodeRole( node, nodeRole );
    }

    updateFrameNode( rect, node );

    if ( node->parent() != parentNode )
        parentNode->appendChildNode( node );
}

void Frame::updateFrameNode( const QRectF& rect, QskBoxNode* node )
{
    const QColor dark = m_color.darker( 150 );
    const QColor light = m_color.lighter( 150 );

    QColor c1, c2;

    switch ( m_style )
    {
        case Frame::Sunken:
        {
            c1 = dark;
            c2 = light;
            break;
        }
        case Frame::Raised:
        {
            c1 = light;
            c2 = dark;
            break;
        }
        default:
        {
            c1 = c2 = dark;
        }
    }

    const QskBoxBorderColors borderColors( c1, c1, c2, c2 );
    const qreal radius = effectiveRadius( rect, m_radius );

    node->setBoxData( rect, radius, m_frameWidth, borderColors, m_color );
}

#include "moc_Frame.cpp"
