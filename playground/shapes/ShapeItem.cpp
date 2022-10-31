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

static inline qreal effectivePenWidth(
    const QPen& pen, const QRectF& r1, const QRectF& r2 )
{
    qreal width = pen.widthF();

    if ( !pen.isCosmetic() )
    {
        const qreal sx = r1.width() / r2.width();
        const qreal sy = r1.height() / r2.height();

        width *= std::min( sx, sy );
    }

    return width;
}

ShapeItem::ShapeItem( QQuickItem* parent )
    : QskControl( parent )
{
    setMargins( 20 );
    setSizePolicy( QskSizePolicy::Ignored, QskSizePolicy::Ignored );
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

void ShapeItem::setGradient( const QskGradient& gradient )
{
    if ( gradient != m_gradient )
    {
        m_gradient = gradient;
        update();
    }
}

const QskGradient& ShapeItem::gradient() const
{
    return m_gradient;
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

    if ( m_gradient.isVisible() )
    {
        if ( fillNode == nullptr )
        {
            fillNode = new QskShapeNode;
            QskSGNode::setNodeRole( fillNode, FillRole );

            parentNode->prependChildNode( fillNode );
        }

        auto fillRect = rect;
        if ( m_pen.style() != Qt::NoPen )
        {
            const auto pw2 = 0.5 * ::effectivePenWidth( m_pen, rect, pathRect );
            fillRect.adjust( pw2, pw2, -pw2, -pw2 );
        }

        const auto transform = ::transformForRects( pathRect, fillRect );
        fillNode->updateNode( m_path, transform, fillRect, m_gradient );
    }
    else
    {
        delete fillNode;
    }

    if ( ::isVisible( m_pen ) )
    {
#if 0
        if ( m_pen.widthF() > 1.0 )
        {
            if ( !( m_pen.isSolid() && m_pen.color().alpha() == 255 ) )
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
#endif

        if ( borderNode == nullptr )
        {
            borderNode = new QskStrokeNode;
            QskSGNode::setNodeRole( borderNode, BorderRole );

            parentNode->appendChildNode( borderNode );
        }

        const auto transform = ::transformForRects( pathRect, rect );
        borderNode->updateNode( m_path, transform, m_pen );
    }
    else
    {
        delete borderNode;
    }
}

#include "moc_ShapeItem.cpp"
