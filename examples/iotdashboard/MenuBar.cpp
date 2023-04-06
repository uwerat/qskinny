/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Claus
 *****************************************************************************/

#include "MenuBar.h"

#include <QskGraphicLabel.h>
#include <QskPushButton.h>

QSK_SUBCONTROL( MenuBar, Panel )

namespace
{
    class Button final : public QskPushButton
    {
      public:
        Button( const QString& name, QQuickItem* parent = nullptr )
            : QskPushButton( name, parent )
        {
            initSizePolicy( QskSizePolicy::MinimumExpanding, QskSizePolicy::Fixed );

            setCheckable( true );
            setExclusive( true );
            setIconSource( name );
        }
    };

    class Logo final : public QskGraphicLabel
    {
      public:
        Logo( QQuickItem* parent = nullptr )
            : QskGraphicLabel( "main-icon", parent )
        {
            setGraphicStrutSize( QSizeF( 40, -1 ) );
            setFillMode( QskGraphicLabel::Pad );
            setAlignment( Qt::AlignCenter );

            initSizePolicy( QskSizePolicy::Ignored, QskSizePolicy::Fixed );
        }
    };
}

MenuBar::MenuBar( QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
    , m_currentIndex( Cube::FrontPos )
{
    setSection( QskAspect::Header );

    setPanel( true );
    setSubcontrolProxy( QskBox::Panel, MenuBar::Panel );

    initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Preferred );
    setSpacing( 0 );

    populate();
}

void MenuBar::populate()
{
    auto logo = new Logo();
    auto logoutButton = new Button( "Logout");

    const QPair< Cube::Position, QString > entries[] =
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
        auto button = new Button( entry.second );

        const int id = entry.first;
        m_buttons[ id ] = button;

        connect( button, &QskPushButton::pressed,
            this, [ this, id ]() { Q_EMIT pageChangeRequested( id ); } );
    }

    addSpacer( 35 );
    addItem( logo );
    addSpacer( 60 );

    for ( auto button : m_buttons )
        addItem( button );

    addStretch( 1 ); // fill the space at the bottom
    addItem( logoutButton );
}

void MenuBar::setActivePage( const int index )
{
    m_currentIndex = index;
    m_buttons[ m_currentIndex ]->setChecked( true );
}

#include "moc_MenuBar.cpp"
