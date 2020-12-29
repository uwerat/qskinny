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

        void transpose();

      protected:
        QSizeF layoutSizeHint( Qt::SizeHint, const QSizeF& ) const override;

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

QSizeF Control::layoutSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which == Qt::PreferredSize )
    {
        if ( constraint.width() >= 0.0 )
            return QSizeF( -1.0, constraint.width() / m_aspectRatio );

        if ( constraint.height() >= 0.0 )
            return QSizeF( constraint.height() * m_aspectRatio, -1.0 );
    }

    return QSizeF();
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
    setDefaultAlignment( Qt::AlignCenter );

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
    for ( int i = 0; i < elementCount(); i++ )
    {
        if ( auto control = dynamic_cast< Control* >( itemAtIndex( i ) ) )
            control->transpose();
    }

    transpose();
}

void Box::addControl( Control* control )
{
    addItem( control );
}

DynamicConstraintsPage::DynamicConstraintsPage( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    setMargins( 10 );
    setBackgroundColor( QskRgb::LightSteelBlue );

    auto box = new Box();

    auto button = new QskPushButton( "Flip" );
    button->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
    button->setLayoutAlignmentHint( Qt::AlignTop | Qt::AlignLeft );

    QObject::connect( button, &QskPushButton::clicked, box, &Box::flip );

    addItem( button );
    addItem( box );
}
