/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MenuBar.h"

#include <QTimer>

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
    , m_currentIndex( Cube::FrontPos )
{
    setPanel( true );
    setSubcontrolProxy( QskBox::Panel, MenuBar::Panel );

    initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Preferred );
    setSpacing( 0 );

    auto graphicLabel = new MenuBarTopLabel( "main-icon", this );
    graphicLabel->setMargins( marginHint( MenuBarTopLabel::Graphic ) );
    graphicLabel->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

    // ### unify the information with the one from MainItem
    const QVector< QPair< Cube::Position, QString > > entries =
    {
        { Cube::FrontPos, "Dashboard" },
        { Cube::RightPos, "Rooms" },
        { Cube::BackPos, "Devices" },
        { Cube::LeftPos, "Statistics" },
        { Cube::TopPos, "Storage" },
        { Cube::BottomPos, "Members" },
    };

    for( const auto& entry : entries )
    {
        auto* button = new MenuButton( entry.second, this );
        m_buttons[ entry.first ] = button;

        connect( button, &QskPushButton::pressed, this, [ this, entry ]()
        {
            for( auto* button : qAsConst( m_buttons ) )
            {
                // the right button will be set to checked after this
                button->setChecked( false );
            }

            Q_EMIT pageChangeRequested( entry.first );
        } );
    }

    addSpacer( 0, 1 ); // fill the space at the bottom

    new MenuButton( "Logout", this );
}

void MenuBar::setActivePage( const int index )
{
    m_buttons[ m_currentIndex ]->setChecked( false );
    m_currentIndex = index;

    QTimer::singleShot( 0, this, [this]()
    {
        m_buttons[ m_currentIndex ]->setChecked( true );
    } );
}

#include "moc_MenuBar.cpp"
