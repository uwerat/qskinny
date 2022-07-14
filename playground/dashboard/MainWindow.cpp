/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MainWindow.h"
#include "ButtonBar.h"
#include "SkinFactory.h"
#include "SpeedometerDisplay.h"

#include <QskGraphic.h>
#include <QskGraphicIO.h>
#include <QskGraphicLabel.h>
#include <QskLinearBox.h>
#include <QskTextLabel.h>

#include <QDate>
#include <QImage>

namespace
{
    class Header : public ButtonBar
    {
      public:
        Header()
        {
            addIndicator( "bluetooth" );
            addIndicator( "location" );
            addIndicator( "phone" );

            ( void ) new QskTextLabel( QDate::currentDate().toString(), this );

            addIndicator( "user" );
            addIndicator( "bookmark" );
            addIndicator( "menu" );
        }
    };

    class Footer : public ButtonBar
    {
      public:
        Footer()
        {
            addIndicator( "cloud" );
            addIndicator( "man" );
            addIndicator( "bus" );
            addIndicator( "plane" );
            addIndicator( "train" );

            addStretch( 10 );
        }
    };
}

MainWindow::MainWindow()
{
    setAutoLayoutChildren( true );

    const QImage image( QStringLiteral( ":/images/background.jpg" ) );

    auto backgroundImage = new QskGraphicLabel( contentItem() );
    backgroundImage->setGraphic( QskGraphic::fromImage( image ) );
    backgroundImage->setFillMode( QskGraphicLabel::Stretch );

    auto header = new Header();
    auto speedoDisplay = new SpeedometerDisplay();
    auto footer = new Footer();

    auto layout = new QskLinearBox( Qt::Vertical, contentItem() );

    layout->addItem( header );
    layout->setStretchFactor( header, 1 );

    layout->addItem( speedoDisplay );
    layout->setStretchFactor( speedoDisplay, 8 );

    layout->addItem( footer );
    layout->setStretchFactor( footer, 1 );
}
