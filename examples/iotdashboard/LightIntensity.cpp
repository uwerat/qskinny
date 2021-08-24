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
#include <QskQuick.h>

#include <QGuiApplication>
#include <QQuickWindow>
#include <QQuickPaintedItem>
#include <QPainter>
#include <QRadialGradient>

QSK_SUBCONTROL( LightDisplay, Panel )
QSK_SUBCONTROL( LightDisplay, ColdPart )
QSK_SUBCONTROL( LightDisplay, WarmPart )
QSK_SUBCONTROL( LightDisplay, ValueText )

namespace
{
    class LightDimmer;

    QColor invertedColor( const QColor& c )
    {
        QColor ret = { 255 - c.red(), 255 - c.green(), 255 - c.blue()};
        return ret;
    }

    class LightDimmer : public QQuickPaintedItem
    {
      public:
        LightDimmer( const QskGradient& coldGradient,
            const QskGradient& warmGradient, QQuickItem* parent );

        double thickness() const
        {
            return m_thickness;
        }

        void setThickness( double thickness )
        {
            m_thickness = thickness;
        }

        QColor backgroundColor() const
        {
            return m_backgroundColor;
        }

        void setBackgroundColor( const QColor& color )
        {
            m_backgroundColor = color;
        }

        QRadialGradient ringGradient() const
        {
            return m_ringGradient;
        }

        void setRingGradient( const QRadialGradient& gradient )
        {
            m_ringGradient = gradient;
        }

        QRectF ringRect() const
        {
            const qreal r = qMin( width(), height() ) - 4;
            return QRectF( 0.0, 0.0, r, r );
        }

      private:
        void paint( QPainter* ) override;
        void updateGradient();

        double m_thickness = 17.57;
        QColor m_backgroundColor;
        QRadialGradient m_ringGradient;
        QskGradient m_coldGradient;
        QskGradient m_warmGradient;
    };

    // ### There must be an easier way to do this
    class DimmerAnimator : public QskAnimator
    {
      public:
        DimmerAnimator( LightDisplay* display, LightDimmer* dimmer )
            : m_display( display )
            , m_dimmer( dimmer )
        {
            QQuickWindow* w = static_cast< QQuickWindow* >( qGuiApp->allWindows().at( 0 ) );
            setWindow( w );
            setDuration( 500 );
            setEasingCurve( QEasingCurve::Linear );
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
}

LightDimmer::LightDimmer( const QskGradient& coldGradient,
        const QskGradient& warmGradient, QQuickItem* parent )
    : QQuickPaintedItem( parent )
    , m_coldGradient( coldGradient )
    , m_warmGradient( warmGradient )
{
    connect( this, &QQuickPaintedItem::widthChanged,
        this, &LightDimmer::updateGradient );

    connect( this, &QQuickPaintedItem::heightChanged,
        this, &LightDimmer::updateGradient );
}

void LightDimmer::updateGradient()
{
    const auto sz = ringRect().size();

    QRadialGradient ringGradient( sz.width() / 2, sz.height() / 2, 110 );
    QGradientStop stop1( 0.0, "#c0c0c0" );
    QGradientStop stop2( 0.5, "#f0f0f0" );
    QGradientStop stop3( 1.0, "#c0c0c0" );
    ringGradient.setStops( {stop1, stop2, stop3} );

    m_ringGradient = ringGradient;
}

void LightDimmer::paint( QPainter* painter )
{
    const auto sz = ringRect().size();

    const qreal knobDiameter = 15.65;
    const qreal offset = ( thickness() - knobDiameter ) + 2;

    painter->setRenderHint( QPainter::Antialiasing, true );

    QRectF outerRect( 0, offset, sz.width(), sz.height() );

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
    QRectF innerRect( thickness() / 2, thickness() / 2 + offset, sz.width() - thickness(), sz.height() - thickness() );
    painter->drawEllipse( innerRect );

    painter->setBrush( m_backgroundColor );
    painter->setPen( "#c4c4c4" );
    QRectF knobRect( ( sz.width() - knobDiameter ) / 2, 1, knobDiameter, knobDiameter );
    painter->drawEllipse( knobRect );
}

LightDisplay::LightDisplay( QQuickItem* parent )
    : QskLinearBox( Qt::Horizontal, parent )
{
    setSubcontrolProxy( QskBox::Panel, LightDisplay::Panel );

    auto leftLabel = new QskTextLabel( QString::number( 0 ), this );
    leftLabel->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

    auto rightLabel = new QskTextLabel( QString::number( 100 ), this );
    rightLabel->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

    auto dimmer = new LightDimmer( gradientHint( ColdPart ), gradientHint( WarmPart ), this );
    dimmer->setBackgroundColor( color( Panel ) );

    m_valueLabel = new QskTextLabel( QString::number( 50 ) + "%", dimmer );
    m_valueLabel->setSubcontrolProxy( QskTextLabel::Text, LightDisplay::ValueText );

    addItem( leftLabel );
    addItem( dimmer );
    addItem( rightLabel );

    auto animator = new DimmerAnimator( this, dimmer );
    connect( qskSetup, &QskSetup::skinChanged,
        [animator]() { animator->start(); } );
}

void LightDisplay::updateLayout()
{
    QskLinearBox::updateLayout();

    auto dimmer = static_cast< const LightDimmer* >( m_valueLabel->parentItem() );

    QRectF r;
    r.setSize( m_valueLabel->sizeConstraint() );
    r.moveCenter( dimmer->ringRect().center() + QPointF( 0, 4 ) );

    m_valueLabel->setGeometry( r );
}

#include "moc_LightIntensity.cpp"
