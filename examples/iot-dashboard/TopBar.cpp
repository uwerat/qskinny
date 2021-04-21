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

#include "TopBar.h"
#include "PieChartPainted.h"

#include <QskSkin.h>
#include <QskTextLabel.h>

#include <QTime>

QSK_SUBCONTROL( TimeTitleLabel, Text )

QSK_SUBCONTROL( TimeLabel, Text )

QSK_SUBCONTROL( TopBarItem, Item1 )
QSK_SUBCONTROL( TopBarItem, Item2 )
QSK_SUBCONTROL( TopBarItem, Item3 )
QSK_SUBCONTROL( TopBarItem, Item4 )

QSK_SUBCONTROL( TopBar, Panel )

namespace
{
    QskAspect::Subcontrol subcontrolForIndex( int index )
    {
        switch( index )
        {
            case 0:
                return TopBarItem::Item1;

            case 1:
                return TopBarItem::Item2;

            case 2:
                return TopBarItem::Item3;

            default:
                return TopBarItem::Item4;
        }
    }
}

TopBarItem::TopBarItem( int index, const QString& name, const QskGradient& gradient, int progress, int value, QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
    , m_name( name )
{
    setAutoLayoutChildren( true );
    setAutoAddChildren( true );
    setSpacing( 15 );

    auto* textLabel = new QskTextLabel( name, this );
    textLabel->setFontRole( QskSkin::SmallFont );

    auto* pieChartAndDisplay = new QskLinearBox( Qt::Horizontal, this );
    pieChartAndDisplay->setSpacing( 10 );

    QskAspect::Subcontrol subcontrol = subcontrolForIndex( index );
    QColor textColor = color( subcontrol | QskAspect::TextColor );
    new PieChartPainted( textColor, gradient, progress, value, pieChartAndDisplay );

    auto* display = new QskLinearBox( Qt::Vertical, pieChartAndDisplay );
    display->setSpacing( 0 );
    display->addSpacer( 0, 1 );

    auto* displayValue = new QskTextLabel( QString::number( value ), display );
    displayValue->setFontRole( QskSkin::MediumFont );

    auto* displayUnit = new QskTextLabel( "kwH", display );
    displayUnit->setFontRole( QskSkin::SmallFont );
    display->addSpacer( 0, 1 );

}

TopBar::TopBar( QQuickItem* parent ) : QskLinearBox( Qt::Horizontal, parent )
{
    setPanel( true );
    setAutoLayoutChildren( true );
    setAutoAddChildren( true );
    setSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Fixed );

    QStringList itemStrings = { "Living Room", "Bedroom", "Bathroom", "Kitchen" };
    int progressValues[] = {25, 45, 15, 86};
    int values[] = {175, 205, 115, 289};

    for( int i = 0; i < itemStrings.count(); i++ )
    {
        QskAspect::Subcontrol subcontrol = subcontrolForIndex( i );
        QskGradient gradient = gradientHint( subcontrol );

        auto* item = new TopBarItem( i, itemStrings.at( i ), gradient, progressValues[i], values[i], this );
        m_entries.append( item );
    }

    auto* timeControl = new QskLinearBox( Qt::Vertical, this );
    new TimeTitleLabel( "Current time", timeControl );

    auto now = QTime::currentTime();
    auto timeString = now.toString();
    new TimeLabel( timeString, timeControl );
}
