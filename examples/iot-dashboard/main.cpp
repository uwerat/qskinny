#include "MainWindow.h"

#include "DaytimeSkin.h"

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskLinearBox.h>
#include <QskPushButton.h>
#include <QskSetup.h>
#include <QskShortcutMap.h>
#include <QskSkinFactory.h>
#include <QskSkinManager.h>
#include <QskWindow.h>

#include <QGuiApplication>


namespace {
    class SkinFactory : public QskSkinFactory
    {
        Q_OBJECT

      public:
        SkinFactory( QObject* parent = nullptr ) : QskSkinFactory( parent )
        {
        }

        QStringList skinNames() const override
        {
            return { "DaytimeSkin" };
        }
        QskSkin* createSkin( const QString& skinName ) override
        {
            if( skinName == "DaytimeSkin" )
            {
                return new DaytimeSkin;
            }

            return nullptr;
        }
    };
}

int main( int argc, char* argv[] )
{
    QGuiApplication app( argc, argv );

    SkinnyFont::init( &app );

    SkinFactory skinFactory;

    qskSkinManager->setPluginPaths( QStringList() ); // no plugins
    qskSkinManager->registerFactory(
        QStringLiteral( "SampleSkinFactory" ), &skinFactory );

//    QskShortcutMap::addShortcut( QKeySequence( Qt::CTRL + Qt::Key_T ),
//        false, skinFactory, SLOT(toggleScheme()) );

//    QskShortcutMap::addShortcut( QKeySequence( Qt::CTRL + Qt::Key_S ),
//        false, skinFactory, SLOT(rotateSkin()) );

    qskSetup->setSkin( "DaytimeSkin" );

    // With CTRL-B you can rotate a couple of visual debug modes
    SkinnyShortcut::enable( SkinnyShortcut::DebugBackground |
        SkinnyShortcut::DebugStatistics | SkinnyShortcut::Quit );

    MainWindow window;
    window.show();

    return app.exec();
}

#include "main.moc"
