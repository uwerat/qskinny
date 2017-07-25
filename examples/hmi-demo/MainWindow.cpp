#include "MainWindow.h"
#include "RadioControl.h"
#include "SoundControl.h"

#include <QskGraphic.h>
#include <QskGraphicLabel.h>
#include <QskLinearBox.h>
#include <QskTextLabel.h>

#include <QDate>
#include <QImage>

namespace
{
    class ButtonBar : public QskLinearBox
    {
    public:
        ButtonBar( QQuickItem* parentItem = nullptr ):
            QskLinearBox( parentItem )
        {
            setOpacity( 0.5 );
            setBackgroundColor( Qt::black );
            setMargins( QMarginsF( 20, 0, 20, 0 ) );

            setSizePolicy( QskSizePolicy::MinimumExpanding, QskSizePolicy::Fixed );
        }

        void addIcon( const char* name )
        {
            const QString fileName = QString( ":/images/%1" ).arg( name );

            auto* label = new QskGraphicLabel( this );
            label->setFixedSize( 76, 36 );
            label->setMargins( QMarginsF( 20, 7, 20, 7 ) );
            label->setGraphic( QskGraphic::fromImage( QImage( fileName ) ) );
        }

    protected:
        virtual QSizeF contentsSizeHint() const override final
        {
            return QSizeF( -1, 50 );
        }
    };
}

MainWindow::MainWindow()
{
    setPreferredSize( QSize( 1024, 576 ) );
    setAutoLayoutChildren( true );

    populate();
}

void MainWindow::populate()
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
    layout->addItem( content );
    layout->addItem( footer );
}

QQuickItem* MainWindow::headerBar() const
{
    auto* header = new ButtonBar();
    header->addIcon( "ic_pan_tool_white_48dp_2x.png" );
    header->addIcon( "ic_star_rate_white_18dp_2x.png" );
    header->addIcon( "ic_airplanemode_active_white_18dp_2x.png" );

    auto dateLabel = new QskTextLabel( QDate::currentDate().toString(), header );
    dateLabel->setColor( QskTextLabel::Text, Qt::white );

    header->addIcon( "ic_face_white_48px.svg" );
    header->addIcon( "ic_extension_white_48dp_2x.png" );
    header->addIcon( "ic_build_white_24dp_2x.png" );

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

    footer->addIcon( "ic_pan_tool_white_48dp_2x.png" );
    footer->addIcon( "ic_star_rate_white_18dp_2x.png" );
    footer->addIcon( "ic_airplanemode_active_white_18dp_2x.png" );
    footer->addStretch( 10 );

    return footer;
}
