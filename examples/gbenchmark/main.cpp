/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Benchmark.h"

#include <QskLinearBox.h>
#include <QskPushButton.h>
#include <QskWindow.h>

#include <QCommandLineParser>
#include <QGuiApplication>

class Button : public QskPushButton
{
  public:
    Button( const QString& testDir )
        : m_testDir( testDir )
    {
        setText( QString( "Run: " ) + testDir );
        setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
    }

    void run()
    {
        Benchmark::run( m_testDir );
    }

  private:
    QString m_testDir;
};

int main( int argc, char* argv[] )
{
    QGuiApplication app( argc, argv );

    QCommandLineParser parser;
    parser.setApplicationDescription( "Benchmark for creating textures from SVGs" );
    parser.addHelpOption();
    parser.addPositionalArgument( "svgdir", "Directory with SVG files.", "[pathname]" );

    parser.process( app );

    const QStringList args = parser.positionalArguments();

    if ( args.count() != 1 )
        parser.showHelp( 1 );

    auto button = new Button( args[ 0 ] );
    button->setLayoutAlignmentHint( Qt::AlignCenter );
    QObject::connect( button, &Button::clicked, button, &Button::run );

    auto box = new QskLinearBox();
    box->addItem( button );

    QskWindow window;
    window.setColor( Qt::white );
    window.resize( 400, 400 );
    window.addItem( box );

    window.show();

    return app.exec();
}
