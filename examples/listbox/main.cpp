/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskAspect.h>
#include <QskObjectCounter.h>
#include <QskSimpleListBox.h>
#include <QskWindow.h>
#include <QskFunctions.h>

#include <QFontMetricsF>
#include <QGuiApplication>

class ListBox : public QskSimpleListBox
{
  public:
    ListBox()
    {
        setMargins( QMarginsF( 15, 10, 10, 10 ) );
        setAlternatingRowColors( true );

        // increasing the padding of each row: usually the job of the skin !
        setPaddingHint( Cell, QMargins( 10, 20, 10, 20 ) );

        populate();

        setSelectedRow( 5 );
    }

  private:
    void populate()
    {
        const QString format( "Row %1: The quick brown fox jumps over the lazy dog" );
        QStringList entries;
        for ( int i = 0; i < 10000; i++ )
        {
            entries += format.arg( i + 1 );
        }

        // we know, that the last entry is the longest one and
        // can prevent the list box from having to find it out
        // the expensive way.

        const qreal maxWidth = qskHorizontalAdvance( effectiveFont( Cell ), entries.last() );
        setColumnWidthHint( 0, maxWidth );

        append( entries );
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

    QskWindow window;
    window.setColor( "Silver" );

    window.addItem( new ListBox() );
    window.resize( 400, 600 );
    window.show();

    return app.exec();
}
