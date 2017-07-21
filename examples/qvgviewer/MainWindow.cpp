/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MainWindow.h"

#include <QskGraphic.h>
#include <QskGraphicLabel.h>
#include <QskGraphicIO.h>
#include <QskPushButton.h>
#include <QskLinearBox.h>
#include <QskColorFilter.h>
#include <QskAspect.h>
#include <QskTabView.h>
#include <QskSetup.h>
#include <QskSkin.h>
#include <QskAnimationHint.h>

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

    GraphicLabel( const QskGraphic& graphic, QQuickItem* parent = nullptr ):
        QskGraphicLabel( graphic, parent )
    {
        setAlignment( Qt::AlignCenter );
        setAutoFillBackground( true );

        setDarknessMode( false );
    }

    void setDarknessMode( bool on )
    {
        using namespace QskAspect;

        const QskAspect::Aspect aspectRole = QskGraphicLabel::Graphic | GraphicRole;

        const QVariant oldRole = storedSkinHint( aspectRole );

        QRgb rgb;
        if ( on )
        {
            rgb = qRgb( 40, 40, 40 );
            setFlagHint( aspectRole, Inverted );
        }
        else
        {
            rgb = qRgb( 255, 228, 181 );
            setFlagHint( aspectRole, Normal );
        }

        const int duration = 500;

        for ( auto edge : { TopEdge, BottomEdge } )
        {
            const Aspect aspectColor = Control | Color | Background | edge;

            const auto oldColor = storedSkinHint( aspectColor );
            setColor( aspectColor, rgb );
            startTransition( aspectColor, duration, oldColor, storedSkinHint( aspectColor ) );
        }

        startTransition( aspectRole, duration, oldRole, storedSkinHint( aspectRole ) );
    }
};

MainWindow::MainWindow()
{
    m_tabView = new QskTabView( Qt::Horizontal );

    const QString resourceDir( ":/qvg" );
    const QStringList icons = QDir( resourceDir ).entryList();

    for ( auto icon : icons )
    {
        m_tabView->addTab( icon.replace( ".qvg", "" ),
            new GraphicLabel( QskGraphicIO::read( resourceDir + "/" + icon ) ) );
    }

    auto invertButton = new QskPushButton( "Inverted" );
    invertButton->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
    invertButton->setCheckable( true );

    auto box = new QskLinearBox( Qt::Vertical );
    box->setMargins( 5 );
    box->addItem( invertButton, Qt::AlignRight );
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
