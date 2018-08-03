/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "DynamicConstraintsPage.h"

#include <QskAspect.h>
#include <QskControl.h>
#include <QskLinearBox.h>
#include <QskPushButton.h>
#include <QskRgbValue.h>

namespace
{
    class Control final : public QskControl
    {
      public:
        Control( const char* colorName, QQuickItem* parent = nullptr );
        Control( const char* colorName, qreal aspectRatio, QQuickItem* parent = nullptr );

        qreal heightForWidth( qreal width ) const override;
        qreal widthForHeight( qreal height ) const override;

        void transpose();

      private:
        qreal m_aspectRatio;
    };

    class Box : public QskLinearBox
    {
      public:
        Box( QQuickItem* parent = nullptr );

        void flip();

      private:
        void addControl( Control* );
    };
}

Control::Control( const char* colorName, QQuickItem* parent )
    : QskControl( parent )
    , m_aspectRatio( 1.0 )
{
    setObjectName( colorName );

    setBackgroundColor( colorName );

    setSizePolicy( QskSizePolicy::MinimumExpanding, QskSizePolicy::Fixed );
    setPreferredSize( 80, 100 );
}

Control::Control( const char* colorName, qreal aspectRatio, QQuickItem* parent )
    : QskControl( parent )
    , m_aspectRatio( aspectRatio )
{
    setObjectName( colorName );

    setBackgroundColor( colorName );

    // setSizePolicy( QskSizePolicy::Constrained, QskSizePolicy::Ignored );
    setSizePolicy( QskSizePolicy::Constrained, QskSizePolicy::Fixed );
    setPreferredHeight( 100 );
}

qreal Control::heightForWidth( qreal width ) const
{
    return width / m_aspectRatio;
}

qreal Control::widthForHeight( qreal height ) const
{
    return height * m_aspectRatio;
}

void Control::transpose()
{
    m_aspectRatio = 1.0 / m_aspectRatio;
    setPreferredSize( preferredSize().transposed() );

    setSizePolicy( sizePolicy().verticalPolicy(), sizePolicy().horizontalPolicy() );
}

Box::Box( QQuickItem* parent )
    : QskLinearBox( Qt::Horizontal, 2, parent )
{
    setObjectName( "Box" );

    setBackgroundColor( Qt::white );

    setMargins( 10 );
    setSpacing( 5 );

    addControl( new Control( "Cyan", 2.0 / 3.0 ) );
    addControl( new Control( "DarkCyan" ) );

    addControl( new Control( "Blue", 1.0 ) );
    addControl( new Control( "DarkBlue" ) );

    addControl( new Control( "Red", 3.0 / 2.0 ) );
    addControl( new Control( "DarkRed" ) );
}

void Box::flip()
{
    setActive( false );

    for ( int i = 0; i < itemCount(); i++ )
    {
        if ( Control* control = dynamic_cast< Control* >( itemAtIndex( i ) ) )
            control->transpose();
    }

    transpose();
    setActive( true );
}

void Box::addControl( Control* control )
{
    addItem( control, Qt::AlignCenter );
}

DynamicConstraintsPage::DynamicConstraintsPage( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    setMargins( 10 );
    setBackgroundColor( QskRgbValue::LightSteelBlue );

    Box* box = new Box();

    QskPushButton* button = new QskPushButton( "Flip" );
    button->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
    QObject::connect( button, &QskPushButton::clicked, box, &Box::flip );

    addItem( button, Qt::AlignTop | Qt::AlignLeft );
    addItem( box );
}
