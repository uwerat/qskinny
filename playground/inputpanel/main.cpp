/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "LineEdit.h"

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskInputPanel.h>
#include <QskDialog.h>
#include <QskFocusIndicator.h>
#include <QskLinearBox.h>

#include <QskWindow.h>
#include <QskSetup.h>

#include <QskObjectCounter.h>

#include <QGuiApplication>

#define STRINGIFY(x) #x
#define STRING(x) STRINGIFY(x)

#define LOCAL_PANEL 1

class InputBox : public QskLinearBox
{
public:
    InputBox( QQuickItem* parentItem = nullptr ) :
        QskLinearBox( Qt::Vertical, parentItem )
    {
        setBackgroundColor( "PapayaWhip" );
        setDefaultAlignment( Qt::AlignHCenter | Qt::AlignTop );

        setMargins( 10 );
        setSpacing( 10 );

        auto* lineEdit = new LineEdit( this );

        lineEdit->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
        lineEdit->setBackgroundColor( Qt::white );
        lineEdit->setText( "I am a line edit. Press and edit Me." );

#if LOCAL_PANEL
        auto* inputPanel = new QskInputPanel( this );

        /*
            QskInputContext is connected to QskSetup::inputPanelChanged,
            making it the system input. If no input panel has been assigned
            QskInputContext would create a window or subwindow on the fly.
         */
        qskSetup->setInputPanel( inputPanel );
#endif
    }
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    qputenv( "QT_IM_MODULE", "skinny" );
    qputenv( "QT_PLUGIN_PATH", STRING( PLUGIN_PATH ) );

    QGuiApplication app( argc, argv );

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::Quit |
        SkinnyShortcut::DebugShortcuts );

#if !LOCAL_PANEL
    // We don't want to have a top level window.
    qskDialog->setPolicy( QskDialog::EmbeddedBox );
#endif

    QskWindow window;
    window.addItem( new InputBox() );
    window.addItem( new QskFocusIndicator() );

    window.resize( 800, 400 );
    window.show();

    return app.exec();
}
