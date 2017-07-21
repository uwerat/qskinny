/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "Frame.h"

#include <QskAspect.h>
#include <QskFrameNode.h>
#include <QskSkinlet.h>

static inline qreal effectiveRadius( const QRectF& rect, qreal percentage )
{
    return percentage / 100.0 * 0.5 * qMin( rect.width(), rect.height() );
}

static inline qreal shadeFactor( Frame::Style style )
{
    switch( style )
    {
        case Frame::Raised:
            return 1.0;

        case Frame::Sunken:
            return -1.0;

        default:
            return 0.0;
    }
}

Frame::Frame( QQuickItem* parent ):
    Inherited( parent ),
    m_style( Frame::Plain ),
    m_color( Qt::gray ),
    m_frameWidth( 1.0 ),
    m_radius( 0.0 )
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

    auto node = static_cast< QskFrameNode* >(
        QskSkinlet::findNodeByRole( parentNode, nodeRole ) );

    const QRectF rect = contentsRect();
    if ( rect.isEmpty() )
    {
        delete node;
    }
    else
    {
        if ( node == nullptr )
        {
            node = new QskFrameNode;
            QskSkinlet::setNodeRole( node, nodeRole );

            parentNode->appendChildNode( node );
        }

        updateFrameNode( rect, node );
    }
}

void Frame::updateFrameNode( const QRectF& rect, QskFrameNode* node )
{
    node->setRect( rect );
    node->setRadius( effectiveRadius( rect, m_radius ) );
    node->setBorderWidth( m_frameWidth );

    const QColor dark = m_color.darker( 150 );
    const QColor light = m_color.lighter( 150 );

    const qreal sf = shadeFactor( m_style );
    if ( sf == 0.0 )
        node->setColors( dark, m_color, dark );
    else
        node->setColors( dark, m_color, light );
    
    node->setShadeFactor( sf );

    node->update();
}

#include "moc_Frame.cpp"
