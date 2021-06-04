/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

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

#include "moc_TopBar.cpp"
