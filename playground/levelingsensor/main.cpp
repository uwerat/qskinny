/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "SkinFactory.h"

#include <SkinnyShortcut.h>

#include <QskObjectCounter.h>
#include <QskSkinManager.h>
#include <QskWindow.h>

#include <LevelingSensor/QskLevelingSensor.h>
#include <QskIntervalF.h>
#include <QskLinearBox.h>
#include <QskSlider.h>
#include <QskTextLabel.h>

#include <QGuiApplication>
#include <QQuickWindow>

namespace
{
    void updateTickmarks( const Qt::Axis axis, const QskIntervalF& intervalA,
        const QskIntervalF& intervalB, QskLevelingSensor* const sensor )
    {
        QskScaleTickmarks tickmarks;
        QVector< qreal > major;
        QVector< qreal > medium;
        QVector< qreal > minor;

        for ( const auto& interval : { intervalA, intervalB } )
        {
            for ( int deg = ( int ) interval.lowerBound(); deg < ( int ) interval.upperBound();
                  ++deg )
            {
                if ( deg % 45 == 0 )
                {
                    major << deg;
                }
                if ( deg % 45 != 0 && deg % 5 == 0 )
                {
                    medium << deg;
                }
                if ( deg % 45 != 0 && deg % 5 != 0 )
                {
                    minor << deg;
                }
            }
        }

        tickmarks.setMajorTicks( major );
        tickmarks.setMediumTicks( medium );
        tickmarks.setMinorTicks( minor );
        sensor->setTickmarks( axis, tickmarks );
    }

    void updateTickmarksLabels( const Qt::Axis axis, const QskIntervalF& intervalA,
        const QskIntervalF& intervalB, int step, QskLevelingSensor* const sensor )
    {
        QskLevelingSensor::TickmarksLabels labels;
        for ( const auto& interval : { intervalA, intervalB } )
        {
            for ( int deg = ( ( int ) interval.lowerBound() / step ) * step;
                  deg < ( int ) interval.upperBound(); deg += step )
            {
                labels << QskLevelingSensor::TickmarksLabels::value_type{ ( float ) deg,
                    QString( deg > 0 ? "+" : "" ) + QString::number( deg ) };
            }
        }
        sensor->setTickmarksLabels( axis, labels );
    }

    Q_REQUIRED_RESULT QskSlider* makeTickmarksSlider( const Qt::Axis axis, QskLevelingSensor* const sensor, int min, int max, std::function<QskIntervalF(qreal)> intervalA, std::function<QskIntervalF(qreal)> intervalB, QQuickItem* const parent = nullptr )
    {
        auto* const slider = new QskSlider( Qt::Horizontal, parent );
        slider->setMinimum( min );
        slider->setMaximum( max );

        QObject::connect(slider, &QskSlider::valueChanged, sensor, [ = ]( const qreal degree ) {
            updateTickmarks( axis, intervalA(degree), intervalB(degree), sensor );
            updateTickmarksLabels( axis, intervalA( degree ), intervalB( degree ), 10, sensor );
        } );

        return slider;
    }

    Q_REQUIRED_RESULT QskSlider* makeRotationSlider(
        const Qt::Axis axis, QskLevelingSensor* const sensor, QQuickItem* const parent = nullptr )
    {
        auto* const slider = new QskSlider( Qt::Horizontal, parent );
        slider->setMinimum( -360 );
        slider->setMaximum( +360 );

        QObject::connect( sensor, &QskLevelingSensor::rotationChanged, slider,
            [ slider, axis ]( const QVector3D& degree ) { slider->setValue( degree[ axis ] ); } );

        QObject::connect( slider, &QskSlider::valueChanged, sensor,
            [ sensor, axis ]( const qreal degree ) { sensor->setRotation( axis, degree ); } );

        return slider;
    }

    class Window : public QskWindow
    {
      public:
        Window()
        {
            auto* const root = new QskLinearBox( Qt::Horizontal, contentItem() );
            auto* const left = new QskLinearBox( Qt::Vertical, root );
            auto* const right = new QskLinearBox( Qt::Vertical, root );
            auto* const sensor = new QskLevelingSensor( left );

            auto linearIntervalA = [](const qreal degree)->QskIntervalF{ return {-degree, +degree};};
            auto linearIntervalB = [](const qreal degree)->QskIntervalF{ return {}; };
            
            auto radialIntervalA = [](const qreal degree)->QskIntervalF{ return {-degree, +degree};};
            auto radialIntervalB = [](const qreal degree)->QskIntervalF{ return {180-degree, 180+degree};};

            ( void ) new QskTextLabel( "Tickmarks X", right );
            auto* const sliderTickmarksX = makeTickmarksSlider( Qt::XAxis, sensor, 0, 90, linearIntervalA, linearIntervalB, right );
            ( void ) new QskTextLabel( "Tickmarks Y", right );
            auto* const sliderTickmarksY = makeTickmarksSlider( Qt::YAxis, sensor, 0, 90, linearIntervalA, linearIntervalB, right );         
            ( void ) new QskTextLabel( "Tickmarks Z", right );
            auto* const sliderTickmarksZ = makeTickmarksSlider( Qt::ZAxis, sensor, 0, 90, radialIntervalA, radialIntervalB, right );

            ( void ) new QskTextLabel( "Rotation X", right );
            ( void ) makeRotationSlider( Qt::XAxis, sensor, right );
            ( void ) new QskTextLabel( "Rotation Y", right );
            ( void ) makeRotationSlider( Qt::YAxis, sensor, right );
            ( void ) new QskTextLabel( "Rotation Z", right );
            ( void ) makeRotationSlider( Qt::ZAxis, sensor, right );

            sliderTickmarksX->setValue(15);
            sliderTickmarksY->setValue(15);
            sliderTickmarksZ->setValue(30);
        }
    };
}

int main( int argc, char** argv )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    qskSkinManager->setPluginPaths( QStringList() ); // no skin plugins
    qskSkinManager->registerFactory( QStringLiteral( "sample" ), new SkinFactory() );

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    Window window;
    window.showMaximized();

    return app.exec();
}
