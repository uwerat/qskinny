/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "SwipeViewPage.h"
#include "TestRectangle.h"

#include <QskRgbValue.h>
#include <QskSwipeView.h>

namespace
{
class SwipeView : public QskSwipeView
    {
        Q_OBJECT

      public:
        SwipeView( QQuickItem* parent = nullptr )
            : QskSwipeView( parent )
        {
            setObjectName( "SwipeView" );

            setBackgroundColor( Qt::white );
            setDefaultAlignment( Qt::AlignCenter );
            setOrientation( Qt::Horizontal );

            addRectangle( "Gold" );
            addRectangle( "SeaGreen" );
            addRectangle( "SlateBlue" );
            addRectangle( "Peru" );

            for ( int i = 0; i < itemCount(); i += 2 )
            {
                if ( auto control = qskControlCast( itemAtIndex( i ) ) )
                    control->setFixedSize( 200, 200 );
            }
        }
      private:
        void addRectangle( const char* colorName )
        {
            auto rect = new TestRectangle( colorName );
            rect->setText( QString::number( itemCount() + 1 ) );
            addItem( rect );
        }

    };
}

SwipeViewPage::SwipeViewPage( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    setObjectName( "SwipeViewPage" );

    setMargins( 10 );
    setBackgroundColor( QskRgb::LightSteelBlue );

    auto swipeView = new SwipeView();
    addItem( swipeView );
}

#include "SwipeViewPage.moc"
