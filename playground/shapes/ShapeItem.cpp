/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "ShapeItem.h"

#include <QskStrokeNode.h>
#include <QskShapeNode.h>
#include <QskSGNode.h>

static inline QTransform transformForRects( const QRectF& r1, const QRectF& r2 )
{
    return QTransform::fromTranslate( -r1.x(), -r1.y() )
        * QTransform::fromScale( r2.width() / r1.width(), r2.height() / r1.height() )
        * QTransform::fromTranslate( r2.x(), r2.y() );
}

static inline bool isVisible( const QColor& color )
{
    return color.isValid() && ( color.alpha() > 0 );
}

static inline bool isVisible( const QPen& pen )
{
    return ( pen.style() != Qt::NoPen ) && isVisible( pen.color() );
}

static inline QPen cosmeticPen( const QPen& pen, const QSizeF& size1, const QSizeF& size2 )
{
    if ( pen.isCosmetic() || pen.widthF() <= 0.0 || size2.isEmpty() )
        return pen;

    const auto f = qMin( size1.width() / size2.width(),
        size1.height() / size2.height() );

    auto newPen = pen;
    newPen.setWidthF( pen.widthF() * f );
    newPen.setCosmetic( true );

    return newPen;
}

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

void ShapeItem::setGradient( const QColor& c1, const QColor& c2 )
{
    if ( c1 != m_fillColor[0] || c2 != m_fillColor[1] )
    {
        m_fillColor[0] = c1;
        m_fillColor[1] = c2;

        update();
    }
}

void ShapeItem::setGradient( QGradient::Preset preset )
{
    const auto stops = QGradient( preset ).stops();
    if ( !stops.isEmpty() )
    {
        // gradients with more than 2 clors do not work TODO ...
        setGradient( stops.first().second, stops.last().second );
    }
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
        FillRole,
        BorderRole
    };

    const auto rect = contentsRect();
    const auto pathRect = m_path.controlPointRect();

    auto fillNode = static_cast< QskShapeNode* >(
        QskSGNode::findChildNode( parentNode, FillRole ) );

    auto borderNode = static_cast< QskStrokeNode* >(
        QskSGNode::findChildNode( parentNode, BorderRole ) );

    if ( rect.isEmpty() || pathRect.isEmpty() )
    {
        delete fillNode;
        delete borderNode;

        return;
    }

    const auto pen = ::cosmeticPen( m_pen, rect.size(), pathRect.size() );

    if ( ::isVisible( m_fillColor[0] ) || ::isVisible( m_fillColor[1] ) )
    {
        if ( fillNode == nullptr )
        {
            fillNode = new QskShapeNode;
            QskSGNode::setNodeRole( fillNode, FillRole );

            parentNode->prependChildNode( fillNode );
        }

        auto fillRect = rect;
        if ( pen.style() != Qt::NoPen )
        {
            const auto pw2 = 0.5 * pen.widthF();
            fillRect.adjust( pw2, pw2, -pw2, -pw2 );
        }

        const auto transform = ::transformForRects( pathRect, fillRect );

        if ( m_fillColor[0] != m_fillColor[1] )
        {
            QLinearGradient gradient;
            gradient.setStart( rect.topLeft() );
            gradient.setFinalStop( rect.bottomRight() );
            gradient.setColorAt( 0.0, m_fillColor[0] );
            gradient.setColorAt( 1.0, m_fillColor[1] );

            fillNode->updateNode( m_path, transform, &gradient );
        }
        else
        {
            fillNode->updateNode( m_path, transform, m_fillColor[0] );
        }
    }
    else
    {
        delete fillNode;
    }

    if ( ::isVisible( pen ) )
    {
        if ( pen.widthF() > 1.0 )
        {
            if ( !( pen.isSolid() && pen.color().alpha() == 255 ) )
            {
                /*
                    We might end up with overlapping parts
                    at corners with angles < 180°

                    What about translating the stroke into
                    a path ( QPainterPathStroker ) and using
                    a QskShapeNode then. TODO ...
                 */
            }
        }

        if ( borderNode == nullptr )
        {
            borderNode = new QskStrokeNode;
            QskSGNode::setNodeRole( borderNode, BorderRole );

            parentNode->appendChildNode( borderNode );
        }

        const auto transform = ::transformForRects( pathRect, rect );

        const auto scaledPath = transform.map( m_path );
        borderNode->updateNode( scaledPath, pen );
    }
    else
    {
        delete borderNode;
    }
}

#include "moc_ShapeItem.cpp"
