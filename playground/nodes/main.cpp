/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include <SkinnyShapeProvider.h>
#include <SkinnyShortcut.h>

#include <QskWindow.h>
#include <QskObjectCounter.h>
#include <QskFocusIndicator.h>
#include <QskBox.h>
#include <QskLinearBox.h>
#include <QskSkinManager.h>
#include <QskAnimationHint.h>
#include <QskSkinTransition.h>
#include <QskSetup.h>
#include <QskSkin.h>
#include <QskSegmentedBar.h>
#include <QskSlider.h>
#include <QskTextLabel.h>

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

    qskSkinManager->setPluginPaths(
        { R"(C:\repositories\qskinny_rveh-install-win\plugins)" } );
    qskSetup->setItemUpdateFlag( QskQuickItem::PreferRasterForTextures, true );
    
    QskWindow window;
    window.resize( 800, 600 );

    auto* const layout = new QskLinearBox(Qt::Horizontal, window.contentItem() );
    auto* const left = new QskLinearBox(Qt::Vertical, layout);
    auto* const right = new QskLinearBox(Qt::Vertical, layout);
    auto* const control = new RadialNodes( left );
    auto* const skinlet = new RadialNodesSkinlet;
    {
        (void) new QskTextLabel("Tickmark Alignment", right);
        auto* const alignment = new QskSegmentedBar(right);
        alignment->setOptions( { "Center", "Bottom", "Top" } );
        QObject::connect(alignment, &QskSegmentedBar::selectedIndexChanged, control, [=](const int i){
                static const Qt::Alignment a[ 3 ]{ Qt::AlignVCenter, Qt::AlignBottom,
                    Qt::AlignTop };
                control->setAlignmentHint(RadialNodes::Lines, a[i]);
        });

        (void) new QskTextLabel("Tickmark Size", right);
        auto* const sliderW = new QskSlider(right);       
        auto* const sliderH = new QskSlider(right);        

        sliderW->setMinimum(1.0);
        sliderW->setMaximum(4.0);
        sliderW->setValue(1.0); 
        sliderH->setValue( 0.5 );

        auto updateStrutSizeHint = [=](const qreal){
            const auto width = sliderW->value();
            const auto height = sliderH->value() * control->height();
            control->setStrutSizeHint(RadialNodes::Lines, width, height );
        };

        QObject::connect( sliderW, &QskSlider::valueChanged, control, updateStrutSizeHint );
        QObject::connect( sliderH, &QskSlider::valueChanged, control, updateStrutSizeHint );        
    }
    control->setSkinlet(skinlet);
    skinlet->setOwnedBySkinnable( true );

    window.show();
    return app.exec();
}
