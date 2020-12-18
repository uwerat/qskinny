/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SpeedometerDisplay.h"
#include "Speedometer.h"

#include <QskTextLabel.h>

#include <QTime>
#include <QTimer>

#include <cstdlib>

namespace
{
    class Dial : public Speedometer
    {
      public:
        Dial( const QString& title, QQuickItem* parent = nullptr )
            : Speedometer( parent )
        {
            setPolishOnResize( true );
            m_label = new QskTextLabel( title, this );
        }

      protected:
        void updateLayout() override
        {
            const auto r = layoutRect();

            const auto hint = m_label->sizeConstraint();

            const qreal y = r.y() + 0.6 * r.height() - 0.5 * hint.height();
            const qreal x = r.center().x() - 0.5 * hint.width();

            m_label->setGeometry( x, y, hint.width(), hint.height() );
        }

      private:
        QskTextLabel* m_label;
    };

    class RevCounter : public Dial
    {
      public:
        RevCounter( QQuickItem* parent = nullptr )
            : Dial( "x 1000 min^-1", parent )
        {
            setMinimum( 145 );
            setMaximum( 305 );
            setValue( 200 );

            constexpr int labelsCount = 8;

            QVector< QString > labels;
            labels.reserve( labelsCount );

            for ( int i = 0; i < labelsCount; i++ )
                labels += QString::number( i );

            setTickLabels( labels );
        }
    };

    class Speedo : public Dial
    {
      public:
        Speedo( QQuickItem* parent = nullptr )
            : Dial( "km/h", parent )
        {
            setMinimum( -215 );
            setMaximum( 35 );
            setValue( -90 );

            constexpr int labelsCount = 17;

            QVector< QString > labels;
            labels.reserve( labelsCount );

            for ( int i = 0; i < labelsCount; i++ )
                labels.append( QString::number( i * 10 ) );

            setTickLabels( labels );
        }

        void updateValue()
        {
            setValue( value() + std::rand() % 3 - 0.95 );
        }
    };

    class FuelGauge : public Dial
    {
      public:
        FuelGauge( QQuickItem* parent = nullptr )
            : Dial( "fuel", parent )
        {
            setMinimum( 195 );
            setMaximum( 345 );
            setValue( 330 );

            setTickLabels( { "0", "", "1/2", "", "1/1" } );
        }

        void updateValue()
        {
            setValue( 0.99997 * value() );
        }
    };
}

SpeedometerDisplay::SpeedometerDisplay( QQuickItem* parent )
    : QskLinearBox( Qt::Horizontal, parent )
{
    std::srand( static_cast< uint >( QTime::currentTime().msec() ) );

    (void ) new RevCounter( this );
    auto speedometer = new Speedo( this );
    auto fuelGauge = new FuelGauge( this );

    setMargins( 10 );
    setSpacing( 10 );
    
    auto timer = new QTimer( this );

    connect( timer, &QTimer::timeout, speedometer, &Speedo::updateValue );
    connect( timer, &QTimer::timeout, fuelGauge, &FuelGauge::updateValue );

    timer->setInterval( 16 );
    timer->start();
}
