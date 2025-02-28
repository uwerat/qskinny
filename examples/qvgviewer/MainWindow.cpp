/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
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
#include <QskSkinManager.h>
#include <QskSkin.h>
#include <QskBoxShapeMetrics.h>

#include <QDir>
#include <QVariant>

class GraphicLabel : public QskGraphicLabel
{
  public:
    enum Role
    {
        Normal = 1000,
        Inverted
    };

    GraphicLabel( QQuickItem* parent = nullptr )
        : QskGraphicLabel( parent )
    {
        setMargins( 10 );
        setPanel( true );

        setBoxShapeHint( Panel, 8 );
        setAlignment( Qt::AlignCenter );
        setDarknessMode( false );
    }

    void setDarknessMode( bool on )
    {
        const int oldRole = graphicRole();

        QskGradient gradient;
        if ( on )
        {
            gradient = QskGradient( qRgb( 40, 40, 40 ) );
            setGraphicRole( Inverted );
        }
        else
        {
            gradient = QskGradient( QskRgb::Wheat );
            setGraphicRole( Normal );
        }

        const int duration = 500;

        const auto oldGradient = gradientHint( Panel );
        setGradientHint( Panel, gradient );

        // finally setup a smooth transition manually
        startTransition( Panel | QskAspect::Color, duration,
            QVariant::fromValue( oldGradient ), QVariant::fromValue( gradient ) );

        startTransition( Graphic | QskAspect::GraphicRole,
            duration, oldRole, graphicRole() );
    }
};

MainWindow::MainWindow()
{
    auto label = new GraphicLabel();

    auto invertButton = new QskPushButton( "Inverted" );
    invertButton->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
    invertButton->setCheckable( true );
    invertButton->setLayoutAlignmentHint( Qt::AlignRight );

    auto box = new QskLinearBox( Qt::Vertical );
    box->setPanel( true );
    box->setPadding( 5 );
    box->addItem( invertButton );
    box->addItem( label );

    addItem( box );

    connect( invertButton, &QskPushButton::toggled,
        label, &GraphicLabel::setDarknessMode );

    connect( qskSkinManager, &QskSkinManager::skinChanged,
        this, &MainWindow::setGraphicRoles );

    setGraphicRoles( qskSkinManager->skin() );
}

void MainWindow::setGraphic( const QString& path )
{
    if ( auto label = findChild< QskGraphicLabel* >() )
        label->setGraphic( QskGraphicIO::read( path ) );
}

void MainWindow::setGraphicRoles( QskSkin* skin )
{
    // substituting black
    QskColorFilter colorFilter;
    colorFilter.addColorSubstitution( QskRgb::Black, QskRgb::DarkKhaki );

    skin->setGraphicFilter( GraphicLabel::Inverted, colorFilter );
}

#include "moc_MainWindow.cpp"
