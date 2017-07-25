#include "MainWindow.h"
#include "SkinFactory.h"

#include <QskShortcut.h>
#include <QskSetup.h>
#include <SkinnyShortcut.h>

#include <QGuiApplication>

int main( int argc, char** argv )
{
    QGuiApplication app( argc, argv );

    SkinFactory skinFactory;
    Qsk::registerSkinFactory( "SampleSkinFactory", &skinFactory );

    qskSetup->setSkin( "DefaultSkin" );

    SkinnyShortcut::enable( SkinnyShortcut::DebugBackground | SkinnyShortcut::Quit );

    QskShortcut::addShortcut( QKeySequence( Qt::CTRL + Qt::Key_T ),
        &skinFactory, SLOT( toggleScheme()), false );

    QskShortcut::addShortcut( QKeySequence( Qt::CTRL + Qt::Key_S ),
        &skinFactory, SLOT( rotateSkin()), false );

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
