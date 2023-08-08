/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include <SkinnyShapeProvider.h>
#include <SkinnyShortcut.h>

#include <QskAnimationHint.h>
#include <QskBox.h>
#include <QskFocusIndicator.h>
#include <QskLinearBox.h>
#include <QskObjectCounter.h>
#include <QskSegmentedBar.h>
#include <QskSetup.h>
#include <QskSkin.h>
#include <QskSkinManager.h>
#include <QskSkinTransition.h>
#include <QskSlider.h>
#include <QskTextLabel.h>
#include <QskWindow.h>

#include <QGuiApplication>

#include "RadialNodes.h"
#include "RadialNodesSkinlet.h"

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    qskSkinManager->setPluginPaths( { R"(C:\repositories\qskinny_rveh-install-win\plugins)" } );
    qskSetup->setItemUpdateFlag( QskQuickItem::PreferRasterForTextures, true );

    QskWindow window;
    window.resize( 800, 600 );

    auto* const layout = new QskLinearBox( Qt::Horizontal, window.contentItem() );
    auto* const left = new QskLinearBox( Qt::Vertical, layout );
    auto* const right = new QskLinearBox( Qt::Vertical, layout );
    auto* const control = new RadialTickmarks( left );
    auto* const skinlet = new RadialTickmarksSkinlet;
    {
        ( void ) new QskTextLabel( "Tickmark Alignment", right );
        auto* const alignment = new QskSegmentedBar( right );
        alignment->setOptions( { "Center", "Bottom", "Top" } );        
        QObject::connect(
            alignment, &QskSegmentedBar::selectedIndexChanged, control, [ = ]( const int i ) {
                static const Qt::Alignment a[ 3 ]{ Qt::AlignVCenter, Qt::AlignBottom,
                    Qt::AlignTop };
                control->setAlignmentHint( RadialTickmarks::Lines, a[ i ] );
            } );
        alignment->setSelectedIndex(0);

        ( void ) new QskTextLabel( "Tickmark Size W / H", right );
        auto* const sliderW = new QskSlider( right );
        auto* const sliderH = new QskSlider( right );

        sliderW->setMinimum( 1.0 );
        sliderW->setMaximum( 4.0 );
        sliderW->setValue( 1.0 );
        sliderH->setValue( 0.5 );

        auto updateStrutSizeHint = [ = ]( const qreal ) {
            const auto width = sliderW->value();
            const auto height = sliderH->value() * qMin(control->height(), control->width()) / 2;
            control->setStrutSizeHint( RadialTickmarks::Lines, width, height );
        };

        QObject::connect( sliderW, &QskSlider::valueChanged, control, updateStrutSizeHint );
        QObject::connect( sliderH, &QskSlider::valueChanged, control, updateStrutSizeHint );

        ( void ) new QskTextLabel( "Tickmarks [min,max]", right );

        auto* const tickmarksMin = new QskSlider( right );
        tickmarksMin->setMinimum( 0 );
        tickmarksMin->setMaximum( 360 );        

        auto* const tickmarksMax = new QskSlider( right );
        tickmarksMax->setMinimum( 0 );
        tickmarksMax->setMaximum( 360 );

        auto updateTickmark = [ control, tickmarksMin, tickmarksMax ]( const qreal v ) {
            QskScaleTickmarks tickmarks;
            QVector< qreal > major, medium, minor;

            for ( int deg = tickmarksMin->value(); deg < tickmarksMax->value(); deg += 1 )
            {
                if ( deg % 10 == 0 )
                    major << deg;
                else if ( deg % 5 == 0 )
                    medium << deg;
                else
                    minor << deg;
            }

            tickmarks.setMajorTicks( major );
            tickmarks.setMediumTicks( medium );
            tickmarks.setMinorTicks( minor );
            control->setTickmarks( tickmarks );
        };

        QObject::connect( tickmarksMin, &QskSlider::valueChanged, control, updateTickmark );
        QObject::connect( tickmarksMax, &QskSlider::valueChanged, control, updateTickmark );

        tickmarksMin->setValue( 0 );
        tickmarksMax->setValue( 270 );

    }
    control->setSkinlet( skinlet );
    skinlet->setOwnedBySkinnable( true );

    window.show();
    return app.exec();
}
