/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "GridLayoutPage.h"
#include "TestRectangle.h"

#include <QskGridBox.h>
#include <QskRgbValue.h>

namespace
{
    class Box : public QskGridBox
    {
      public:
        Box( QQuickItem* parent = nullptr )
            : QskGridBox( parent )
        {
            setObjectName( "GridBox" );

            setBackgroundColor( Qt::white );
            setMargins( 10 );

            addItem( new TestRectangle( "PaleVioletRed" ), 0, 0, 1, 2 );
            addItem( new TestRectangle( "DarkSeaGreen" ), 1, 0, 2, 1 );
            addItem( new TestRectangle( "SkyBlue" ), 2, 1, 1, 1 );
            addItem( new TestRectangle( "NavajoWhite" ), 0, 2, 4, 1 );
        }

        void mirror()
        {
            setLayoutMirroring( !layoutMirroring() );
        }
    };
}

GridLayoutPage::GridLayoutPage( QQuickItem* parent )
    : QskControl( parent )
{
#if 1
    setMargins( 10 );
    setBackgroundColor( QskRgbValue::LightSteelBlue );
#endif

    setAutoLayoutChildren( true );
    new Box( this );
}
