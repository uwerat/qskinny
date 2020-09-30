#include "CircularProgressBar.h"

#include <QPainter>

CircularProgressBar::CircularProgressBar( const QGradient& gradient, int progress, QQuickItem* parent )
    : QQuickPaintedItem( parent )
    , m_gradient( gradient )
    , m_progress( progress )
{
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

void CircularProgressBar::paint( QPainter* painter )
{
    auto size = contentsSize();
    QRectF outerRect( {0, 0}, size );

    painter->setRenderHint( QPainter::Antialiasing, true );

    painter->setBrush( m_ringGradient );
    painter->setPen( m_backgroundColor );
    painter->drawEllipse( outerRect );

    int startAngle = 1440;
    int endAngle = -16 * ( m_progress / 100.0 ) * 360;

    painter->setBrush( m_gradient );
    painter->drawPie( outerRect, startAngle, endAngle );

    painter->setBrush( m_backgroundColor );
    painter->setPen( m_backgroundColor );
    QRectF innerRect( width() / 2, width() / 2, size.width() - width(), size.height() - width() );
    painter->drawEllipse( innerRect );
}
