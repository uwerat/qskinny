/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskLinearBox.h>
#include <QskObjectCounter.h>
#include <QskTextLabel.h>
#include <QskWindow.h>

#include <QGuiApplication>

class TextBox : public QskLinearBox
{
  public:
    TextBox( QQuickItem* parent = nullptr )
        : QskLinearBox( Qt::Vertical, 3, parent )
    {
        setMargins( 10 );
        setDefaultAlignment( Qt::AlignTop );
        setExtraSpacingAt( Qt::BottomEdge );

        const QStringList texts =
            { "Default", "Tiny", "Small", "Medium", "Large", "Huge" };

        for ( int i = 0; i < texts.size(); i++ )
        {
            auto label = new QskTextLabel( texts[ i ] + " Font", this );
            label->setPanel( true );
            label->setFontRole( i );
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
    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    auto box = new TextBox();

    QskWindow window;
    window.setColor( Qt::lightGray );
    window.addItem( box );
    window.resize( box->sizeHint().toSize() );

    window.show();

    return app.exec();
}
