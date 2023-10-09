/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "SkinnyShortcut.h"

#include <QskRgbValue.h>
#include <QskControl.h>
#include <QskDrawer.h>
#include <QskPushButton.h>
#include <QskWindow.h>
#include <QskEvent.h>
#include <QskAnimationHint.h>

#include <QGuiApplication>

namespace
{
    class Drawer : public QskDrawer
    {
      public:
        Drawer( Qt::Edge edge,  QQuickItem* parent )
            : QskDrawer( parent )
        {
#if 1
            setAnimationHint( Panel | QskAspect::Position, 1000 );
#endif

            setEdge( edge );
            setOverlay( true );

            auto content = new QskControl( this );

            switch( edge )
            {
                case Qt::LeftEdge:
                    content->setBackgroundColor( QskRgb::Tomato );
                    setFixedWidth( 100 );
                    break;

                case Qt::RightEdge:
                    setFixedWidth( 200 );
                    content->setBackgroundColor( QskRgb::Orchid );
                    break;

                case Qt::TopEdge:
                    setFixedHeight( 100 );
                    content->setBackgroundColor( QskRgb::Wheat );
                    break;

                case Qt::BottomEdge:
                    setFixedHeight( 200 );
                    content->setBackgroundColor( QskRgb::Wheat );
                    break;
            }
        }
    };

    class DrawerBox : public QskControl
    {
      public:
        DrawerBox( QQuickItem* parent = nullptr )
            : QskControl( parent )
        {
            setBackgroundColor( QskRgb::LightSteelBlue );

            setMargins( 50 );
            setAutoLayoutChildren( true );

            (void) new QskPushButton( this );

            for ( int i = 0; i < 4; i++ )
            {
                const auto edge = static_cast< Qt::Edge >( 1 << i );
                m_drawers[i] = new Drawer( edge, this );
            }
        }

      private:
        Drawer* m_drawers[4];
    };

    class MainBox : public QskControl
    {
      public:
        MainBox( QQuickItem* parent = nullptr )
            : QskControl( parent )
        {
            setBackgroundColor( QskRgb::LemonChiffon );
            setMargins( 40 );
            setAutoLayoutChildren( true );

            ( void ) new DrawerBox( this );
        }
    };
}

int main( int argc, char* argv[] )
{
    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    QskWindow window;
    window.addItem( new MainBox() );
    window.resize( 600, 600 );
    window.show();

    return app.exec();
}
