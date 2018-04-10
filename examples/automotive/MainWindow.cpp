#include "MainWindow.h"
#include "ButtonBar.h"
#include "SkinFactory.h"
#include "SoundControl.h"
#include "SpeedometerDisplay.h"

#include <QskGraphic.h>
#include <QskGraphicIO.h>
#include <QskGraphicLabel.h>
#include <QskLinearBox.h>
#include <QskTextLabel.h>

#include <QDate>
#include <QImage>

MainWindow::MainWindow() :
    m_layout( nullptr )
{
    const QImage image( QStringLiteral( ":/images/background.jpg" ) );

    auto backgroundImage = new QskGraphicLabel( contentItem() );
    backgroundImage->setGraphic( QskGraphic::fromImage( image ) );
    backgroundImage->setFillMode( QskGraphicLabel::Stretch );

    m_layout = new QskLinearBox( Qt::Vertical, contentItem() );

    auto header = headerBar();
    auto content = mainContent();
    auto footer = footerBar();

    m_layout->addItem( header );
    m_layout->setStretchFactor( header, 1 );

    m_layout->addItem( content );
    m_layout->setStretchFactor( content, 10 );

    m_layout->addItem( footer );
    m_layout->setStretchFactor( footer, 1 );

    setAutoLayoutChildren( true );
}

QQuickItem* MainWindow::headerBar() const
{
    auto* header = new ButtonBar( m_layout );
    header->addIndicator( "bluetooth" );
    header->addIndicator( "location" );
    header->addIndicator( "phone" );

    (void) new QskTextLabel( QDate::currentDate().toString(), header );

    header->addIndicator( "user" );
    header->addIndicator( "bookmark" );
    header->addIndicator( "menu" );

    return header;
}

QQuickItem* MainWindow::mainContent() const
{
    return new SpeedometerDisplay( m_layout );
    //return new SoundControl(); ###
}

QQuickItem* MainWindow::footerBar() const
{
    auto* footer = new ButtonBar( m_layout );

    footer->addIndicator( "cloud" );
    footer->addIndicator( "man" );
    footer->addIndicator( "bus" );
    footer->addIndicator( "plane" );
    footer->addIndicator( "train" );

    footer->addStretch( 10 );

    return footer;
}
