/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Dashboard.h"
#include "SkinFactory.h"

#include <SkinnyShortcut.h>

#include <QskWindow.h>
#include <QskSkinManager.h>
#include <QskObjectCounter.h>

#include <QskGraphic.h>
#include <QskGraphicIO.h>
#include <QskGraphicLabel.h>

#include <QGuiApplication>

namespace
{
    class Window : public QskWindow
    {
      public:
        Window()
        { 
            setAutoLayoutChildren( true );

            const QImage image( QStringLiteral( ":/images/background.jpg" ) );

            auto backgroundImage = new QskGraphicLabel( contentItem() );
            backgroundImage->setGraphic( QskGraphic::fromImage( image ) );
            backgroundImage->setFillMode( QskGraphicLabel::Stretch );

            (void ) new Dashboard( contentItem() );
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
    window.show();

    return app.exec();
}
