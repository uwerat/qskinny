/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskAspect.h>
#include <QskModule.h>
#include <QskObjectCounter.h>
#include <QskSkin.h>
#include <QskLinearBox.h>
#include <QskTextLabel.h>
#include <QskWindow.h>

#include <QGuiApplication>
#include <QFont>

class TextBox: public QskLinearBox
{
public:
    TextBox( QQuickItem* parent = nullptr ):
        QskLinearBox( Qt::Vertical, 3, parent )
    {
        const QStringList texts = { "Default", "Tiny", "Small",
            "Medium", "Large", "Huge" };

        for ( int i = 0; i < texts.size(); i++ )
        {
            auto label = new QskTextLabel( texts[i] + " Font", this );
            label->setFontRole( QskTextLabel::Text, i );
        }
    }
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::Quit |
        SkinnyShortcut::DebugShortcuts );

    auto box = new TextBox();

    QskWindow window;
    window.addItem( box );
    window.resize( box->sizeHint().toSize() );

    window.show();

    return app.exec();
}
