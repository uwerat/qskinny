/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "GradientQuickShape.h"

#include <QskGradient.h>
#include <QskGradientDirection.h>

QSK_QT_PRIVATE_BEGIN

#ifndef signals
    #define signals Q_SIGNALS
#endif

#include <private/qquickitem_p.h>
#include <private/qquickshape_p.h>
#include <private/qquickshape_p_p.h>

QSK_QT_PRIVATE_END

namespace
{
    class ShapePath : public QQuickShapePath
    {
      public:
        ShapePath( QObject* parent = nullptr )
            : QQuickShapePath( parent )
        {
            setStrokeWidth( 0 );
        }

        void setRect( const QRectF& rect )
        {
            auto& path = QQuickShapePathPrivate::get( this )->_path;

            path.clear();
            path.addRect( rect );
        }

        void setGradient( const QRectF& rect, const QskGradient& gradient )
        {
            auto d = QQuickShapePathPrivate::get( this );

            delete d->sfp.fillGradient;

            d->sfp.fillGradient = createShapeGradient( rect, gradient );
            d->sfp.fillGradient->setParent( this );
        }

      private:
        QQuickShapeGradient* createShapeGradient(
            const QRectF& rect, const QskGradient& gradient ) const
        {
            QQuickShapeGradient* shapeGradient = nullptr;

            auto effectiveGradient = gradient.effectiveGradient();
            effectiveGradient.stretchTo( rect );

            switch( static_cast< int >( gradient.type() ) )
            {
                case QskGradient::Linear:
                {
                    const auto dir = effectiveGradient.linearDirection();

                    auto g = new QQuickShapeLinearGradient();

                    g->setX1( dir.x1() );
                    g->setY1( dir.y1() );
                    g->setX2( dir.x2() );
                    g->setY2( dir.y2() );

                    shapeGradient = g;
                    break;
                }

                case QskGradient::Radial:
                {
                    const auto dir = effectiveGradient.radialDirection();

                    auto g = new QQuickShapeRadialGradient();

                    g->setCenterX( dir.x() );
                    g->setCenterY( dir.y() );
                    g->setFocalX( dir.x() );
                    g->setFocalY( dir.y() );

                    g->setCenterRadius( qMax( dir.radiusX(), dir.radiusY() ) );

                    shapeGradient = g;
                    break;
                }

                case QskGradient::Conic:
                {
                    const auto dir = effectiveGradient.conicDirection();

                    auto g = new QQuickShapeConicalGradient();

                    g->setCenterX( dir.x() );
                    g->setCenterY( dir.y() );
                    g->setAngle( dir.startAngle() ); // dir.spanAngle() is not supported

                    shapeGradient = g;
                    break;
                }
            }

            shapeGradient->setSpread(
                static_cast< QQuickShapeGradient::SpreadMode >( gradient.spreadMode() ) );

            /*
                QQuickGradient has been made in the early days of Qt5 for the QML
                use case. Everything - even each stop - is a QObject.
             */
            const auto qtStops = qskToQGradientStops( gradient.stops() );

            for ( const auto& stop : qtStops )
            {
                class MyGradient : public QObject
                {
                  public:
                    QList< QQuickGradientStop* > m_stops;
                };

                auto s = new QQuickGradientStop( shapeGradient );
                s->setPosition( stop.first );
                s->setColor( stop.second );

                reinterpret_cast< MyGradient* >( shapeGradient )->m_stops += s;
            }

            return shapeGradient;
        }
    };

    class ShapeItem : public QQuickShape
    {
      public:
        ShapeItem()
        {
            auto d = QQuickShapePrivate::get( this );
            d->sp += new ShapePath( this );
        }

        QSGNode* updateShapeNode( QQuickWindow* window, const QRectF& rect,
            const QskGradient& gradient, QSGNode* node )
        {
            auto d = QQuickShapePrivate::get( this );

            ShapePath path;
            path.setRect( rect );
            path.setGradient( rect, gradient );

            d->sp += &path;
            d->spChanged = true;

            d->refWindow( window );
            updatePolish();
            node = QQuickShape::updatePaintNode( node, nullptr );
            d->derefWindow();

            d->sp.clear();

            return node;
        }

      private:
        QSGNode* updatePaintNode( QSGNode*, UpdatePaintNodeData* ) override
        {
            Q_ASSERT( false );
            return nullptr;
        }
    };
}

Q_GLOBAL_STATIC( ShapeItem, shapeItem )

QSGNode* GradientQuickShape::updateNode( QQuickWindow* window,
    const QRectF& rect, const QskGradient& gradient, QSGNode* node )
{
    /*
        Unfortunately the different materials for the gradients are hidden
        in private classes of the quickshape module, and can't be accessed
        from application code. Hard to understand why such basic functionality
        is not offered like QSGFlatColorMaterial and friends. Anyway - we have
        QskGradientMaterial now ...

        But for the purpose of comparing our shaders with those from quickshape we
        use a static QQuickShape to create/update scene graph node, that actually
        belong to a different QQuickItem.
     */
    return shapeItem->updateShapeNode( window, rect, gradient, node );
}
