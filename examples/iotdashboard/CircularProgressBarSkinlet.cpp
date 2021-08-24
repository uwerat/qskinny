/******************************************************************************
 * QSkinny - Copyright (C) 2021 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "CircularProgressBarSkinlet.h"
#include "CircularProgressBar.h"

#include <QskPaintedNode.h>

#include <QEasingCurve>
#include <QPainter>

namespace
{
    class ArcNode : public QskPaintedNode
    {
      public:
        void setGradient( const QskGradient& gradient )
        {
            m_gradient = gradient;
        }

        void setGradientType( QGradient::Type type )
        {
            m_gradientType = type;
        }

        void setWidth( double width )
        {
            m_width = width;
        }

        void setValue( double value )
        {
            m_value = value;
        }

        void setOrigin( double origin )
        {
            m_origin = origin;
        }

        void setMaximum( double maximum )
        {
            m_maximum = maximum;
        }

        void setIndeterminate( bool isIndeterminate )
        {
            m_isIndeterminate = isIndeterminate;
        }

        void setPosition( double position )
        {
            m_position = position;
        }

        void paint( QPainter* painter, const QSizeF& size ) override
        {
            int startAngle;
            int spanAngle;

            if( m_isIndeterminate )
            {
                static const QEasingCurve curve( QEasingCurve::Linear );

                startAngle = -1 * m_position * 360;
                // the other option is to just set a fixed value for the
                // span angle (or do some advanced stuff with easing curves)
                spanAngle = qAbs( 0.5 - m_position ) * 360;
            }
            else
            {
                startAngle = 90 + -1 * ( m_origin / m_maximum ) * 360;
                spanAngle = -1 * ( m_value / m_maximum ) * 360;
            }

            painter->setRenderHint( QPainter::Antialiasing, true );

            const QRectF r( 0.5 * m_width, 0.5 * m_width,
                size.width() - m_width, size.height() - m_width );

            QGradientStops stops;

            for( const QskGradientStop& stop : m_gradient.stops() )
            {
                QGradientStop s( stop.position(), stop.color() );
                stops.append( s );
            }

            if( m_gradientType == QGradient::RadialGradient )
            {
                QRadialGradient radialGradient( r.center(), qMin( r.width(), r.height() ) );
                radialGradient.setStops( stops );

                painter->setPen( QPen( radialGradient, m_width, Qt::SolidLine, Qt::FlatCap ) );
                painter->drawArc( r, startAngle * 16, spanAngle * 16 );
            }
            else
            {
                QConicalGradient conicalGradient( r.center(), startAngle );
                conicalGradient.setStops( stops );

                painter->setPen( QPen( conicalGradient, m_width, Qt::SolidLine, Qt::FlatCap ) );
                painter->drawArc( r, startAngle * 16, spanAngle * 16 );
            }
        }

        uint hash() const override
        {
            uint h = qHash( m_gradientType );
            h = qHash( m_width, h );
            h = qHash( m_value, h );
            h = qHash( m_origin, h );
            h = qHash( m_maximum, h );
            h = qHash( m_isIndeterminate, h );
            h = qHash( m_position, h );

            for( const QskGradientStop& stop : m_gradient.stops() )
            {
                h = stop.hash( h );
            }

            return h;
        }

      private:
        QskGradient m_gradient;
        QGradient::Type m_gradientType;
        double m_width;
        double m_value;
        double m_origin;
        double m_maximum;
        bool m_isIndeterminate;
        double m_position;
    };
}

CircularProgressBarSkinlet::CircularProgressBarSkinlet( QskSkin* skin )
    : QskSkinlet( skin )
{
    setNodeRoles( { GrooveRole, BarRole } );
}

CircularProgressBarSkinlet::~CircularProgressBarSkinlet()
{
}

QRectF CircularProgressBarSkinlet::subControlRect(
    const QskSkinnable*, const QRectF& contentsRect, QskAspect::Subcontrol ) const
{
    return contentsRect;
}

QSGNode* CircularProgressBarSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto bar = static_cast< const CircularProgressBar* >( skinnable );

    switch( nodeRole )
    {
        case GrooveRole: // fall through
        case BarRole:
        {
            return updateBarNode( bar, nodeRole, node );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* CircularProgressBarSkinlet::updateBarNode(
    const CircularProgressBar* bar, quint8 nodeRole, QSGNode* node ) const
{
    auto arcNode = static_cast< ArcNode* >( node );

    if( arcNode == nullptr )
    {
        arcNode = new ArcNode();
    }

    const auto subControl = ( nodeRole == GrooveRole ) ? CircularProgressBar::Groove
                                                       : CircularProgressBar::Bar;

    const QskGradient gradient = bar->gradientHint( subControl );

    const QGradient::Type type = ( nodeRole == GrooveRole ) ?
        QGradient::RadialGradient : QGradient::ConicalGradient;

    const double width = bar->metric( subControl | QskAspect::Size );
    const double value = ( nodeRole == GrooveRole ) ? bar->maximum() : bar->value();

    arcNode->setGradient( gradient );
    arcNode->setGradientType( type );
    arcNode->setWidth( width );
    arcNode->setOrigin( bar->origin() );
    arcNode->setMaximum( bar->maximum() );
    arcNode->setIndeterminate( bar->isIndeterminate() );

    if( bar->isIndeterminate() )
    {
        const double position = bar->metric( CircularProgressBar::Bar | QskAspect::Position );
        arcNode->setPosition( position );
    }
    else
    {
        arcNode->setValue( value );
    }

    QQuickWindow* window = bar->window();
    const QRect rect = bar->contentsRect().toRect();
    arcNode->update( window, QskTextureRenderer::AutoDetect, rect );

    return arcNode;
}

#include "moc_CircularProgressBarSkinlet.cpp"
