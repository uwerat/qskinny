/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "FlowLayoutPage.h"
#include "ButtonBox.h"
#include "TestRectangle.h"

#include <QskAspect.h>
#include <QskLinearBox.h>
#include <QskRgbValue.h>
#include <QskTextLabel.h>

namespace
{
    class Box : public QskLinearBox
    {
      public:
        Box( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, 5, parent )
        {
            setObjectName( "Box" );

            setBackgroundColor( Qt::white );

            setMargins( 10 );
            setSpacing( 5 );

            setDefaultAlignment( Qt::AlignCenter );

            addRectangle( "LightSteelBlue" );
            addRectangle( "PowderBlue" );
            addRectangle( "LightBlue" );
            addRectangle( "SkyBlue" );
            addRectangle( "LightSkyBlue" );
            addRectangle( "DeepSkyBlue" );
            addRectangle( "DodgerBlue" );
            addRectangle( "CornflowerBlue" );
            addRectangle( "SteelBlue" );
            addRectangle( "RoyalBlue" );
            addRectangle( "Blue" );
            addRectangle( "MediumBlue" );
            addRectangle( "DarkBlue" );
            addRectangle( "Navy" );
            addRectangle( "MidnightBlue" );
        }

        void mirror()
        {
            setLayoutMirroring( !layoutMirroring() );
        }

        void rotate()
        {
            const int index = 0;

            auto item = itemAtIndex( index );
            removeAt( index );

            if ( item == nullptr )
            {
                // how to get the spacer item and its settings ???
                addSpacer( 30 );
            }
            else
            {
                addItem( item );
            }
        }

        void incrementDimension( int count )
        {
            setDimension( dimension() + count );
        }

      private:
        void addRectangle( const char* colorName )
        {
            auto rect = new TestRectangle( colorName );
            rect->setText( QString::number( elementCount() + 1 ) );

            addItem( rect );
        }
    };
}

FlowLayoutPage::FlowLayoutPage( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    setMargins( 10 );
    setBackgroundColor( QskRgb::LightSteelBlue );

    auto box = new Box();

    auto buttonBox = new ButtonBox();
    buttonBox->setLayoutAlignmentHint( Qt::AlignTop | Qt::AlignLeft );
    buttonBox->addButton( "Flip", [ box ]() { box->transpose(); } );
    buttonBox->addButton( "Mirror", [ box ]() { box->mirror(); } );
    buttonBox->addButton( "Rotate", [ box ]() { box->rotate(); } );
    buttonBox->addButton( "Dim+", [ box ]() { box->incrementDimension( +1 ); } );
    buttonBox->addButton( "Dim-", [ box ]() { box->incrementDimension( -1 ); } );

    addItem( buttonBox );
    addItem( box );
}
