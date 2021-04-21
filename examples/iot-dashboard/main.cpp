/****************************************************************************
**
** Copyright 2021 Edelhirsch Software GmbH. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of the copyright holder nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/

#include "MainWindow.h"

#include "Skin.h"

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


namespace
{
    class SkinFactory : public QskSkinFactory
    {
            Q_OBJECT

        public:
            SkinFactory( QObject* parent = nullptr ) : QskSkinFactory( parent )
            {
            }

            QStringList skinNames() const override
            {
                return { "DaytimeSkin", "NighttimeSkin" };
            }

            QskSkin* createSkin( const QString& skinName ) override
            {
                if( skinName == "DaytimeSkin" )
                {
                    return new DaytimeSkin;
                }

                if( skinName == "NighttimeSkin" )
                {
                    return new NighttimeSkin;
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

    qskSkinManager->unregisterFactory( "materialfactory" );
    qskSkinManager->unregisterFactory( "squiekfactory" );
    qskSkinManager->registerFactory(
        QStringLiteral( "SampleSkinFactory" ), &skinFactory );

//    QskShortcutMap::addShortcut( QKeySequence( Qt::CTRL + Qt::Key_T ),
//        false, skinFactory, SLOT(toggleScheme()) );

//    QskShortcutMap::addShortcut( QKeySequence( Qt::CTRL + Qt::Key_S ),
//                                 true, &skinFactory, SLOT( rotateSkin() ) );

    qskSetup->setSkin( "DaytimeSkin" );

    // With CTRL-B you can rotate a couple of visual debug modes
    SkinnyShortcut::enable( SkinnyShortcut::RotateSkin | SkinnyShortcut::DebugBackground |
                            SkinnyShortcut::DebugStatistics | SkinnyShortcut::Quit );

    MainWindow window;
    window.show();

    return app.exec();
}

#include "main.moc"
