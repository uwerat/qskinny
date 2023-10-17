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
            setAnimationHint( faderAspect(), 1000 );
#endif

            setEdge( edge );
            setOverlay( false );

            auto content = new QskControl( this );
            content->setObjectName( "Content" );

            switch( edge )
            {
                case Qt::LeftEdge:
                    content->setBackgroundColor( QskRgb::Tomato );
                    content->setFixedWidth( 100 );
                    break;

                case Qt::RightEdge:
                    content->setFixedWidth( 200 );
                    content->setBackgroundColor( QskRgb::Orchid );
                    break;

                case Qt::TopEdge:
                    content->setFixedHeight( 100 );
                    content->setBackgroundColor( QskRgb::Chartreuse );
                    break;

                case Qt::BottomEdge:
                    content->setFixedHeight( 200 );
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

            setMargins( 10 );
            setAutoLayoutChildren( true );

            for ( int i = 0; i < 4; i++ )
            {
                const auto edge = static_cast< Qt::Edge >( 1 << i );
                m_drawers[i] = new Drawer( edge, this );

                auto dragMargin = 30; // the default setting is pretty small
                if ( edge == Qt::TopEdge )
                {
                    // to check if dragging works above the button
                    dragMargin = 120;
                }

                m_drawers[i]->setDragMargin( dragMargin );
            }

            auto button = new QskPushButton( "Push Me", this );
            button->setPreferredHeight( 100 );
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
