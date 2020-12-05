/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "ProgressBarPage.h"
#include <QskProgressBar.h>
#include <QskGraphicProvider.h>
#include <QskGraphic.h>
#include <QskGradient.h>
#include <QskRgbPalette.h>
#include <QskRgbValue.h>

namespace
{
    class ProgressBar : public QskProgressBar
    {
      public:
        ProgressBar( QskLinearBox* box )
            : QskProgressBar( box )
        {
            setOrientation( ( box->orientation() == Qt::Horizontal )
                ? Qt::Vertical : Qt::Horizontal );

            setBoundaries( 0, 100 );
        }

        void setTheme( QskRgbPalette::Theme theme )
        {
            const auto pal = QskRgbPalette::palette( theme );

            QVector< QRgb > rgb;
            rgb += pal.rgb( QskRgbPalette::W200 );
            rgb += pal.rgb( QskRgbPalette::W400 );
            rgb += pal.rgb( QskRgbPalette::W600 );
            rgb += pal.rgb( QskRgbPalette::W900 );

            const auto stops = QskRgbPalette::colorStops( rgb, true );

            setBarGradient( QskGradient( orientation(), stops ) );
        }
    };
}

ProgressBarPage::ProgressBarPage( QQuickItem* parent )
    : Page( Qt::Horizontal, parent )
{
    setGradient( QskRgb::AliceBlue );
    setSpacing( 40 );

    populate();
}

void ProgressBarPage::populate()
{
    auto hBox = new QskLinearBox( Qt::Horizontal, this );
    hBox->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
    hBox->setSpacing( 20 );

    {
        auto bar = new ProgressBar( hBox );
        bar->setValue( 35 );
    }

    {
        auto bar = new ProgressBar( hBox );
        bar->setTheme( QskRgbPalette::BlueGrey );
        bar->setValue( 100 );
    }

    {
        auto bar = new ProgressBar( hBox );
        bar->setTheme( QskRgbPalette::Blue );
        bar->setValue( 75 );
    }

    {
        auto bar = new ProgressBar( hBox );
        bar->setTheme( QskRgbPalette::Blue );
        bar->setOrigin( 60 );
        bar->setValue( 25 );
    }

    {
        auto bar = new ProgressBar( hBox );
        bar->setIndeterminate( true );
    }

    auto vBox = new QskLinearBox( Qt::Vertical, this );
    vBox->setSpacing( 20 );
    vBox->setExtraSpacingAt( Qt::BottomEdge );

    {
        auto bar = new ProgressBar( vBox );
        bar->setTheme( QskRgbPalette::DeepOrange );
        bar->setValue( 100 );
    }

    {
        auto bar = new ProgressBar( vBox );
        bar->setTheme( QskRgbPalette::Pink );
        bar->setMaximum( 40 );
        bar->setValue( 25 );
    }

    {
        auto bar = new ProgressBar( vBox );
        bar->setTheme( QskRgbPalette::Pink );
        bar->setOrigin( 40 );
        bar->setValue( 10 );
    }

    {
        auto bar = new ProgressBar( vBox );
        bar->setIndeterminate( true );
    }
}
