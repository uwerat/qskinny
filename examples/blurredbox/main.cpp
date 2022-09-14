#include <SkinnyShortcut.h>

#include <QskObjectCounter.h>
#include <QskSetup.h>
#include <QskShortcutMap.h>
#include <QskSkinFactory.h>
#include <QskSkinManager.h>
#include <QskWindow.h>

#include <QGuiApplication>

#include "MainWindow.h"

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    // disable default skins
    qskSkinManager->setPluginPaths( QStringList() ); // no plugins

    // With CTRL-B you can rotate a couple of visual debug modes
    SkinnyShortcut::enable( SkinnyShortcut::RotateSkin | SkinnyShortcut::DebugBackground |
                            SkinnyShortcut::DebugStatistics | SkinnyShortcut::Quit );

    MainWindow window;
    window.show();

    return app.exec();
}
