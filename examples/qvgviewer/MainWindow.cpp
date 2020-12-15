/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MainWindow.h"

#include <QskAnimationHint.h>
#include <QskAspect.h>
#include <QskColorFilter.h>
#include <QskGradient.h>
#include <QskGraphic.h>
#include <QskGraphicIO.h>
#include <QskGraphicLabel.h>
#include <QskLinearBox.h>
#include <QskPushButton.h>
#include <QskRgbValue.h>
#include <QskSetup.h>
#include <QskSkin.h>
#include <QskTabView.h>

#include <QDir>
#include <QVariant>

class GraphicLabel : public QskGraphicLabel
{
  public:
    enum Role
    {
        Normal,
        Inverted
    };

    GraphicLabel( const QskGraphic& graphic, QQuickItem* parent = nullptr )
        : QskGraphicLabel( graphic, parent )
    {
        setAlignment( Qt::AlignCenter );
        setAutoFillBackground( true );

        setDarknessMode( false );
    }

    void setDarknessMode( bool on )
    {
        const int oldRole = graphicRole();

        QskGradient gradient;
        if ( on )
        {
            gradient.setColor( qRgb( 40, 40, 40 ) );
            setGraphicRole( Inverted );
        }
        else
        {
            gradient.setColor( QskRgb::Wheat );
            setGraphicRole( Normal );
        }

        const int duration = 500;

        const QskGradient oldGradient = background();

        setBackground( gradient );

        // finally setup a smooth transition manually
        startTransition( QskAspect::Control | QskAspect::Color, duration,
            QVariant::fromValue( oldGradient ), QVariant::fromValue( gradient ) );

        startTransition( QskGraphicLabel::Graphic | QskAspect::GraphicRole,
            duration, oldRole, graphicRole() );
    }
};

MainWindow::MainWindow()
{
    m_tabView = new QskTabView( Qsk::Left );

    const QString resourceDir( ":/qvg" );
    const QStringList icons = QDir( resourceDir ).entryList();

    for ( const auto& icon : icons )
    {
        auto title = icon;
        m_tabView->addTab( title.replace( ".qvg", "" ),
            new GraphicLabel( QskGraphicIO::read( resourceDir + "/" + icon ) ) );
    }

    auto invertButton = new QskPushButton( "Inverted" );
    invertButton->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
    invertButton->setCheckable( true );
    invertButton->setLayoutAlignmentHint( Qt::AlignRight );

    auto box = new QskLinearBox( Qt::Vertical );
    box->setMargins( 5 );
    box->addItem( invertButton );
    box->addItem( m_tabView );

    addItem( box );

    connect( invertButton, &QskPushButton::toggled,
        this, &MainWindow::setDarknessMode );

    connect( qskSetup, &QskSetup::skinChanged,
        this, &MainWindow::setGraphicRoles );

    setGraphicRoles( qskSetup->skin() );
}

void MainWindow::setDarknessMode( bool on )
{
    for ( int i = 0; i < m_tabView->count(); i++ )
    {
        auto label = static_cast< GraphicLabel* >( m_tabView->itemAt( i ) );
        label->setDarknessMode( on );
    }
}

void MainWindow::setGraphicRoles( QskSkin* skin )
{
    // substituting black and white
    QskColorFilter colorFilter;
    colorFilter.addColorSubstitution( qRgb( 0, 0, 0 ), qRgb( 189, 183, 107 ) );
    colorFilter.addColorSubstitution( qRgb( 255, 255, 255 ), qRgb( 50, 50, 50 ) );

    skin->setGraphicFilter( GraphicLabel::Inverted, colorFilter );
}

#include "moc_MainWindow.cpp"
