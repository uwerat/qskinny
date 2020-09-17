#include "LightIntensity.h"
#include "DaytimeSkin.h"

#include <QskTextLabel.h>

#include <QPainter>
#include <QRadialGradient>

LightDimmer::LightDimmer( QQuickItem* parent )
    : QQuickPaintedItem( parent )
{
}

void LightDimmer::paint( QPainter* painter )
{
    const qreal knobDiameter = 15.65;
    const qreal offset = ( thickness() - knobDiameter ) + 2;

    painter->setRenderHint( QPainter::Antialiasing, true );

    auto size = contentsSize();
    QRectF outerRect( {0, offset}, size );

    QRadialGradient gradient( size.width() / 2, size.height() / 2, 70 );
    QGradientStop stop1( 0.0, "#c0c0c0" );
    QGradientStop stop2( 0.5, "#f0f0f0" );
    QGradientStop stop3( 1.0, "#c0c0c0" );
    gradient.setStops( {stop1, stop2, stop3} );

    painter->setBrush( gradient );
    painter->setPen( Qt::white );
    painter->drawEllipse( outerRect );

    int startAngle = 16 * 180;
    int middleAngle = 16 * -90;
    int endAngle = 16 * -90;

    QLinearGradient blueGradient( {thickness(), 0.0}, {thickness(), thickness()} );
    blueGradient.setColorAt( 0, "#a7b0ff" );
    blueGradient.setColorAt( 1, "#6776ff" );
    painter->setBrush( blueGradient );
    painter->setPen( Qt::transparent );
    painter->drawPie( outerRect, startAngle, middleAngle );

    QLinearGradient redGradient( {thickness(), 0.0}, {thickness(), thickness()} );
    redGradient.setColorAt( 0, "#feeeb7" );
    redGradient.setColorAt( 1, "#ff3122" );
    painter->setBrush( redGradient );
    painter->drawPie( outerRect, 16 * 90, endAngle );

    painter->setBrush( Qt::white );
    painter->setPen( Qt::white );
    QRectF innerRect( thickness() / 2, thickness() / 2 + offset, size.width() - thickness(), size.height() - thickness() );
    painter->drawEllipse( innerRect );

    painter->setBrush( Qt::white );
    painter->setPen( "#c4c4c4" );
    QRectF knobRect( ( contentsSize().width() - knobDiameter ) / 2, 1, knobDiameter, knobDiameter );
    painter->drawEllipse( knobRect );
}

LightDisplay::LightDisplay( QQuickItem* parent )
    : QskControl( parent )
    , m_leftLabel( new QskTextLabel( QString::number( 0 ), this ) )
    , m_centreLabel( new QskTextLabel( QString::number( 50 ) + "%", this ) )
    , m_rightLabel( new QskTextLabel( QString::number( 100 ), this ) )
    , m_dimmer( new LightDimmer( this ) )
{
    m_leftLabel->setSizePolicy( Qt::Horizontal, QskSizePolicy::Maximum );
    m_centreLabel->setSizePolicy( Qt::Horizontal, QskSizePolicy::Maximum );
    m_centreLabel->setZ( 1 );
    m_centreLabel->setFontRole( DaytimeSkin::LargeFont );
    m_centreLabel->setTextColor( "#929cb2" );
    m_rightLabel->setSizePolicy( Qt::Horizontal, QskSizePolicy::Maximum );
    m_rightLabel->setZ( 1 );

    setAutoLayoutChildren( true );
}

void LightDisplay::updateLayout()
{
    const int r = qMin( width(), height() );
    m_dimmer->setContentsSize( {r - 4, r - 4} ); // for some reason we need some padding, hence the 4
    m_dimmer->setSize( QSizeF( r, r ) );
    const qreal padding = 8;
    const qreal x = ( width() - ( m_leftLabel->width() + 2 * padding + m_dimmer->width() + m_rightLabel->width() ) ) / 2;
    const qreal offset = 2; // circle doesn't start at 0
    m_leftLabel->setPosition( {x, ( m_dimmer->height() - m_leftLabel->height() ) / 2 + offset} );

    m_dimmer->setPosition( {m_leftLabel->x() + m_leftLabel->width() + padding, 0} );

    qreal centreX = m_dimmer->x() + ( m_dimmer->width() - m_centreLabel->width() ) / 2;
    qreal centreY = m_dimmer->y() + ( m_dimmer->height() - m_centreLabel->height() ) / 2;
    m_centreLabel->setPosition( {centreX, centreY + offset} );

    m_rightLabel->setPosition( {m_dimmer->x() + m_dimmer->width() + padding, m_leftLabel->y()} );
}

LightIntensity::LightIntensity( QQuickItem* parent )
    : Box( "Light intensity", parent )
{
    new LightDisplay( this );
}
