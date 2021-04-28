/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "LightIntensity.h"
#include "Skin.h"

#include <QskAnimator.h>
#include <QskRgbValue.h>
#include <QskSetup.h>
#include <QskTextLabel.h>

#include <QGuiApplication>
#include <QQuickWindow>
#include <QPainter>
#include <QRadialGradient>

QSK_SUBCONTROL( LightIntensityValueLabel, Text )

QSK_SUBCONTROL( LightDisplay, Panel )
QSK_SUBCONTROL( LightDisplay, ColdPart )
QSK_SUBCONTROL( LightDisplay, WarmPart )

namespace
{
    QColor invertedColor( const QColor& c )
    {
        QColor ret = { 255 - c.red(), 255 - c.green(), 255 - c.blue()};
        return ret;
    }
}

// ### There must be an easier way to do this
class DimmerAnimator : public QskAnimator
{
    public:
        DimmerAnimator( LightDisplay* display, LightDimmer* dimmer )
            : m_display( display )
            , m_dimmer( dimmer )
        {
            QQuickWindow* w = static_cast<QQuickWindow*>( qGuiApp->allWindows().at( 0 ) );
            setWindow( w );
            setDuration( 500 );
            setEasingCurve( QEasingCurve::Linear );
            setAutoRepeat( false );
        }

        void setup() override
        {
            m_backgroundColor = m_display->color( LightDisplay::Panel );
            m_ringGradient = m_dimmer->ringGradient();
        }

        void advance( qreal value ) override
        {
            const QColor c = m_backgroundColor;
            const QColor c2 = invertedColor( c );
            const QColor newColor = QskRgb::interpolated( c2, c, value );
            m_dimmer->setBackgroundColor( newColor );

            QRadialGradient gradient = m_ringGradient;
            QRadialGradient newGradient = gradient;

            for( const QGradientStop& stop : gradient.stops() )
            {
                QColor c = stop.second;
                QColor c2 = invertedColor( c );
                const QColor newColor = QskRgb::interpolated( c, c2, value );
                newGradient.setColorAt( stop.first, newColor );
            }

            m_dimmer->setRingGradient( newGradient );
            m_dimmer->update();
        }

    private:
        QColor m_backgroundColor;
        QRadialGradient m_ringGradient;
        LightDisplay* m_display;
        LightDimmer* m_dimmer;
};

LightDimmer::LightDimmer( const QskGradient& coldGradient, const QskGradient& warmGradient, QQuickItem* parent )
    : QQuickPaintedItem( parent )
    , m_coldGradient( coldGradient )
    , m_warmGradient( warmGradient )
{
    connect( this, &QQuickPaintedItem::contentsSizeChanged, [this]()
    {
        auto size = contentsSize();
        QRadialGradient ringGradient( size.width() / 2, size.height() / 2, 110 );
        QGradientStop stop1( 0.0, "#c0c0c0" );
        QGradientStop stop2( 0.5, "#f0f0f0" );
        QGradientStop stop3( 1.0, "#c0c0c0" );
        ringGradient.setStops( {stop1, stop2, stop3} );

        m_ringGradient = ringGradient;
    } );
}

void LightDimmer::paint( QPainter* painter )
{
    const qreal knobDiameter = 15.65;
    const qreal offset = ( thickness() - knobDiameter ) + 2;

    painter->setRenderHint( QPainter::Antialiasing, true );

    auto size = contentsSize();
    QRectF outerRect( {0, offset}, size );

    painter->setBrush( m_ringGradient );

    painter->setPen( m_backgroundColor );
    painter->drawEllipse( outerRect );

    int startAngle = 16 * 180;
    int middleAngle = 16 * -90;
    int endAngle = 16 * -90;

    QLinearGradient coldGradient( {thickness(), 0.0}, {thickness(), thickness()} );
    coldGradient.setColorAt( 0, m_coldGradient.colorAt( 0 ) );
    coldGradient.setColorAt( 1, m_coldGradient.colorAt( 1 ) );
    painter->setBrush( coldGradient );
    painter->setPen( Qt::transparent );
    painter->drawPie( outerRect, startAngle, middleAngle );

    QLinearGradient warmGradient( {thickness(), 0.0}, {thickness(), thickness()} );
    warmGradient.setColorAt( 0, m_warmGradient.colorAt( 0 ) );
    warmGradient.setColorAt( 1, m_warmGradient.colorAt( 1 ) );
    painter->setBrush( warmGradient );
    painter->drawPie( outerRect, 16 * 90, endAngle );

    painter->setBrush( m_backgroundColor );
    painter->setPen( m_backgroundColor );
    QRectF innerRect( thickness() / 2, thickness() / 2 + offset, size.width() - thickness(), size.height() - thickness() );
    painter->drawEllipse( innerRect );

    painter->setBrush( m_backgroundColor );
    painter->setPen( "#c4c4c4" );
    QRectF knobRect( ( contentsSize().width() - knobDiameter ) / 2, 1, knobDiameter, knobDiameter );
    painter->drawEllipse( knobRect );
}

LightDisplay::LightDisplay( QQuickItem* parent )
    : QskControl( parent )
    , m_leftLabel( new QskTextLabel( QString::number( 0 ), this ) )
    , m_centreLabel( new LightIntensityValueLabel( QString::number( 50 ) + "%", this ) )
    , m_rightLabel( new QskTextLabel( QString::number( 100 ), this ) )
    , m_dimmer( new LightDimmer( gradientHint( ColdPart ), gradientHint( WarmPart ), this ) )
    , m_animator( new DimmerAnimator( this, m_dimmer ) )
{
    m_leftLabel->setSizePolicy( Qt::Horizontal, QskSizePolicy::Maximum );
    m_centreLabel->setSizePolicy( Qt::Horizontal, QskSizePolicy::Maximum );
    m_centreLabel->setZ( 1 );
    m_rightLabel->setSizePolicy( Qt::Horizontal, QskSizePolicy::Maximum );
    m_rightLabel->setZ( 1 );

    setAutoLayoutChildren( true );

    const QColor c = color( Panel );
    m_dimmer->setBackgroundColor( c );

    connect( qskSetup, &QskSetup::skinChanged, [this]()
    {
        m_animator->start();
    } );
}

QskAspect::Subcontrol LightDisplay::effectiveSubcontrol( QskAspect::Subcontrol subControl ) const
{
    if( subControl == QskBox::Panel )
    {
        return LightDisplay::Panel;
    }

    return subControl;
}

void LightDisplay::updateLayout()
{
    const qreal w = width() - ( m_leftLabel->width() + m_rightLabel->width() );
    const int r = qMin( w, height() );
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

#include "moc_LightIntensity.cpp"
