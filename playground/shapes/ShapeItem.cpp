/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "ShapeItem.h"
#include "Stroke.h"

#include <QskGradient.h>
#include <QskStrokeNode.h>
#include <QskShapeNode.h>
#include <QskSGNode.h>

#include <qpainterpath.h>
#include <qpen.h>

static inline QTransform transformForRects( const QRectF& r1, const QRectF& r2 )
{
    return QTransform::fromTranslate( -r1.x(), -r1.y() )
        * QTransform::fromScale( r2.width() / r1.width(), r2.height() / r1.height() )
        * QTransform::fromTranslate( r2.x(), r2.y() );
}

static inline qreal effectiveStrokeWidth(
    const Stroke& stroke, const QRectF& r1, const QRectF& r2 )
{
    qreal width = qMax( stroke.width(), 0.0 );

    if ( !stroke.isCosmetic() )
    {
        const qreal sx = r1.width() / r2.width();
        const qreal sy = r1.height() / r2.height();

        width *= std::min( sx, sy );
    }

    return width;
}

class ShapeItem::PrivateData
{
  public:
    inline PrivateData()
        : stroke( Qt::black, -1.0 ) // no stroke
        , gradient( Qt::white )
    {
    }

    Stroke stroke;
    QskGradient gradient;
    QPainterPath path;
};

ShapeItem::ShapeItem( QQuickItem* parent )
    : QskControl( parent )
    , m_data( new PrivateData() )
{
    setMargins( 20 );
    setSizePolicy( QskSizePolicy::Ignored, QskSizePolicy::Ignored );
}

ShapeItem::~ShapeItem()
{
}

void ShapeItem::setStroke( const QColor& color, qreal width )
{
    setStroke( Stroke( color, width ) );
}

void ShapeItem::setStroke( const Stroke& stroke )
{
    if ( stroke != m_data->stroke )
    {
        m_data->stroke = stroke;
        update();

        Q_EMIT strokeChanged( m_data->stroke );
    }
}

void ShapeItem::resetStroke()
{
    setStroke( Stroke( Qt::black, -1.0 ) );
}

Stroke ShapeItem::stroke() const
{
    return m_data->stroke;
}

void ShapeItem::setGradient( const QskGradient& gradient )
{
    if ( gradient != m_data->gradient )
    {
        m_data->gradient = gradient;
        update();

        Q_EMIT gradientChanged( m_data->gradient );
    }
}

void ShapeItem::resetGradient()
{
    setGradient( Qt::white );
}

QskGradient ShapeItem::gradient() const
{
    return m_data->gradient;
}

void ShapeItem::setPath( const QPainterPath& path )
{
    if ( path != m_data->path )
    {
        m_data->path = path;
        update();

        Q_EMIT pathChanged( m_data->path );
    }
}

void ShapeItem::resetPath()
{
    setPath( QPainterPath() );
}

QPainterPath ShapeItem::path() const
{
    return m_data->path;
}

void ShapeItem::updateNode( QSGNode* parentNode )
{
    enum NodeRole
    {
        FillRole,
        BorderRole
    };

    const auto rect = contentsRect();
    const auto pathRect = m_data->path.controlPointRect();

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

    if ( m_data->gradient.isVisible() )
    {
        if ( fillNode == nullptr )
        {
            fillNode = new QskShapeNode;
            QskSGNode::setNodeRole( fillNode, FillRole );

            parentNode->prependChildNode( fillNode );
        }

        auto fillRect = rect;
#if 0
        /*
            when the stroke is not opaque ( transparent color or dashed ) we
            would see, that the filling is not inside. But simply adjusting
            by the half of the stroke width is only correct for rectangles. TODO ...
         */
        const auto pw2 = 0.5 * ::effectiveStrokeWidth( m_data->stroke, rect, pathRect );
        fillRect.adjust( pw2, pw2, -pw2, -pw2 );
#endif

        const auto transform = ::transformForRects( pathRect, fillRect );
        fillNode->updateNode( m_data->path, transform, fillRect, m_data->gradient );
    }
    else
    {
        delete fillNode;
    }

    if ( m_data->stroke.isVisible() )
    {
        const auto pen = m_data->stroke.toPen();
#if 0
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
#endif

        if ( borderNode == nullptr )
        {
            borderNode = new QskStrokeNode;
            QskSGNode::setNodeRole( borderNode, BorderRole );

            parentNode->appendChildNode( borderNode );
        }

        const auto transform = ::transformForRects( pathRect, rect );
        borderNode->updateNode( m_data->path, transform, pen );
    }
    else
    {
        delete borderNode;
    }
}

#include "moc_ShapeItem.cpp"
