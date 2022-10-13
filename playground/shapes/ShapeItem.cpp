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

namespace
{
    class ShapeNode : public QskShapeNode
    {
      public:
        void updateShape( const QPainterPath& path,
            const QTransform& transform, const QRectF& rect, const Gradient& gradient )
        {
            if ( gradient.isMonochrome() )
            {
                updateNode( path, transform, gradient.stops().first().color() );
                return;
            }

            /*
                Stupid code to map Gradient -> QGradient
                Can be removed once Gradient has been merged into QskGradient.
             */

            switch( static_cast<int>( gradient.type() ) )
            {
                case QGradient::LinearGradient:
                {
                    const auto& g = gradient.asLinearGradient();

                    QLinearGradient qgradient( g.start(), g.stop() );
                    qgradient.setSpread( g.spread() );
                    qgradient.setStops( g.qtStops() );

                    updateNode( path, transform, rect, &qgradient );

                    break;
                }
                case QGradient::RadialGradient:
                {
                    const auto& g = gradient.asRadialGradient();

                    QRadialGradient qgradient( g.center(), g.radius() );
                    qgradient.setSpread( g.spread() );
                    qgradient.setStops( g.qtStops() );

                    updateNode( path, transform, rect, &qgradient );

                    break;
                }
                case QGradient::ConicalGradient:
                {
                    const auto& g = gradient.asConicGradient();

                    QConicalGradient qgradient( g.center(), g.startAngle() );
                    qgradient.setSpread( g.spread() );
                    qgradient.setStops( g.qtStops() );

                    /*
                        Once ConicGradient has become QskConicGradient we do not
                        need QConicalGradient anymore and passing the spanAngle
                        as extra parameter can go away.
                     */
                    updateNode( path, transform, rect, &qgradient, g.spanAngle() );

                    break;
                }
            }
        }
    };
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

void ShapeItem::setGradient( const Gradient& gradient )
{
    if ( gradient != m_gradient )
    {
        m_gradient = gradient;
        update();
    }
}

const Gradient& ShapeItem::gradient() const
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

    auto fillNode = static_cast< ShapeNode* >(
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
            fillNode = new ShapeNode;
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
        fillNode->updateShape( m_path, transform, fillRect, m_gradient );
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
