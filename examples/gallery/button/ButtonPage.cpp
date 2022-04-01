/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "ButtonPage.h"

#include <QskSwitchButton.h>
#include <QskLinearBox.h>
#include <QskTextLabel.h>
#include <QskSeparator.h>

#include <QskRgbValue.h>

ButtonPage::ButtonPage( QQuickItem* parent )
    : Page( Qt::Horizontal, parent )
{
    setSpacing( 40 );
    populate();
}

void ButtonPage::populate()
{
    auto hbox1 = new QskLinearBox();
    hbox1->setSizePolicy( Qt::Vertical, QskSizePolicy::Fixed );
    hbox1->setExtraSpacingAt( Qt::LeftEdge );

    auto label = new QskTextLabel( "Disable the switches:", hbox1 );
    label->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

    auto button0 = new QskSwitchButton( hbox1 );

    auto hbox2 = new QskLinearBox( Qt::Horizontal );
    hbox2->setDefaultAlignment( Qt::AlignHCenter | Qt::AlignTop );
    hbox2->setMargins( 30 );

    (void) new QskSwitchButton( Qt::Vertical, hbox2 );
    (void) new QskSwitchButton( Qt::Horizontal, hbox2 );

    auto button3 = new QskSwitchButton( Qt::Vertical, hbox2 );
    button3->setInverted( true );

    auto button4 = new QskSwitchButton( Qt::Horizontal, hbox2 );
    button4->setInverted( true );

    auto vbox = new QskLinearBox( Qt::Vertical, this );
    vbox->addItem( hbox1 );
    vbox->addItem( new QskSeparator() );
    vbox->addItem( hbox2 );
    vbox->setExtraSpacingAt( Qt::BottomEdge );

    QObject::connect( button0, &QskSwitchButton::checkedChanged,
        hbox2, &QskQuickItem::setDisabled );
}
