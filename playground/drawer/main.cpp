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
    inline qreal distanceToEdge(
        const QRectF& rect, const QPointF& pos, int edge )
    {
        qreal dt = 10e6;
        switch( edge )
        {
            case Qt::TopEdge:
                dt = pos.y() - rect.top();
                break;

            case Qt::BottomEdge:
                dt = rect.bottom() - pos.y();
                break;

            case Qt::LeftEdge:
                dt = pos.x() - rect.left();
                break;

            case Qt::RightEdge:
                dt = rect.right() - pos.x();
                break;
        }

        return std::abs( dt );
    }

    class Drawer : public QskDrawer
    {
      public:
        Drawer( Qt::Edge edge,  QQuickItem* parent )
            : QskDrawer( parent )
        {
#if 1
            setAnimationHint( Panel | QskAspect::Position, 2000 );
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

            setAcceptedMouseButtons( Qt::LeftButton );
        }

      protected:
        virtual bool contains( const QPointF& pos ) const
        {
            if ( auto control = qskControlCast( parentItem() ) )
            {
                // we want to catch clicks on the margins of the parent

                auto r = rect();
                if ( !r.contains( pos ) )
                {
                    r = r.marginsAdded( control->margins() );
                    return r.contains( pos );
                }

                return false;
            }

            return QskControl::contains( pos );
        }

        void mousePressEvent( QMouseEvent* event ) override
        {
            const auto pos = qskMousePosition( event );

            int drawerIndex = -1;
            qreal minDist = 10e6;

            for ( int i = 0; i < 4; i++ )
            {
                const auto edge = static_cast< Qt::Edge >( 1 << i );
                const auto dist = distanceToEdge( rect(), pos, edge );

                if ( dist < minDist )
                {
                    minDist = dist;
                    drawerIndex = i;
                }
            }

            if ( drawerIndex >= 0 )
                m_drawers[drawerIndex]->open();
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
