#include "MainWindow.h"
#include "RadioControl.h"
#include "SoundControl.h"

#include <QskGraphic.h>
#include <QskGraphicLabel.h>
#include <QskLinearBox.h>
#include <QskPushButton.h>
#include <QskSkin.h>
#include <QskTextLabel.h>

#include <QDate>
#include <QTimer>

namespace {
    void addIcon( QskLayout* layout, const QString& fileName )
    {
        QImage buildIcon( fileName );
        QskGraphic graphic = QskGraphic::fromImage( buildIcon );
        QskGraphicLabel* buildLabel = new QskGraphicLabel( layout );
        buildLabel->setFixedSize( 76, 36 );
        buildLabel->setMargins( QMarginsF( 20, 7, 20, 7 ) );
        buildLabel->setGraphic( graphic );
    }
}

MainWindow::MainWindow() : QskWindow()
  , m_backgroundImage( new QskGraphicLabel( contentItem() ) )
  , m_layout( new QskLinearBox( Qt::Vertical, contentItem() ) )
{
    setPreferredSize( QSize( 1024, 576 ) );
    setAutoLayoutChildren( true );

    // later:
//    QFont font( "Roboto" );
//    font.setPointSize( 20 );
//    setFont( QskSkin::DefaultFont, 20 );

    QImage image( ":/images/background.jpg" );
    QskGraphic graphic = QskGraphic::fromImage( image );
    m_backgroundImage->setGraphic( graphic );

    m_layout->setAutoAddChildren( true );

    addHeaderBar();
    addMainContent();
    addBottomBar();
}

void MainWindow::addHeaderBar()
{
    QskPushButton* headerBar = new QskPushButton( m_layout );
    headerBar->setEnabled( false );
    headerBar->setPosition( QPointF( 0, 0 ) );
    headerBar->setOpacity( 0.5 );
    headerBar->setBackgroundColor( Qt::black );
    headerBar->setFixedHeight( 50 );
    headerBar->setFlat( true );
    headerBar->setAutoLayoutChildren( true );

    QskLinearBox* headerLayout = new QskLinearBox( headerBar );
    headerLayout->setMargins( QMarginsF( 20, 0, 20, 0 ) );

    addIcon( headerLayout, ":/images/ic_pan_tool_white_48dp_2x.png" );
    addIcon( headerLayout, ":/images/ic_star_rate_white_18dp_2x.png" );
    addIcon( headerLayout, ":/images/ic_airplanemode_active_white_18dp_2x.png" );

    QDate currentDate = QDate::currentDate();
    QskTextLabel* dateLabel = new QskTextLabel( currentDate.toString(), headerLayout );
    dateLabel->setColor( QskTextLabel::Text, Qt::white );

    addIcon( headerLayout, ":/images/ic_face_white_48px.svg" );
    addIcon( headerLayout, ":/images/ic_extension_white_48dp_2x.png" );
    addIcon( headerLayout, ":/images/ic_build_white_24dp_2x.png" );
}

void MainWindow::addMainContent()
{
//    RadioControl* radioControl = new RadioControl( m_layout );
//    m_layout->setRetainSizeWhenHidden( radioControl, true );
//    radioControl->setVisible( false );

    SoundControl* soundControl = new SoundControl( m_layout );
}

void MainWindow::addBottomBar()
{
    QskPushButton* bottomBar = new QskPushButton( m_layout );
    bottomBar->setEnabled( false );
    bottomBar->setPosition( QPointF( 0, 0 ) );
    bottomBar->setOpacity( 0.5 );
    bottomBar->setBackgroundColor( Qt::black );
    bottomBar->setFixedHeight( 50 );
    bottomBar->setFlat( true );

    QskLinearBox* bottomLayout = new QskLinearBox( bottomBar );
    bottomLayout->setMargins( QMarginsF( 20, 0, 20, 0 ) );

    addIcon( bottomLayout, ":/images/ic_pan_tool_white_48dp_2x.png" );
    addIcon( bottomLayout, ":/images/ic_star_rate_white_18dp_2x.png" );
    addIcon( bottomLayout, ":/images/ic_airplanemode_active_white_18dp_2x.png" );
}
