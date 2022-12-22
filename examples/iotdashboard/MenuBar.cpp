/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MenuBar.h"

QSK_SUBCONTROL( MenuBarTopLabel, Graphic )

QSK_SUBCONTROL( MenuButton, Panel )
QSK_SUBCONTROL( MenuButton, Text )
QSK_SUBCONTROL( MenuButton, Graphic )

QSK_SUBCONTROL( MenuBar, Panel )

MenuButton::MenuButton( const QString& name, QQuickItem* parent )
    : QskPushButton( name, parent )
{
    setCheckable( true );
    initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

    setSubcontrolProxy( QskPushButton::Panel, MenuButton::Panel );
    setSubcontrolProxy( QskPushButton::Text, MenuButton::Text );
    setSubcontrolProxy( QskPushButton::Graphic, MenuButton::Graphic );

    setGraphicSource( name );
}

MenuBar::MenuBar( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
    , m_currentIndex( 0 )
{
    setPanel( true );
    setSubcontrolProxy( QskBox::Panel, MenuBar::Panel );

    initSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Preferred );
    setSpacing( 0 );

    auto graphicLabel = new MenuBarTopLabel( "main-icon", this );
    graphicLabel->setMargins( marginHint( MenuBarTopLabel::Graphic ) );
    graphicLabel->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

    m_entryStrings = { "Dashboard", "Rooms", "Devices", "Statistics", "Storage", "Members" };

    for( int i = 0; i < m_entryStrings.count(); ++i )
    {
        auto* button = new MenuButton( m_entryStrings.at( i ), this );

        connect( button, &QskPushButton::pressed, this, [ this, i ]()
        {
            Q_EMIT pageChangeRequested( i );
        } );

        m_entries.append( button );
    }

    m_entries.at( m_currentIndex )->setChecked( true );

    addSpacer( 0, 1 ); // fill the space at the bottom

    new MenuButton( "Logout", this );
}

void MenuBar::setActivePage( const int index )
{
    m_entries.at( m_currentIndex )->setChecked( false );
    m_currentIndex = index;
    m_entries.at( m_currentIndex )->setChecked( true );
}

#include "moc_MenuBar.cpp"
