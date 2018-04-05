#include "SpeedometerDisplay.h"

#include "Speedometer.h"

#include <QskLinearBox.h>
#include <QskTextLabel.h>

#include <QTime>
#include <QTimer>
#include <QtGlobal>

SpeedometerDisplay::SpeedometerDisplay( QQuickItem *parent ) :
    QskControl( parent )
{
    qsrand( QTime::currentTime().msec() );

    auto box = new QskLinearBox( Qt::Horizontal, this );
    box->setAutoAddChildren( true );
    box->setAutoLayoutChildren( true );
    box->setMargins( QMarginsF( 40, 20, 40, 20 ) );
    box->setAlignment( 0, Qt::AlignHCenter );

    auto revCounter = new Speedometer( box );
    revCounter->setFixedSize( QSizeF( 300, 300 ) );
//    revCounter->setSizePolicy( QskSizePolicy::Maximum, QskSizePolicy::Maximum );
    revCounter->setValue( 270 );

    auto speedometer = new Speedometer( box );
    speedometer->setFixedSize( QSizeF( 300, 300 ) );
//    speedometer->setSizePolicy( QskSizePolicy::Maximum, QskSizePolicy::Maximum );
    speedometer->setValue( 270 );

    auto timer = new QTimer( this );
    connect( timer, &QTimer::timeout, this, [ speedometer ]() {
        int newValue = speedometer->value() + qrand() % 3 - 0.8;
        speedometer->setValue( newValue );
    });
    timer->setInterval( 16 );
    timer->start();

    auto fuelGauge = new Speedometer( box );
    fuelGauge->setFixedSize( QSizeF( 200, 200 ) );
    fuelGauge->setValue( 270 );
}
