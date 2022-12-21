/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MenuBar.h"

QSK_SUBCONTROL( MenuBarTopLabel, Graphic )
QSK_SUBCONTROL( MenuBarGraphicLabel, Graphic )
QSK_SUBCONTROL( MenuBarLabel, Text )
QSK_SUBCONTROL( MenuItem, Panel )
QSK_SUBCONTROL( MenuBar, Panel )

QSK_STATE( MenuItem, Active, ( QskAspect::FirstUserState << 1 ) )

MenuItem::MenuItem( const QString& name, QQuickItem* parent )
    : QskLinearBox( Qt::Horizontal, parent )
{
    initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
    setSpacing( 6 );

    setAcceptHoverEvents( true );

    setPanel( true );
    setSubcontrolProxy( QskBox::Panel, MenuItem::Panel );

    auto graphicLabel = new MenuBarGraphicLabel( name, this );
    graphicLabel->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
    graphicLabel->setFixedWidth( metric( MenuBarGraphicLabel::Graphic | QskAspect::Size ) );

    new MenuBarLabel( name, this );
}

MenuBar::MenuBar( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
{
    setPanel( true );
    setSubcontrolProxy( QskBox::Panel, MenuBar::Panel );

    initSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Preferred );
    setSpacing( 0 );

    auto graphicLabel = new MenuBarTopLabel( "main-icon", this );
    graphicLabel->setMargins( marginHint( MenuBarTopLabel::Graphic ) );
    graphicLabel->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

    m_entryStrings = { "Dashboard", "Rooms", "Devices", "Statistics", "Storage", "Members" };

    for( const auto& entryString : qAsConst( m_entryStrings ) )
    {
        auto* entry = new MenuItem( entryString, this );
        m_entries.append( entry );
    }

    m_entries.at( m_activeEntry )->setSkinStateFlag( MenuItem::Active );

    addSpacer( 0, 1 ); // fill the space at the bottom

    new MenuItem( "Logout", this );
}

#include "moc_MenuBar.cpp"
