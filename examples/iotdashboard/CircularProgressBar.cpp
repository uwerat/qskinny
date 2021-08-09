/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "CircularProgressBar.h"

#include <QPainter>

CircularProgressBar::CircularProgressBar( const QskGradient& gradient, int progress, QQuickItem* parent )
    : QQuickPaintedItem( parent )
    , m_progress( progress )
{
    // This is a bit hackish, but let's do this properly
    // once QSkinny has an arc renderer in place
    QLinearGradient g( 0, 0, 30, 0 );
    QGradientStop stop1( 0.0, gradient.colorAt( 0 ) );
    QGradientStop stop2( 1.0, gradient.colorAt( 1 ) );
    g.setStops( {stop1, stop2} );
    m_gradient = g;

    connect( this, &QQuickPaintedItem::contentsSizeChanged, [this]()
    {
        auto size = contentsSize();
        QRadialGradient ringGradient( size.width() / 2, size.height() / 2, 45 );
        QGradientStop stop1( 0.0, "#c0c0c0" );
        QGradientStop stop2( 0.5, "#f0f0f0" );
        QGradientStop stop3( 1.0, "#c0c0c0" );
        ringGradient.setStops( {stop1, stop2, stop3} );

        m_ringGradient = ringGradient;
    } );
}

double CircularProgressBar::width() const
{
    return m_width;
}

void CircularProgressBar::setWidth( double width )
{
    m_width = width;
}

QColor CircularProgressBar::backgroundColor() const
{
    return m_backgroundColor;
}

void CircularProgressBar::setBackgroundColor( const QColor& color )
{
    m_backgroundColor = color;
}

QRadialGradient CircularProgressBar::ringGradient() const
{
    return m_ringGradient;
}

void CircularProgressBar::setRingGradient( const QRadialGradient& gradient )
{
    m_ringGradient = gradient;
}

void CircularProgressBar::paint( QPainter* painter )
{
    const auto size = contentsSize();

    const int startAngle = 1440;
    const int endAngle = -16 * ( m_progress / 100.0 ) * 360;

    painter->setRenderHint( QPainter::Antialiasing, true );

    const qreal w = 8.53;

    const QRectF r( 0.5 * w, 0.5 * w, size.width() - w, size.height() - w );

    const QColor c0 ( Qt::lightGray );

    QRadialGradient g1( r.center(), qMin( r.width(), r.height() ) );
    g1.setColorAt( 0.0, c0 );
    g1.setColorAt( 0.5, c0.lighter( 120 ) );
    g1.setColorAt( 1.0, c0 );

    painter->setPen( QPen( g1, w, Qt::SolidLine, Qt::FlatCap ) );
    painter->drawArc( r, startAngle, 16 * 360 );

    QConicalGradient g2( r.center(), 90 );
    g2.setStops( m_gradient.stops() );

    painter->setPen( QPen( g2, w, Qt::SolidLine, Qt::FlatCap ) );
    painter->drawArc( r, startAngle, endAngle );
}
