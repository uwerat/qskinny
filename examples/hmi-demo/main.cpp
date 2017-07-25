#include "MainWindow.h"
#include "DefaultSkin.h"
#include "OtherSkin.h"

#include <QskShortcut.h>
#include <QskSkinFactory.h>
#include <QskSetup.h>

#include <SkinnyShortcut.h>

#include <QGuiApplication>

namespace {

    class SkinFactory : public QskSkinFactory
    {
        Q_OBJECT

public:
        SkinFactory(): QskSkinFactory()
        {
        }

        virtual QStringList skinNames() const override final
        {
            return { "DefaultSkin", "OtherSkin" };
        }

        virtual QskSkin* createSkin( const QString& skinName ) override
        {
            if ( skinName == "DefaultSkin" )
                return new DefaultSkin( skinName );

            if ( skinName == "OtherSkin" )
                return new OtherSkin( skinName );

            return nullptr;
        }

public Q_SLOTS:
        void toggleScheme()
        {
            DefaultSkin* skin = static_cast< DefaultSkin* >( qskSetup->skin() );
            if ( skin )
                skin->toggleScheme();
        }
    };

}

int main( int argc, char** argv )
{
    QGuiApplication app( argc, argv );

    SkinFactory skinFactory;
    Qsk::registerSkinFactory( "SampleSkinFactory", &skinFactory );
    QskSetup::instance()->setSkin( "DefaultSkin" );

    SkinnyShortcut::enable( SkinnyShortcut::DebugBackground | SkinnyShortcut::Quit | SkinnyShortcut::RotateSkin );

    QskShortcut::addShortcut( QKeySequence( Qt::CTRL + Qt::Key_T ), &skinFactory,
        SLOT(toggleScheme()), false );

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}

#include "main.moc"
