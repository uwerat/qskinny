/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "StackLayoutPage.h"
#include "ButtonBox.h"
#include "TestRectangle.h"

#include <QskAspect.h>
#include <QskPageIndicator.h>
#include <QskRgbValue.h>
#include <QskStackBox.h>
#include <QskStackBoxAnimator.h>
#include <QskTextLabel.h>

namespace
{
    class StackBox : public QskStackBox
    {
      public:
        StackBox( QQuickItem* parent = nullptr )
            : QskStackBox( parent )
        {
            setObjectName( "StackBox" );

            setBackgroundColor( Qt::white );

            setMargins( 30 );

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

      public:
        void incrementFading( int offset )
        {
            auto animator = dynamic_cast< QskStackBoxAnimator3* >( this->animator() );

            if ( animator == nullptr )
            {
                animator = new QskStackBoxAnimator3( this );
                animator->setEasingCurve( QEasingCurve::InQuad );
                animator->setDuration( 500 );
            }

            setAnimator( animator );
            setCurrentIndex( incrementedIndex( offset ) );
        }

        void incrementScrolling( Qt::Orientation orientation, int offset )
        {
            auto animator = dynamic_cast< QskStackBoxAnimator1* >( this->animator() );

            if ( animator == nullptr )
            {
                animator = new QskStackBoxAnimator1( this );
                animator->setDuration( 1000 );
            }

            animator->setOrientation( orientation );
            setAnimator( animator );

            setCurrentIndex( incrementedIndex( offset ) );
        }

      private:
        void addRectangle( const char* colorName )
        {
            auto rect = new TestRectangle( colorName );
            rect->setText( QString::number( itemCount() + 1 ) );
            addItem( rect, Qt::AlignCenter );
        }

        int incrementedIndex( int offset ) const
        {
            int index = currentIndex();
            if ( index < 0 )
                index = 0;

            int newIndex = ( index + offset ) % itemCount();
            if ( newIndex < 0 )
                newIndex += itemCount();

            return newIndex;
        }
    };
}

StackLayoutPage::StackLayoutPage( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    setObjectName( "StackLayoutPage" );

    setMargins( 10 );
    setBackgroundColor( QskRgbValue::LightSteelBlue );

    auto* box = new StackBox();

    auto* buttonBox = new ButtonBox();
    buttonBox->addButton( "<<", [ box ]() { box->incrementScrolling( Qt::Horizontal, +1 ); } );
    buttonBox->addButton( ">>", [ box ]() { box->incrementScrolling( Qt::Horizontal, -1 ); } );
    buttonBox->addButton( "^", [ box ]() { box->incrementScrolling( Qt::Vertical, -1 ); } );
    buttonBox->addButton( "v", [ box ]() { box->incrementScrolling( Qt::Vertical, +1 ); } );
    buttonBox->addButton( "Fading", [ box ]() { box->incrementFading( +1 ); } );

    auto pageIndicator = new QskPageIndicator();
    pageIndicator->setCount( box->itemCount() );
    pageIndicator->setCurrentIndex( box->currentIndex() );

    addItem( buttonBox, Qt::AlignTop | Qt::AlignLeft );
    addItem( pageIndicator, Qt::AlignCenter );
    addItem( box );

    connect( box, &QskStackBox::currentIndexChanged,
        pageIndicator, &QskPageIndicator::setCurrentIndex );
}
