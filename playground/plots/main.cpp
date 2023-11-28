/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Plot.h"

#include <QskWindow.h>
#include <QskMainView.h>
#include <QskLinearBox.h>
#include <QskPushButton.h>
#include <QskObjectCounter.h>

#include <SkinnyShortcut.h>

#include <QGuiApplication>

namespace
{
    class TestPlot : public Plot
    {
      public:
        TestPlot( QQuickItem* parentItem = nullptr )
            : Plot( parentItem )
        {
            setMargins( 5 );

            QVector< Sample > samples;

            samples += { -50, 30, 60, 80 };
            samples += { -45, 35, 55, 85 };
            samples += { -40, 32, 60, 77 };
            samples += { -35, 38, 75, 66 };
            samples += { -30, 40, 65, 75 };
            samples += { -25, 48, 58, 82 };
            samples += { -20, 27, 62, 85 };
            samples += { -15, 32, 22, 70 };
            samples += { -10, 30, 24, 77 };
            samples += { -5, 20, 33, 70 };
            samples += { 0, 40, 60, 80 };

            setSamples( samples );
        }
    };

    class Header : public QskLinearBox
    {
        Q_OBJECT

      public:
        Header( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, parent )
        {
            initSizePolicy( QskSizePolicy::Ignored, QskSizePolicy::Fixed );

            setPanel( true );
            setPaddingHint( QskBox::Panel, 5 );

            addStretch( 1 );

            auto buttonLeft = new QskPushButton( "<", this );
            buttonLeft->setAutoRepeat( true );
            connect( buttonLeft, &QskPushButton::clicked,
                this, [this] { Q_EMIT shiftClicked( +1 ); } );

            auto buttonRight = new QskPushButton( ">", this );
            buttonRight->setAutoRepeat( true );
            connect( buttonRight, &QskPushButton::clicked,
                this, [this] { Q_EMIT shiftClicked( -1 ); } );

            auto buttonReset = new QskPushButton( "Reset", this );
            connect( buttonReset, &QskPushButton::clicked,
                this, &Header::resetClicked );
        }

      Q_SIGNALS:
        void shiftClicked( int steps );
        void resetClicked();
    };

    class MainView : public QskMainView
    {
      public:
        MainView( QQuickItem* parent = nullptr )
            : QskMainView( parent )
        {
            auto header = new Header( this );
            auto plot = new TestPlot();

            connect( header, &Header::resetClicked,
                plot, &Plot::resetAxes );

            connect( header, &Header::shiftClicked,
                plot, &Plot::shiftXAxis );

            setHeader( header );
            setBody( plot );
        }
    };
}

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    QskWindow window;
    window.addItem( new MainView() );
    window.resize( 800, 600 );
    window.show();

    return app.exec();
}

#include "main.moc"
