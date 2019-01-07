/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskWindow.h>
#include <QskObjectCounter.h>

#include <QGuiApplication>
#include <QUrl>

#include <private/qquickwebengineview_p.h>

class WebView : public QQuickWebEngineView
{
  public:
    WebView( const char* url, QQuickItem* parent = nullptr )
        : WebView( parent )
    {
        setUrl( QUrl( url ) );
    }

    WebView( QQuickItem* parent = nullptr )
        : QQuickWebEngineView( parent )
    {
    }
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app( argc, argv );

    QtWebEngine::initialize();

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::Quit | SkinnyShortcut::DebugShortcuts );

    QskWindow window;

    window.addItem( new WebView( "https://github.com/uwerat/qskinny" ) );
    window.resize( 800, 600 );
    window.show();

    return app.exec();
}
