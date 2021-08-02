/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "SwitchButtonPage.h"

#include <QskSwitchButton.h>
#include <QskLinearBox.h>
#include <QskTextLabel.h>

#include <QskRgbValue.h>

SwitchButtonPage::SwitchButtonPage( QQuickItem* parent )
    : Page( Qt::Horizontal, parent )
{
    setGradient( QskRgb::AliceBlue );
    setSpacing( 40 );

    populate();
}

void SwitchButtonPage::populate()
{
    auto vbox = new QskLinearBox( Qt::Vertical, this );

    auto hbox = new QskLinearBox( vbox );
    new QskTextLabel( "Disable the boxes: ", hbox );
    auto disabler = new QskSwitchButton( hbox );

    auto targets = new QskLinearBox( Qt::Horizontal, vbox );

    auto target1 = new QskSwitchButton( targets );
    target1->setOrientation( Qt::Vertical );

    auto target2 = new QskSwitchButton( targets );
    target2->setOrientation( Qt::Horizontal );

    auto target3 = new QskSwitchButton( targets );
    target3->setChecked( true );
    target3->setOrientation( Qt::Vertical );

    auto target4 = new QskSwitchButton( targets );
    target4->setChecked( true );
    target4->setOrientation( Qt::Horizontal );

    QObject::connect( disabler, &QskSwitchButton::checkedChanged,
        targets, &QskQuickItem::setDisabled );

    targets->setExtraSpacingAt( Qt::RightEdge );
    vbox->setExtraSpacingAt( Qt::BottomEdge );
}
