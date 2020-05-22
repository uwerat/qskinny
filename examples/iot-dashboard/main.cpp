#include "MainWindow.h"

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskLinearBox.h>
#include <QskShortcutMap.h>
#include <QskPushButton.h>
#include <QskWindow.h>

#include <QGuiApplication>

int main( int argc, char* argv[] )
{
    QGuiApplication app( argc, argv );

    SkinnyFont::init( &app );

//    QskShortcutMap::addShortcut( QKeySequence( Qt::CTRL + Qt::Key_T ),
//        false, skinFactory, SLOT(toggleScheme()) );

//    QskShortcutMap::addShortcut( QKeySequence( Qt::CTRL + Qt::Key_S ),
//        false, skinFactory, SLOT(rotateSkin()) );

    // With CTRL-B you can rotate a couple of visual debug modes
    SkinnyShortcut::enable( SkinnyShortcut::DebugBackground |
        SkinnyShortcut::DebugStatistics | SkinnyShortcut::Quit );

    MainWindow window;
    window.show();

    return app.exec();
}
