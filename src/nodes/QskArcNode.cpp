/**********************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskArcNode.h"

#include <QPainter>

QskArcNode::QskArcNode()
{
}

QskArcNode::~QskArcNode()
{
}

void QskArcNode::setArcData( const QskGradient &gradient,
    QGradient::Type gradientType, double width, double value, double origin,
    double maximum, bool isIndeterminate, double position )
{
    m_gradient = gradient;
    m_gradientType = gradientType;
    m_width = width;
    m_value = value;
    m_origin = origin;
    m_maximum = maximum;
    m_isIndeterminate = isIndeterminate;
    m_position = position;
}

void QskArcNode::paint( QPainter *painter, const QSizeF &size )
{
    int startAngle;
    int spanAngle;

    if( m_isIndeterminate )
    {
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

uint QskArcNode::hash() const
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
