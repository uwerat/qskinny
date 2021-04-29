/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MenuBar.h"

#include <QskGraphic.h>
#include <QskGraphicLabel.h>
#include <QskGraphicIO.h>
#include <QskSkin.h>
#include <QskTextLabel.h>

#include <QtGui/QImage>

QSK_SUBCONTROL( MenuBarTopLabel, Graphic )
QSK_SUBCONTROL( MenuBarGraphicLabel, Graphic )
QSK_SUBCONTROL( MenuBarLabel, Text )
QSK_SUBCONTROL( MenuItem, Panel )
QSK_SUBCONTROL( MenuBar, Panel )

QSK_STATE( MenuItem, Active, ( QskAspect::FirstUserState << 1 ) )

MenuItem::MenuItem( const QString& name, QQuickItem* parent ) : QskLinearBox( Qt::Horizontal, parent ),
    m_name( name )
{
    setAutoLayoutChildren( true );
    setAutoAddChildren( true );
    setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
    setSpacing( 6 );

    setAcceptHoverEvents( true );
    setPanel( true );

    QString fileName = ":/images/" + name.toLower() + ".png";
    QImage image( fileName );
    auto graphic = QskGraphic::fromImage( image );
    auto* graphicLabel = new MenuBarGraphicLabel( graphic, this );
    graphicLabel->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
    graphicLabel->setFixedWidth( metric( MenuBarGraphicLabel::Graphic | QskAspect::Size ) );

    new MenuBarLabel( name, this );
}

QskAspect::Subcontrol MenuItem::effectiveSubcontrol( QskAspect::Subcontrol subControl ) const
{
    if( subControl == QskBox::Panel )
    {
        return MenuItem::Panel;
    }

    return subControl;
}


MenuBar::MenuBar( QQuickItem* parent ) : QskLinearBox( Qt::Vertical, parent )
{
    setPanel( true );
    setSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Preferred );
    setSpacing( 8 );

    auto* mainIcon = ":/images/main-icon.png";
    QImage image( mainIcon );
    auto graphic = QskGraphic::fromImage( image );
    auto* graphicLabel = new MenuBarTopLabel( graphic, this );
    graphicLabel->setMargins( marginHint( MenuBarTopLabel::Graphic ) );
    graphicLabel->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

    m_entryStrings = { "Dashboard", "Rooms", "Devices", "Statistics", "Storage", "Members" };

    for( const auto& entryString : qskAsConst( m_entryStrings ) )
    {
        auto* entry = new MenuItem( entryString, this );
        m_entries.append( entry );
    }

    m_entries.at( m_activeEntry )->setSkinState( MenuItem::Active );

    addSpacer( 0, 1 ); // fill the space at the bottom

    new MenuItem( "Logout", this );
}

QskAspect::Subcontrol MenuBar::effectiveSubcontrol( QskAspect::Subcontrol subControl ) const
{
    if( subControl == QskBox::Panel )
    {
        return MenuBar::Panel;
    }

    return subControl;
}

#include "moc_MenuBar.cpp"
