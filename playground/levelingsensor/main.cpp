/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "SkinFactory.h"

#include <SkinnyShortcut.h>

#include <QskObjectCounter.h>
#include <QskSkinManager.h>
#include <QskWindow.h>

#include <QskIntervalF.h>
#include <QskLevelingSensor.h>
#include <QskLinearBox.h>
#include <QskScaleTickmarks.h>
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

    Q_REQUIRED_RESULT QskSlider* makeAngleSlider( const Qt::Axis axis,
        QskLevelingSensor* const sensor, double min = 0, double max = 90, QQuickItem* const parent = nullptr)
    {
        auto* const slider = new QskSlider( Qt::Horizontal, parent );
        slider->setMinimum( min );
        slider->setMaximum( max );
        slider->setValue( sensor->angles()[ axis ] );

        QObject::connect(slider, &QskSlider::valueChanged, sensor, [axis, sensor](const qreal v){
            auto angles = sensor->angles();
            angles[axis] = v;
            sensor->setAngle(angles);
        });

        return slider;
    }

    Q_REQUIRED_RESULT QskSlider* makeTickmarksSlider( const Qt::Axis axis,
        QskLevelingSensor* const sensor, int min, int max,
        std::function< QskIntervalF( qreal ) > intervalA,
        std::function< QskIntervalF( qreal ) > intervalB, QQuickItem* const parent = nullptr )
    {
        auto* const slider = new QskSlider( Qt::Horizontal, parent );
        slider->setMinimum( min );
        slider->setMaximum( max );

        QObject::connect( slider, &QskSlider::valueChanged, sensor, [ = ]( const qreal degree ) {
            updateTickmarks( axis, intervalA( degree ), intervalB( degree ), sensor );
            updateTickmarksLabels( axis, intervalA( degree ), intervalB( degree ), 10, sensor );
        } );

        return slider;
    }

    Q_REQUIRED_RESULT QskSlider* makeRotationSlider( const Qt::Axis axis,
        QskLevelingSensor* const sensor, const QskAspect::Subcontrol subControl,
        QQuickItem* const parent = nullptr )
    {
        auto* const slider = new QskSlider( Qt::Horizontal, parent );
        slider->setMinimum( -360 );
        slider->setMaximum( +360 );

        QObject::connect( sensor, &QskLevelingSensor::subControlRotationChanged, slider,
            [ = ]( const QskAspect::Subcontrol control, const QVector3D& degree ) {
                if ( control == subControl )
                {
                    slider->setValue( degree[ axis ] );
                }
            } );

        QObject::connect( slider, &QskSlider::valueChanged, sensor, [ = ]( const qreal degree ) {
            auto d = sensor->subControlRotation( subControl );
            d[ axis ] = degree;
            sensor->setSubControlRotation( subControl, d );
        } );

        return slider;
    }

    class Window : public QskWindow
    {
      public:
        Window()
        {
            auto* const root = new QskLinearBox( Qt::Horizontal, contentItem() );
            root->setSpacing( 8 );
            root->setMargins( 8 );
            auto* const left = new QskLinearBox( Qt::Vertical, root );
            auto* const right = new QskLinearBox( Qt::Vertical, root );
            auto* const sensor = new QskLevelingSensor( left );

            auto linearIntervalA = []( const qreal degree ) -> QskIntervalF {
                return { -degree, +degree };
            };
            auto linearIntervalB = []( const qreal /*degree*/ ) -> QskIntervalF { return {}; };

            auto radialIntervalA = []( const qreal degree ) -> QskIntervalF {
                return { -degree, +degree };
            };
            auto radialIntervalB = []( const qreal degree ) -> QskIntervalF {
                return { 180 - degree, 180 + degree };
            };

            ( void ) new QskTextLabel( "Angles XYZ", right );
            (void) makeAngleSlider(Qt::XAxis, sensor, 0, 45, right);
            (void) makeAngleSlider(Qt::YAxis, sensor, 0, 45, right);
            (void) makeAngleSlider(Qt::ZAxis, sensor, 0, 45, right);

            ( void ) new QskTextLabel( "Tickmarks XXZ", right );
            auto* const sliderTickmarksX = makeTickmarksSlider(
                Qt::XAxis, sensor, 0, 90, linearIntervalA, linearIntervalB, right );
            auto* const sliderTickmarksY = makeTickmarksSlider(
                Qt::YAxis, sensor, 0, 90, linearIntervalA, linearIntervalB, right );
            auto* const sliderTickmarksZ = makeTickmarksSlider(
                Qt::ZAxis, sensor, 0, 90, radialIntervalA, radialIntervalB, right );

            ( void ) new QskTextLabel( "Rotation X Plane", right );
            ( void ) makeRotationSlider( Qt::XAxis, sensor, QskLevelingSensor::TickmarksX, right );
            ( void ) makeRotationSlider( Qt::YAxis, sensor, QskLevelingSensor::TickmarksX, right );
            ( void ) makeRotationSlider( Qt::ZAxis, sensor, QskLevelingSensor::TickmarksX, right );
            ( void ) new QskTextLabel( "Rotation Y Plane", right );
            ( void ) makeRotationSlider( Qt::XAxis, sensor, QskLevelingSensor::TickmarksY, right );
            ( void ) makeRotationSlider( Qt::YAxis, sensor, QskLevelingSensor::TickmarksY, right );
            ( void ) makeRotationSlider( Qt::ZAxis, sensor, QskLevelingSensor::TickmarksY, right );
            ( void ) new QskTextLabel( "Rotation Z Plane", right );
            ( void ) makeRotationSlider( Qt::XAxis, sensor, QskLevelingSensor::TickmarksZ, right );
            ( void ) makeRotationSlider( Qt::YAxis, sensor, QskLevelingSensor::TickmarksZ, right );
            ( void ) makeRotationSlider( Qt::ZAxis, sensor, QskLevelingSensor::TickmarksZ, right );
            ( void ) new QskTextLabel( "Horizon", right );
            ( void ) makeRotationSlider( Qt::XAxis, sensor, QskLevelingSensor::Horizon, right );
            ( void ) makeRotationSlider( Qt::YAxis, sensor, QskLevelingSensor::Horizon, right );
            ( void ) makeRotationSlider( Qt::ZAxis, sensor, QskLevelingSensor::Horizon, right );

            sliderTickmarksX->setValue( 15 );
            sliderTickmarksY->setValue( 15 );
            sliderTickmarksZ->setValue( 30 );
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
