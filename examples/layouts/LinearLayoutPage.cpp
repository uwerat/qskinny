/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "LinearLayoutPage.h"
#include "ButtonBox.h"
#include "TestRectangle.h"

#include <QskAspect.h>
#include <QskLinearBox.h>
#include <QskRgbValue.h>

namespace
{
    class Box : public QskLinearBox
    {
      public:
        Box( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, parent )
        {
            setObjectName( "Box" );

            setBackgroundColor( Qt::white );

            setMargins( 10 );
            setSpacing( 5 );

            addRectangle( "LightSalmon" );
            addRectangle( "Salmon" );
            addRectangle( "DarkSalmon" );
            addRectangle( "LightCoral" );
            addRectangle( "IndianRed" );
            addRectangle( "Crimson" );
            addRectangle( "FireBrick" );
            addRectangle( "DarkRed" );

            insertSpacer( 5, 30 );
        }

        void mirror()
        {
            setLayoutMirroring( !layoutMirroring() );
        }

        void rotate()
        {
            const int index = 0;

            if ( auto item = itemAtIndex( index ) )
            {
                removeAt( index );
                addItem( item );
            }
            else
            {
                const auto spacing = spacingAtIndex( index );
                removeAt( index );
                addSpacer( spacing );
            }
        }

        void incrementSpacing( int spacing )
        {
            setSpacing( this->spacing() + spacing );
        }

      private:
        void addRectangle( const char* colorName )
        {
            auto rect = new TestRectangle( colorName );
            rect->setText( QString::number( count() + 1 ) );

            addItem( rect, Qt::AlignCenter );
        }
    };
}

LinearLayoutPage::LinearLayoutPage( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    setMargins( 10 );
    setBackgroundColor( QskRgbValue::LightSteelBlue );

    auto box = new Box();

    auto buttonBox = new ButtonBox();
    buttonBox->addButton( "Flip", [ box ]() { box->transpose(); } );
    buttonBox->addButton( "Mirror", [ box ]() { box->mirror(); } );
    buttonBox->addButton( "Rotate", [ box ]() { box->rotate(); } );
    buttonBox->addButton( "Spacing+", [ box ]() { box->incrementSpacing( +1 ); }, true );
    buttonBox->addButton( "Spacing-", [ box ]() { box->incrementSpacing( -1 ); }, true );

    addItem( buttonBox, Qt::AlignTop | Qt::AlignLeft );
    addItem( box );
}
