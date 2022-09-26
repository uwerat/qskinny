/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "ShapeItem.h"

#include <QskStrokeNode.h>
#include <QskShapeNode.h>
#include <QskSGNode.h>

ShapeItem::ShapeItem( QQuickItem* parent )
    : QskControl( parent )
{
    setMargins( 20 );
}

ShapeItem::~ShapeItem()
{
}

void ShapeItem::setPen( const QPen& pen )
{
    if ( pen != m_pen )
    {
        m_pen = pen;
        update();
    }
}

QPen ShapeItem::pen() const
{
    return m_pen;
}

void ShapeItem::setFillColor( const QColor& color )
{
    if ( color != m_fillColor )
    {
        m_fillColor = color;
        update();
    }
}

QColor ShapeItem::fillColor() const
{
    return m_fillColor;
}

void ShapeItem::setPath( const QPainterPath& path )
{
    if ( path != m_path )
    {
        m_path = path;
        update();
    }
}

QPainterPath ShapeItem::path() const
{
    return m_path;
}

void ShapeItem::updateNode( QSGNode* parentNode )
{
    enum NodeRole
    {
        ShapeRole,
        StrokeRole
    };

    const auto pathRect = m_path.controlPointRect();
    const auto rect = contentsRect();

#if 1
    QTransform transform;
    transform.translate( rect.left(), rect.top() );

    transform.scale( rect.width() / pathRect.width(),
        rect.height() / pathRect.height() );

    /*
        The triangulators in the nodes are able to do transformations
        on the fly. TODO ...
     */
    const auto effectivePath = transform.map( m_path );
#endif

    auto shapeNode = static_cast< QskShapeNode* >(
        QskSGNode::findChildNode( parentNode, ShapeRole ) );

    if ( pathRect.isEmpty() || rect.isEmpty() )
    {
        delete shapeNode;
    }
    else
    {
        if ( shapeNode == nullptr )
        {
            shapeNode = new QskShapeNode;
            QskSGNode::setNodeRole( shapeNode, ShapeRole );
        }

        shapeNode->updateNode( effectivePath, m_fillColor );

        if ( shapeNode->parent() != parentNode )
            parentNode->prependChildNode( shapeNode );
    }

    auto strokeNode = static_cast< QskStrokeNode* >(
        QskSGNode::findChildNode( parentNode, StrokeRole ) );

    if ( pathRect.isEmpty() || rect.isEmpty() )
    {
        delete strokeNode;
    }
    else
    {
        if ( strokeNode == nullptr )
        {
            strokeNode = new QskStrokeNode;
            QskSGNode::setNodeRole( strokeNode, StrokeRole );
        }

        strokeNode->updateNode( effectivePath, m_pen );

        if ( strokeNode->parent() != parentNode )
            parentNode->appendChildNode( strokeNode );
    }
}

#include "moc_ShapeItem.cpp"
