#include "MainWindow.h"
#include "ButtonBar.h"
#include "RadioControl.h"
#include "SoundControl.h"
#include "SkinFactory.h"

#include <QskGraphic.h>
#include <QskGraphicIO.h>
#include <QskGraphicLabel.h>
#include <QskLinearBox.h>
#include <QskTextLabel.h>

#include <QDate>
#include <QImage>

MainWindow::MainWindow()
{
    const QImage image( ":/images/background.jpg" );

    auto backgroundImage = new QskGraphicLabel( contentItem() );
    backgroundImage->setGraphic( QskGraphic::fromImage( image ) );
    backgroundImage->setFillMode( QskGraphicLabel::Stretch );

    auto header = headerBar();
    auto content = mainContent();
    auto footer = footerBar();

    auto layout = new QskLinearBox( Qt::Vertical, contentItem() );

    layout->addItem( header );
    layout->setStretchFactor( header, 1 );

    layout->addItem( content );
    layout->setStretchFactor( content, 10 );

    layout->addItem( footer );
    layout->setStretchFactor( footer, 1 );

    setAutoLayoutChildren( true );
}

QQuickItem* MainWindow::headerBar() const
{
    auto* header = new ButtonBar();
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
//    RadioControl* radioControl = new RadioControl( m_layout );
//    m_layout->setRetainSizeWhenHidden( radioControl, true );
//    radioControl->setVisible( false );

    return new SoundControl();
}

QQuickItem* MainWindow::footerBar() const
{
    auto* footer = new ButtonBar();

    footer->addIndicator( "cloud" );
    footer->addIndicator( "man" );
    footer->addIndicator( "bus" );
    footer->addIndicator( "plane" );
    footer->addIndicator( "train" );

    footer->addStretch( 10 );

    return footer;
}
