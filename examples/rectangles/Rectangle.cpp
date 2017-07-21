/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "Rectangle.h"
#include "Border.h"

#include <QskSkinlet.h>
#include <QskCorner.h>
#include <QskRectNode.h>

class Rectangle::PrivateData
{
public:
    PrivateData():
        border( 0.0, QColor() )
    {
    }

    QskCorner corner;
    Border border;
    QColor fillColor;
    QskGradient gradient;
};

Rectangle::Rectangle( QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
{
}

Rectangle::~Rectangle()
{
}

void Rectangle::setCorner( const QskCorner& corner )
{
    if ( corner != m_data->corner )
    {
        m_data->corner = corner;

        update();
        Q_EMIT cornerChanged();
    }
}

const QskCorner& Rectangle::corner() const
{
    return m_data->corner;
}

void Rectangle::setBorder( const Border& border )
{
    if ( border != m_data->border )
    {
        m_data->border = border;
        Q_EMIT borderChanged();

        update();
    }
}

const Border& Rectangle::border() const
{
    return m_data->border;
}

void Rectangle::setBorderWidth( int width )
{
    setBorder( Border( width, m_data->border.color() ) );
}

int Rectangle::borderWidth() const
{
    return m_data->border.width();
}

void Rectangle::setBorderColor( const QColor& color )
{
    setBorder( Border( m_data->border.width(), color ) );
}

QColor Rectangle::borderColor() const
{
    return m_data->border.color();
}

void Rectangle::setGradient( const QskGradient& gradient )
{
    // are we called, when the gradient gets changed from QML
    // after instantiation TODO ...??
    if ( gradient != m_data->gradient )
    {
        m_data->gradient = gradient;
        m_data->fillColor = QColor();

        update();
        Q_EMIT colorChanged();
    }
}

QskGradient Rectangle::gradient() const
{
    return m_data->gradient;
}

void Rectangle::resetGradient()
{
    m_data->gradient = QskGradient();
}

void Rectangle::setColor( const QColor& color )
{
    if ( color != m_data->fillColor )
    {
        m_data->fillColor = color;
        m_data->gradient = QskGradient();

        update();
        Q_EMIT colorChanged();
    }
}

QColor Rectangle::color() const
{
    return m_data->fillColor;
}

void Rectangle::updateNode( QSGNode* parentNode )
{
    // boilerplate code, that is not needed, when using a skinlet.

    const quint8 nodeRole = 0;

    auto node = static_cast< QskRectNode* >(
        QskSkinlet::findNodeByRole( parentNode, nodeRole ) );

    if ( node == nullptr )
    {
        node = new QskRectNode;
        QskSkinlet::setNodeRole( node, nodeRole );
    }

    const QRectF cr = contentsRect();

    node->setRect( cr );
    node->setRadius( m_data->corner.effectiveRadius( cr.width(), cr.height() ) );
    node->setBorderWidth( m_data->border.width() );
    node->setBorderColor( m_data->border.color() );
    node->setFillColor( m_data->fillColor );
    node->setFillGradient( m_data->gradient );

    node->update();

    if ( node->parent() != parentNode )
        parentNode->appendChildNode( node );
}

#include "moc_Rectangle.cpp"
