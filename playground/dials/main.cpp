/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "Dashboard.h"
#include "Skin.h"

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

    QGuiApplication app( argc, argv );
    qskSkinManager->setSkin( new Skin() );

    SkinnyShortcut::enable( SkinnyShortcut::DebugBackground |
        SkinnyShortcut::DebugStatistics | SkinnyShortcut::Quit );

    Window window;
    window.show();

    return app.exec();
}
