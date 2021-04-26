/****************************************************************************
**
** Copyright 2021 Edelhirsch Software GmbH. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of the copyright holder nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/

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
